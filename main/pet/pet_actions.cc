#include "pet_actions.h"
#include "esp_log.h"

static const char* TAG = "PetActions";

PetActions::PetActions(PetServo* servo)
    : servo_(servo), current_action_(ACTION_UPRIGHT),
      action_running_(false), stop_requested_(false) {

    // 设置默认参数
    default_params_.speed_delay = 80;
    default_params_.swing_delay = 10;
    default_params_.repeat_count = 1;
    default_params_.continuous = false;

    ESP_LOGI(TAG, "PetActions initialized");
}

PetActions::~PetActions() {
}

void PetActions::SetDefaultParams(const ActionParams& params) {
    default_params_ = params;
}

void PetActions::Delay(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

bool PetActions::ShouldStop() {
    return stop_requested_;
}

void PetActions::StopAction() {
    stop_requested_ = true;
    ESP_LOGI(TAG, "Action stop requested");
}

void PetActions::PerformAction(PetActionId action_id, const ActionParams* params) {
    if (action_running_) {
        ESP_LOGW(TAG, "Action already running, stopping previous action");
        StopAction();
        Delay(100);
    }

    action_running_ = true;
    stop_requested_ = false;
    current_action_ = action_id;

    // 使用提供的参数或默认参数
    current_params_ = params ? *params : default_params_;

    ESP_LOGI(TAG, "Performing action: %d", action_id);

    switch (action_id) {
        case ACTION_RELAXED_GETDOWN:
            ActionRelaxedGetdown();
            break;
        case ACTION_SIT:
            ActionSit();
            break;
        case ACTION_UPRIGHT:
            ActionUpright();
            break;
        case ACTION_GETDOWN:
            ActionGetdown();
            break;
        case ACTION_ADVANCE:
            ActionAdvance();
            break;
        case ACTION_BACK:
            ActionBack();
            break;
        case ACTION_LEFT_ROTATION:
            ActionLeftRotation();
            break;
        case ACTION_RIGHT_ROTATION:
            ActionRightRotation();
            break;
        case ACTION_SWING:
            ActionSwing();
            break;
        case ACTION_JUMP_FORWARD:
            ActionJumpForward();
            break;
        case ACTION_JUMP_BACK:
            ActionJumpBack();
            break;
        case ACTION_HELLO:
            ActionHello();
            break;
        case ACTION_STRETCH:
            ActionStretch();
            break;
        default:
            ESP_LOGE(TAG, "Unknown action ID: %d", action_id);
            break;
    }

    action_running_ = false;
    ESP_LOGI(TAG, "Action completed: %d", action_id);
}

// ========== 动作实现 ==========

void PetActions::ActionRelaxedGetdown() {
    // 放松趴下：前腿低，后腿高
    servo_->SetAngle(SERVO_LEFT_FRONT, 20);
    servo_->SetAngle(SERVO_RIGHT_FRONT, 20);
    Delay(current_params_.speed_delay);
    if (ShouldStop()) return;

    servo_->SetAngle(SERVO_LEFT_BACK, 160);
    servo_->SetAngle(SERVO_RIGHT_BACK, 160);
}

void PetActions::ActionUpright() {
    // 站立：所有腿90度
    servo_->SetAngle(SERVO_LEFT_FRONT, 90);
    servo_->SetAngle(SERVO_RIGHT_FRONT, 90);
    Delay(current_params_.speed_delay);
    if (ShouldStop()) return;

    servo_->SetAngle(SERVO_LEFT_BACK, 90);
    servo_->SetAngle(SERVO_RIGHT_BACK, 90);
}

void PetActions::ActionGetdown() {
    // 趴下：所有腿都低
    servo_->SetAngle(SERVO_LEFT_FRONT, 20);
    servo_->SetAngle(SERVO_RIGHT_FRONT, 20);
    Delay(current_params_.speed_delay);
    if (ShouldStop()) return;

    servo_->SetAngle(SERVO_LEFT_BACK, 20);
    servo_->SetAngle(SERVO_RIGHT_BACK, 20);
}

void PetActions::ActionSit() {
    // 坐下：前腿站立，后腿趴下
    servo_->SetAngle(SERVO_LEFT_FRONT, 90);
    servo_->SetAngle(SERVO_RIGHT_FRONT, 90);
    Delay(current_params_.speed_delay);
    if (ShouldStop()) return;

    servo_->SetAngle(SERVO_LEFT_BACK, 20);
    servo_->SetAngle(SERVO_RIGHT_BACK, 20);
}

void PetActions::ActionAdvance() {
    // 前进：对角线步态
    uint8_t repeat = current_params_.repeat_count;

    while (repeat > 0 && !ShouldStop()) {
        // 右前+左后 向前
        servo_->SetAngle(SERVO_RIGHT_FRONT, 45);
        servo_->SetAngle(SERVO_LEFT_BACK, 45);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        // 左前+右后 向前
        servo_->SetAngle(SERVO_LEFT_FRONT, 135);
        servo_->SetAngle(SERVO_RIGHT_BACK, 135);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        // 回中位
        servo_->SetAngle(SERVO_RIGHT_FRONT, 90);
        servo_->SetAngle(SERVO_LEFT_BACK, 90);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        servo_->SetAngle(SERVO_LEFT_FRONT, 90);
        servo_->SetAngle(SERVO_RIGHT_BACK, 90);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        // 另一组对角线
        servo_->SetAngle(SERVO_LEFT_FRONT, 45);
        servo_->SetAngle(SERVO_RIGHT_BACK, 45);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        servo_->SetAngle(SERVO_RIGHT_FRONT, 135);
        servo_->SetAngle(SERVO_LEFT_BACK, 135);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        // 回中位
        servo_->SetAngle(SERVO_LEFT_FRONT, 90);
        servo_->SetAngle(SERVO_RIGHT_BACK, 90);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        servo_->SetAngle(SERVO_RIGHT_FRONT, 90);
        servo_->SetAngle(SERVO_LEFT_BACK, 90);
        Delay(current_params_.speed_delay);

        if (!current_params_.continuous) repeat--;
    }
}

void PetActions::ActionBack() {
    // 后退：与前进相反
    uint8_t repeat = current_params_.repeat_count;

    while (repeat > 0 && !ShouldStop()) {
        servo_->SetAngle(SERVO_RIGHT_FRONT, 135);
        servo_->SetAngle(SERVO_LEFT_BACK, 135);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        servo_->SetAngle(SERVO_LEFT_FRONT, 45);
        servo_->SetAngle(SERVO_RIGHT_BACK, 45);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        servo_->SetAngle(SERVO_RIGHT_FRONT, 90);
        servo_->SetAngle(SERVO_LEFT_BACK, 90);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        servo_->SetAngle(SERVO_LEFT_FRONT, 90);
        servo_->SetAngle(SERVO_RIGHT_BACK, 90);
        Delay(current_params_.speed_delay);

        if (!current_params_.continuous) repeat--;
    }
}

void PetActions::ActionLeftRotation() {
    // 左转：左侧腿后退，右侧腿前进
    uint8_t repeat = current_params_.repeat_count;

    while (repeat > 0 && !ShouldStop()) {
        servo_->SetAngle(SERVO_RIGHT_FRONT, 45);
        servo_->SetAngle(SERVO_LEFT_BACK, 135);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        servo_->SetAngle(SERVO_LEFT_FRONT, 45);
        servo_->SetAngle(SERVO_RIGHT_BACK, 135);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        servo_->SetAllAngles(90, 90, 90, 90);
        Delay(current_params_.speed_delay);

        if (!current_params_.continuous) repeat--;
    }
}

void PetActions::ActionRightRotation() {
    // 右转：右侧腿后退，左侧腿前进
    uint8_t repeat = current_params_.repeat_count;

    while (repeat > 0 && !ShouldStop()) {
        servo_->SetAngle(SERVO_LEFT_FRONT, 45);
        servo_->SetAngle(SERVO_RIGHT_BACK, 135);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        servo_->SetAngle(SERVO_RIGHT_FRONT, 45);
        servo_->SetAngle(SERVO_LEFT_BACK, 135);
        Delay(current_params_.speed_delay);
        if (ShouldStop()) break;

        servo_->SetAllAngles(90, 90, 90, 90);
        Delay(current_params_.speed_delay);

        if (!current_params_.continuous) repeat--;
    }
}

void PetActions::ActionSwing() {
    // 摇摆：所有腿同时摆动
    uint8_t repeat = current_params_.repeat_count;

    while (repeat > 0 && !ShouldStop()) {
        for (int angle = 30; angle < 150 && !ShouldStop(); angle += 5) {
            servo_->SetAllAngles(angle, angle, angle, angle);
            Delay(current_params_.swing_delay);
        }

        for (int angle = 150; angle > 30 && !ShouldStop(); angle -= 5) {
            servo_->SetAllAngles(angle, angle, angle, angle);
            Delay(current_params_.swing_delay);
        }

        repeat--;
    }

    // 回到站立位置
    servo_->SetAllAngles(90, 90, 90, 90);
}

void PetActions::ActionJumpForward() {
    // 前跳：后腿用力，前腿抬起
    servo_->SetAngle(SERVO_LEFT_FRONT, 140);
    servo_->SetAngle(SERVO_RIGHT_FRONT, 140);
    Delay(current_params_.speed_delay);
    if (ShouldStop()) return;

    servo_->SetAngle(SERVO_LEFT_BACK, 35);
    servo_->SetAngle(SERVO_RIGHT_BACK, 35);
    Delay(current_params_.speed_delay + 80);
    if (ShouldStop()) return;

    // 回到站立
    ActionUpright();
}

void PetActions::ActionJumpBack() {
    // 后跳：前腿用力，后腿抬起
    servo_->SetAngle(SERVO_LEFT_BACK, 140);
    servo_->SetAngle(SERVO_RIGHT_BACK, 140);
    Delay(current_params_.speed_delay);
    if (ShouldStop()) return;

    servo_->SetAngle(SERVO_LEFT_FRONT, 35);
    servo_->SetAngle(SERVO_RIGHT_FRONT, 35);
    Delay(current_params_.speed_delay + 80);
    if (ShouldStop()) return;

    // 回到站立
    ActionUpright();
}

void PetActions::ActionHello() {
    // 打招呼：坐下后挥前腿
    ActionSit();
    Delay(100);
    if (ShouldStop()) return;

    uint8_t repeat = current_params_.repeat_count;
    while (repeat > 0 && !ShouldStop()) {
        for (int angle = 90; angle >= 45 && !ShouldStop(); angle -= 5) {
            servo_->SetAngle(SERVO_LEFT_FRONT, angle);
            Delay(current_params_.swing_delay);
        }

        for (int angle = 45; angle <= 90 && !ShouldStop(); angle += 5) {
            servo_->SetAngle(SERVO_LEFT_FRONT, angle);
            Delay(current_params_.swing_delay);
        }

        repeat--;
    }

    ActionUpright();
}

void PetActions::ActionStretch() {
    // 伸懒腰：先后腿站立，前腿下压，再相反
    servo_->SetAngle(SERVO_LEFT_BACK, 90);
    servo_->SetAngle(SERVO_RIGHT_BACK, 90);
    Delay(current_params_.speed_delay);
    if (ShouldStop()) return;

    // 前腿下压
    for (int angle = 90; angle > 10 && !ShouldStop(); angle -= 5) {
        servo_->SetAngle(SERVO_LEFT_FRONT, angle);
        servo_->SetAngle(SERVO_RIGHT_FRONT, angle);
        Delay(15);
    }

    // 前腿回升
    for (int angle = 10; angle < 90 && !ShouldStop(); angle += 5) {
        servo_->SetAngle(SERVO_LEFT_FRONT, angle);
        servo_->SetAngle(SERVO_RIGHT_FRONT, angle);
        Delay(15);
    }

    // 后腿上抬
    for (int angle = 90; angle < 170 && !ShouldStop(); angle += 5) {
        servo_->SetAngle(SERVO_LEFT_BACK, angle);
        servo_->SetAngle(SERVO_RIGHT_BACK, angle);
        Delay(15);
    }

    // 后腿回落
    for (int angle = 170; angle > 90 && !ShouldStop(); angle -= 5) {
        servo_->SetAngle(SERVO_LEFT_BACK, angle);
        servo_->SetAngle(SERVO_RIGHT_BACK, angle);
        Delay(15);
    }

    ActionUpright();
}
