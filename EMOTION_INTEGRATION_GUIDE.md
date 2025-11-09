# 表情联动集成指南

## 📊 概述

表情联动系统可以根据设备状态自动显示对应的表情，提升用户交互体验。

## 🎯 使用方式

### 方式1：状态自动联动 (推荐)

在应用程序中切换设备状态时，自动切换表情：

```cpp
// 在 application.cc 中
void Application::SetDeviceState(DeviceState new_state) {
    device_state_ = new_state;

    // 自动更新表情显示
    auto display = GetDisplay();
    if (display != nullptr) {
        OledDisplay* oled = dynamic_cast<OledDisplay*>(display);
        if (oled != nullptr) {
            oled->SetEmotionForState(new_state);
        }
    }

    // ... 其他状态处理逻辑
}
```

### 方式2：手动指定表情

在特定场景下手动设置表情：

```cpp
auto display = GetDisplay();
if (display != nullptr) {
    OledDisplay* oled = static_cast<OledDisplay*>(display);

    // 显示特定表情
    oled->SetEmotion(EMOJI_HAPPY);

    // 3秒后清除，恢复正常UI
    vTaskDelay(pdMS_TO_TICKS(3000));
    oled->ClearEmotion();
}
```

## 📋 状态到表情的映射表

| 设备状态 | 表情 | 说明 |
|---------|------|------|
| `kDeviceStateIdle` | EMOJI_HAPPY | 空闲待机 - 开心表情 |
| `kDeviceStateListening` | EMOJI_EXCITED | 录音中 - 兴奋/专注 |
| `kDeviceStateSpeaking` | EMOJI_SPEAKING | 说话中 - 张嘴表情 |
| `kDeviceStateConnecting` | EMOJI_THINKING | 连接中 - 思考表情 |
| `kDeviceStateActivating` | EMOJI_THINKING | 激活中 - 思考表情 |
| `kDeviceStateStarting` | EMOJI_SQUINT | 启动中 - 眯眼/醒来 |
| `kDeviceStateWifiConfiguring` | EMOJI_SONFUSED | 配置中 - 困惑表情 |
| `kDeviceStateUpgrading` | EMOJI_THINKING | 升级中 - 思考表情 |
| `kDeviceStateFatalError` | EMOJI_SAD | 错误 - 难过表情 |
| `kDeviceStateAudioTesting` | EMOJI_LOVE | 测试中 - 活泼表情 |

## 🎨 自定义映射关系

修改 `main/display/emotion_manager.c` 中的映射逻辑：

```c
Emotion get_emotion_for_state(DeviceState state) {
    switch (state) {
        case kDeviceStateIdle:
            return EMOJI_HAPPY;  // 修改为你想要的表情

        // ... 其他状态

        default:
            return EMOJI_HAPPY;
    }
}
```

## 🔗 集成到现有代码

### 步骤1：在Application类中添加表情更新

```cpp
// application.h
class Application {
private:
    void UpdateEmotionForState();  // 新增方法
};

// application.cc
void Application::UpdateEmotionForState() {
    auto display = GetDisplay();
    if (display == nullptr) return;

    OledDisplay* oled = dynamic_cast<OledDisplay*>(display);
    if (oled == nullptr) return;

    // 只在表情模式未激活时自动更新
    if (!oled->IsEmotionModeActive()) {
        oled->SetEmotionForState(device_state_);
    }
}
```

### 步骤2：在状态变化时调用

```cpp
void Application::SetDeviceState(DeviceState new_state) {
    if (device_state_ == new_state) {
        return;
    }

    device_state_ = new_state;
    ESP_LOGI(TAG, "STATE: %s", GetDeviceStateName(new_state));

    // 更新表情显示
    UpdateEmotionForState();

    // 发布状态变化事件
    DeviceStateEvent event(device_state_);
    event_bus_.Publish(event);
}
```

### 步骤3：音频事件联动

