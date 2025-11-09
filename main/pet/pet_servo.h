#ifndef PET_SERVO_H
#define PET_SERVO_H

#include "driver/ledc.h"
#include "esp_log.h"
#include <cstdint>

// SG90舵机参数
#define SERVO_MIN_PULSEWIDTH_US 500   // 最小脉宽（微秒）对应0度
#define SERVO_MAX_PULSEWIDTH_US 2500  // 最大脉宽（微秒）对应180度
#define SERVO_MIN_DEGREE 0            // 最小角度
#define SERVO_MAX_DEGREE 180          // 最大角度
#define SERVO_TIMEBASE_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD 20000   // 20000 ticks, 20ms (50Hz)

// 舵机索引定义（对应4条腿）
enum ServoIndex {
    SERVO_LEFT_FRONT = 0,   // 左前腿 - GPIO9
    SERVO_RIGHT_FRONT = 1,  // 右前腿 - GPIO10
    SERVO_LEFT_BACK = 2,    // 左后腿 - GPIO11
    SERVO_RIGHT_BACK = 3,   // 右后腿 - GPIO12
    SERVO_COUNT = 4
};

class PetServo {
public:
    PetServo();
    ~PetServo();

    // 初始化所有舵机
    void Init();

    // 设置单个舵机角度 (0-180度)
    void SetAngle(ServoIndex index, uint8_t angle);

    // 批量设置所有舵机角度
    void SetAllAngles(uint8_t angle1, uint8_t angle2, uint8_t angle3, uint8_t angle4);

    // 获取当前角度
    uint8_t GetAngle(ServoIndex index);

    // 平滑移动到目标角度（带延时）
    void SmoothMove(ServoIndex index, uint8_t target_angle, uint16_t duration_ms);

private:
    // 角度转换为PWM占空比
    uint32_t AngleToDuty(uint8_t angle);

    // 角度转换为脉宽（微秒）
    uint32_t AngleToPulseWidth(uint8_t angle);

    // 舵机GPIO引脚
    static const int servo_pins_[SERVO_COUNT];

    // LEDC通道
    ledc_channel_t ledc_channels_[SERVO_COUNT];

    // 当前角度缓存
    uint8_t current_angles_[SERVO_COUNT];

    // 定时器配置标志
    bool timer_initialized_;

    // 上次移动时间记录(用于自动节流,避免电流冲击)
    int64_t last_move_time_us_[SERVO_COUNT];
};

#endif // PET_SERVO_H
