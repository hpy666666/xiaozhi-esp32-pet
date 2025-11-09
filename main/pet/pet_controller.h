#ifndef PET_CONTROLLER_H
#define PET_CONTROLLER_H

#include "pet_servo.h"
#include "pet_actions.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// 动作命令结构
struct ActionCommand {
    PetActionId action_id;
    ActionParams params;
};

class PetController {
public:
    static PetController& GetInstance() {
        static PetController instance;
        return instance;
    }

    // 初始化宠物控制器
    void Init();

    // 执行动作（立即执行）
    void PerformAction(PetActionId action_id, const ActionParams* params = nullptr);

    // 加入动作队列（异步执行）
    bool EnqueueAction(PetActionId action_id, const ActionParams* params = nullptr);

    // 停止当前动作
    void StopCurrentAction();

    // 设置速度（全局）
    void SetSpeed(uint16_t speed_delay);

    // 获取当前状态
    bool IsIdle();
    PetActionId GetCurrentAction();

    // 注册MCP工具
    void RegisterMcpTools();

private:
    PetController();
    ~PetController();

    // FreeRTOS任务入口
    static void TaskEntry(void* param);
    void TaskLoop();

    PetServo* servo_;
    PetActions* actions_;
    TaskHandle_t task_handle_;
    QueueHandle_t command_queue_;

    bool initialized_;
};

#endif // PET_CONTROLLER_H