```cpp
// 在 audio_service.cc 中
void AudioService::OnRecordingStart() {
    // 录音开始时显示倾听表情
    auto display = Application::GetInstance().GetDisplay();
    if (display) {
        OledDisplay* oled = dynamic_cast<OledDisplay*>(display);
        if (oled) {
            oled->SetEmotion(EMOJI_EXCITED);
        }
    }
}

void AudioService::OnPlaybackStart() {
    // 播放开始时显示说话表情
    auto display = Application::GetInstance().GetDisplay();
    if (display) {
        OledDisplay* oled = dynamic_cast<OledDisplay*>(display);
        if (oled) {
            oled->SetEmotion(EMOJI_SPEAKING);
        }
    }
}

void AudioService::OnPlaybackComplete() {
    // 播放完成后恢复正常UI
    auto display = Application::GetInstance().GetDisplay();
    if (display) {
        OledDisplay* oled = dynamic_cast<OledDisplay*>(display);
        if (oled) {
            oled->ClearEmotion();
        }
    }
}
```

### 步骤4：宠物动作联动 (可选)

```cpp
// 在 pet_controller.cc 中
void PetController::PerformAction(int action_id) {
    auto display = Application::GetInstance().GetDisplay();
    if (display) {
        OledDisplay* oled = dynamic_cast<OledDisplay*>(display);
        if (oled) {
            // 动作开始前显示对应表情
            Emotion emotion = get_emotion_for_pet_action(action_id);
            oled->SetEmotion(emotion);
        }
    }

    // 执行舵机动作
    PetActions::PerformAction(action_id);

    // 动作完成后可选择：
    // 选项1：保持表情一段时间再清除
    // 选项2：立即恢复正常UI
}
```

## ⚙️ 高级功能

### 表情队列系统 (未来扩展)

如果需要播放表情动画序列：

```cpp
class EmotionQueue {
public:
    void Add(Emotion emotion, uint32_t duration_ms);
    void Play();
    void Stop();
private:
    std::queue<std::pair<Emotion, uint32_t>> queue_;
    TimerHandle_t timer_;
};
```

### 眨眼动画 (未来扩展)

实现idle状态下的眨眼动画：

```cpp
// 需要准备5帧眨眼动画位图
Emotion blink_sequence[] = {
    EMOJI_IDLE_OPEN,    // 睁眼
    EMOJI_IDLE_HALF,    // 半闭
    EMOJI_IDLE_CLOSED,  // 闭眼
    EMOJI_IDLE_HALF,    // 半开
    EMOJI_IDLE_OPEN     // 睁眼
};

void PlayBlinkAnimation() {
    for (int i = 0; i < 5; i++) {
        oled->SetEmotion(blink_sequence[i]);
        vTaskDelay(pdMS_TO_TICKS(100));  // 每帧100ms
    }
}
```

## 📝 注意事项

1. **线程安全**: 所有LVGL相关操作都需要使用DisplayLockGuard
2. **性能考虑**: 表情切换会清空LVGL UI，频繁切换可能影响性能
3. **状态优先级**: 手动设置的表情不会被自动状态联动覆盖
4. **测试建议**: 在实际部署前充分测试各个状态的表情显示

## 🐛 调试技巧

### 启用详细日志

```cpp
// 在 emotion_manager.c 中
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
```

### 查看当前表情状态

```cpp
bool is_active = oled->IsEmotionModeActive();
ESP_LOGI(TAG, "Emotion mode: %s", is_active ? "ACTIVE" : "INACTIVE");
```

### 串口监视关键日志

```
I (xxx) OledDisplay: Setting emotion: emoji_happy (128x64)
I (xxx) OledDisplay: Emotion displayed successfully
I (xxx) EmotionManager: State kDeviceStateIdle -> EMOJI_HAPPY
```

## 📚 相关文件

- `main/display/emotion_manager.h/c` - 状态到表情的映射逻辑
- `main/display/oled_display.h/cc` - 表情显示实现
- `main/display/emotion_bitmaps.h/c` - 表情位图数据
- `main/device_state.h` - 设备状态枚举定义

---

**最后更新**: 2025-11-08
**适用版本**: xiaozhi-esp32 v1.9.2+
