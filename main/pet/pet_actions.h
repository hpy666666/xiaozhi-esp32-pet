#ifndef PET_ACTIONS_H
#define PET_ACTIONS_H

#include "pet_servo.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 动作ID枚举（基于STM32桌面宠物，调整为4腿版本）
enum PetActionId {
    ACTION_RELAXED_GETDOWN = 0,  // 放松趴下
    ACTION_SIT = 1,              // 坐下
    ACTION_UPRIGHT = 2,          // 站立
    ACTION_GETDOWN = 3,          // 趴下
    ACTION_ADVANCE = 4,          // 前进
    ACTION_BACK = 5,             // 后退
    ACTION_LEFT_ROTATION = 6,    // 左转
    ACTION_RIGHT_ROTATION = 7,   // 右转
    ACTION_SWING = 8,            // 摇摆
    ACTION_JUMP_FORWARD = 10,    // 前跳
    ACTION_JUMP_BACK = 11,       // 后跳
    ACTION_HELLO = 13,           // 打招呼（挥前腿）
    ACTION_STRETCH = 14,         // 伸懒腰
    ACTION_COUNT                 // 动作总数
};

// 动作参数配置
struct ActionParams {
    uint16_t speed_delay;      // 动作速度延迟（ms）
    uint16_t swing_delay;      // 摇摆延迟（ms）
    uint8_t repeat_count;      // 重复次数
    bool continuous;           // 是否持续执行
};

class PetActions {
public:
    PetActions(PetServo* servo);
    ~PetActions();

    // 执行指定动作
    void PerformAction(PetActionId action_id, const ActionParams* params = nullptr);

    // 停止当前动作
    void StopAction();

    // 设置默认参数
    void SetDefaultParams(const ActionParams& params);

    // 获取当前动作ID
    PetActionId GetCurrentAction() { return current_action_; }

    // 检查是否正在执行动作
    bool IsActionRunning() { return action_running_; }

private:
    // 各种动作的具体实现
    void ActionRelaxedGetdown();
    void ActionSit();
    void ActionUpright();
    void ActionGetdown();
    void ActionAdvance();
    void ActionBack();
    void ActionLeftRotation();
    void ActionRightRotation();
    void ActionSwing();
    void ActionJumpForward();
    void ActionJumpBack();
    void ActionHello();
    void ActionStretch();

    // 延时函数（支持中断）
    void Delay(uint32_t ms);

    // 检查是否需要停止
    bool ShouldStop();

    PetServo* servo_;
    ActionParams default_params_;
    ActionParams current_params_;
    PetActionId current_action_;
    bool action_running_;
    bool stop_requested_;
};

#endif // PET_ACTIONS_H
