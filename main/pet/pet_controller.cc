#include "pet_controller.h"
#include "mcp_server.h"
#include "esp_log.h"

static const char* TAG = "PetController";

PetController::PetController()
    : servo_(nullptr), actions_(nullptr), task_handle_(nullptr),
      command_queue_(nullptr), initialized_(false) {
}

PetController::~PetController() {
    if (actions_) delete actions_;
    if (servo_) delete servo_;
    if (command_queue_) vQueueDelete(command_queue_);
}

void PetController::Init() {
    if (initialized_) {
        ESP_LOGW(TAG, "Already initialized");
        return;
    }

    ESP_LOGI(TAG, "Initializing Pet Controller");

    // 创建舵机驱动
    servo_ = new PetServo();
    servo_->Init();

    // 创建动作控制
    actions_ = new PetActions(servo_);

    // 创建命令队列
    command_queue_ = xQueueCreate(10, sizeof(ActionCommand));
    if (!command_queue_) {
        ESP_LOGE(TAG, "Failed to create command queue");
        return;
    }

    // 创建FreeRTOS任务
    BaseType_t result = xTaskCreate(
        TaskEntry,
        "pet_task",
        4096,  // 栈大小
        this,
        5,     // 优先级（低于音频任务）
        &task_handle_
    );

    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create pet task");
        return;
    }

    initialized_ = true;
    ESP_LOGI(TAG, "Pet Controller initialized successfully");

    // 初始化到站立姿态
    PerformAction(ACTION_UPRIGHT);
}

void PetController::TaskEntry(void* param) {
    PetController* controller = static_cast<PetController*>(param);
    controller->TaskLoop();
}

void PetController::TaskLoop() {
    ActionCommand cmd;

    ESP_LOGI(TAG, "Pet control task started");

    while (true) {
        // 等待命令队列
        if (xQueueReceive(command_queue_, &cmd, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "Received action command: %d", cmd.action_id);
            actions_->PerformAction(cmd.action_id, &cmd.params);
        }
    }
}

void PetController::PerformAction(PetActionId action_id, const ActionParams* params) {
    if (!initialized_) {
        ESP_LOGE(TAG, "Not initialized");
        return;
    }

    // 停止当前动作
    if (!IsIdle()) {
        StopCurrentAction();
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // 准备命令
    ActionCommand cmd;
    cmd.action_id = action_id;

    if (params) {
        cmd.params = *params;
    } else {
        // 使用默认参数
        cmd.params.speed_delay = 80;
        cmd.params.swing_delay = 10;
        cmd.params.repeat_count = 1;
        cmd.params.continuous = false;
    }

    // 加入队列
    EnqueueAction(action_id, &cmd.params);
}

bool PetController::EnqueueAction(PetActionId action_id, const ActionParams* params) {
    if (!initialized_ || !command_queue_) {
        ESP_LOGE(TAG, "Not initialized or no queue");
        return false;
    }

    ActionCommand cmd;
    cmd.action_id = action_id;

    if (params) {
        cmd.params = *params;
    } else {
        cmd.params.speed_delay = 80;
        cmd.params.swing_delay = 10;
        cmd.params.repeat_count = 1;
        cmd.params.continuous = false;
    }

    BaseType_t result = xQueueSend(command_queue_, &cmd, pdMS_TO_TICKS(100));
    return result == pdTRUE;
}

void PetController::StopCurrentAction() {
    if (actions_) {
        actions_->StopAction();
    }
}

void PetController::SetSpeed(uint16_t speed_delay) {
    if (actions_) {
        ActionParams params;
        params.speed_delay = speed_delay;
        params.swing_delay = 10;
        params.repeat_count = 1;
        params.continuous = false;
        actions_->SetDefaultParams(params);
    }
}

bool PetController::IsIdle() {
    return actions_ ? !actions_->IsActionRunning() : true;
}

PetActionId PetController::GetCurrentAction() {
    return actions_ ? actions_->GetCurrentAction() : ACTION_UPRIGHT;
}

void PetController::RegisterMcpTools() {
    auto& mcp_server = McpServer::GetInstance();

    ESP_LOGI(TAG, "Registering MCP tools");

    // 工具1：执行动作
    mcp_server.AddTool(
        "self.pet.action",
        "控制桌面宠物执行动作。可用动作:\n"
        "0-放松趴下, 1-坐下, 2-站立, 3-趴下\n"
        "4-前进, 5-后退, 6-左转, 7-右转\n"
        "8-摇摆, 10-前跳, 11-后跳\n"
        "13-打招呼, 14-伸懒腰",
        PropertyList({
            Property("action_id", kPropertyTypeInteger, 2, 0, 14),
            Property("repeat", kPropertyTypeInteger, 1, 1, 10)
        }),
        [this](const PropertyList& props) -> ReturnValue {
            int action_id = props["action_id"].value<int>();
            int repeat = props["repeat"].value<int>();

            ActionParams params;
            params.speed_delay = 80;
            params.swing_delay = 10;
            params.repeat_count = repeat;
            params.continuous = false;

            PerformAction(static_cast<PetActionId>(action_id), &params);

            return "动作已执行";
        }
    );

    // 工具2：设置速度
    mcp_server.AddTool(
        "self.pet.set_speed",
        "设置桌面宠物动作速度（延迟ms，越小越快）",
        PropertyList({
            Property("speed", kPropertyTypeInteger, 80, 20, 200)
        }),
        [this](const PropertyList& props) -> ReturnValue {
            int speed = props["speed"].value<int>();
            SetSpeed(speed);
            return "速度已设置";
        }
    );

    // 工具3：停止动作
    mcp_server.AddTool(
        "self.pet.stop",
        "立即停止桌面宠物当前动作",
        PropertyList(),
        [this](const PropertyList& props) -> ReturnValue {
            StopCurrentAction();
            return "已停止";
        }
    );

    // 工具4：获取状态
    mcp_server.AddTool(
        "self.pet.get_status",
        "获取桌面宠物当前状态",
        PropertyList(),
        [this](const PropertyList& props) -> ReturnValue {
            if (IsIdle()) {
                return "空闲";
            } else {
                return "执行中: 动作" + std::to_string(GetCurrentAction());
            }
        }
    );

    ESP_LOGI(TAG, "MCP tools registered: self.pet.*");
}
