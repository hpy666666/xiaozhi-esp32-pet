#include "pet_servo.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <cmath>

static const char* TAG = "PetServo";

// 舵机GPIO引脚定义 (GPIO9-12)
const int PetServo::servo_pins_[SERVO_COUNT] = {
    9,   // 左前腿
    10,  // 右前腿
    11,  // 左后腿
    12   // 右后腿
};

PetServo::PetServo() : timer_initialized_(false) {
    // 初始化角度缓存为90度（中位）
    for (int i = 0; i < SERVO_COUNT; i++) {
        current_angles_[i] = 90;
        last_move_time_us_[i] = 0;  // 初始化时间记录
    }
}

PetServo::~PetServo() {
    // 清理资源
}

void PetServo::Init() {
    ESP_LOGI(TAG, "Initializing pet servos on GPIO9-12");

    // 配置LEDC定时器（只需配置一次）
    if (!timer_initialized_) {
        ledc_timer_config_t ledc_timer = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = LEDC_TIMER_13_BIT,  // 13位分辨率 (0-8191)
            .timer_num = LEDC_TIMER_2,             // 使用定时器2（避免与音频冲突）
            .freq_hz = 50,                         // 50Hz标准舵机频率
            .clk_cfg = LEDC_AUTO_CLK
        };
        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
        timer_initialized_ = true;
        ESP_LOGI(TAG, "LEDC timer configured: 50Hz, 13-bit resolution");
    }

    // 配置每个舵机的LEDC通道
    for (int i = 0; i < SERVO_COUNT; i++) {
        ledc_channels_[i] = static_cast<ledc_channel_t>(i);  // 使用通道0-3

        ledc_channel_config_t ledc_channel = {
            .gpio_num = servo_pins_[i],
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = ledc_channels_[i],
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = LEDC_TIMER_2,
            .duty = 0,
            .hpoint = 0,
            .flags = {}
        };
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
        ESP_LOGI(TAG, "Servo %d initialized on GPIO%d (channel %d)",
                 i, servo_pins_[i], ledc_channels_[i]);
    }

    // 初始化所有舵机到90度（站立位置）- 分时启动避免电流冲击
    for (int i = 0; i < SERVO_COUNT; i++) {
        SetAngle(static_cast<ServoIndex>(i), 90);
        if (i < SERVO_COUNT - 1) {  // 最后一个不需要延迟
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }

    ESP_LOGI(TAG, "All servos initialized to 90 degrees (standing position)");
}

uint32_t PetServo::AngleToPulseWidth(uint8_t angle) {
    // 限制角度范围
    if (angle > SERVO_MAX_DEGREE) angle = SERVO_MAX_DEGREE;

    // 线性映射：0度->500us, 180度->2500us
    return SERVO_MIN_PULSEWIDTH_US +
           (angle * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / SERVO_MAX_DEGREE);
}

uint32_t PetServo::AngleToDuty(uint8_t angle) {
    uint32_t pulse_width = AngleToPulseWidth(angle);

    // 占空比计算：(脉宽 / 周期) * 分辨率
    // 13位分辨率 = 8192级，周期 = 20000us
    uint32_t duty = (pulse_width * 8192) / SERVO_TIMEBASE_PERIOD;

    return duty;
}

void PetServo::SetAngle(ServoIndex index, uint8_t angle) {
    if (index >= SERVO_COUNT) {
        ESP_LOGE(TAG, "Invalid servo index: %d", index);
        return;
    }

    // 限制角度范围
    if (angle > SERVO_MAX_DEGREE) angle = SERVO_MAX_DEGREE;

    // 自动节流:如果距离上次移动不足60ms,则等待
    // 这样可以避免多个舵机几乎同时启动导致电流冲击
    // MT3608升压模块需要足够的恢复时间
    int64_t now_us = esp_timer_get_time();
    int64_t elapsed_us = now_us - last_move_time_us_[index];
    const int64_t MIN_INTERVAL_US = 60000;  // 60ms最小间隔

    if (elapsed_us < MIN_INTERVAL_US && last_move_time_us_[index] != 0) {
        int64_t wait_us = MIN_INTERVAL_US - elapsed_us;
        vTaskDelay(pdMS_TO_TICKS(wait_us / 1000));
    }

    // 渐进移动:大角度移动时分步进行,降低瞬时电流
    uint8_t current = current_angles_[index];
    int diff = abs((int)angle - (int)current);

    // 如果角度变化超过20度,则分步移动
    if (diff > 20) {
        int steps = diff / 10;  // 每10度一步
        int direction = (angle > current) ? 10 : -10;

        for (int i = 0; i < steps; i++) {
            current += direction;

            uint32_t duty = AngleToDuty(current);
            uint8_t actual_angle = current;

            // 右侧舵机需要反向（因为镜像安装）
            if (index == SERVO_RIGHT_FRONT || index == SERVO_RIGHT_BACK) {
                actual_angle = 180 - current;
                duty = AngleToDuty(actual_angle);
            }

            ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_channels_[index], duty));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_channels_[index]));

            vTaskDelay(pdMS_TO_TICKS(15));  // 每步延迟15ms
        }

        // 设置最终角度
        current = angle;
    }

    uint32_t duty = AngleToDuty(angle);

    // 右侧舵机需要反向（因为镜像安装）
    if (index == SERVO_RIGHT_FRONT || index == SERVO_RIGHT_BACK) {
        angle = 180 - angle;
        duty = AngleToDuty(angle);
    }

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_channels_[index], duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_channels_[index]));

    current_angles_[index] = current;
    last_move_time_us_[index] = esp_timer_get_time();  // 记录本次移动时间
}

void PetServo::SetAllAngles(uint8_t angle1, uint8_t angle2, uint8_t angle3, uint8_t angle4) {
    // 分时启动舵机,避免瞬时电流过大导致MT3608升压模块电压跌落
    SetAngle(SERVO_LEFT_FRONT, angle1);
    vTaskDelay(pdMS_TO_TICKS(50));  // 延迟50ms

    SetAngle(SERVO_RIGHT_FRONT, angle2);
    vTaskDelay(pdMS_TO_TICKS(50));  // 延迟50ms

    SetAngle(SERVO_LEFT_BACK, angle3);
    vTaskDelay(pdMS_TO_TICKS(50));  // 延迟50ms

    SetAngle(SERVO_RIGHT_BACK, angle4);
}

uint8_t PetServo::GetAngle(ServoIndex index) {
    if (index >= SERVO_COUNT) return 0;
    return current_angles_[index];
}

void PetServo::SmoothMove(ServoIndex index, uint8_t target_angle, uint16_t duration_ms) {
    if (index >= SERVO_COUNT) return;

    uint8_t current = current_angles_[index];
    int diff = target_angle - current;

    if (diff == 0) return;

    int steps = abs(diff);
    int delay_per_step = duration_ms / steps;

    if (delay_per_step < 5) delay_per_step = 5;  // 最小延时5ms

    int direction = (diff > 0) ? 1 : -1;

    for (int i = 0; i < steps; i++) {
        current += direction;
        SetAngle(index, current);
        vTaskDelay(pdMS_TO_TICKS(delay_per_step));
    }
}
