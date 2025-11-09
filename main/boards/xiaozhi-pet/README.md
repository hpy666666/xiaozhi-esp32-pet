# Xiaozhi Pet Board - 桌面宠物板

## 硬件配置

### 开发板
- ESP32-S3N16R8 最小系统板

### 引脚连接

#### 音频系统
- **MAX98357A (音频输出)**
  - BCLK: GPIO15
  - LRC: GPIO16
  - DIN: GPIO17

- **INMP441 (麦克风输入)**
  - WS: GPIO4
  - SCK: GPIO5
  - SD: GPIO6

#### 显示屏
- **OLED 128x32 (I2C)**
  - SDA: GPIO41
  - SCL: GPIO42

#### 舵机系统（4个SG90舵机）
- 舵机1 (左前腿): GPIO9
- 舵机2 (右前腿): GPIO10
- 舵机3 (左后腿): GPIO11
- 舵机4 (右后腿): GPIO12

#### 按钮
- BOOT按钮: GPIO0

### 供电
- 5V供电（主板和舵机共用）

## 编译说明

### 1. 环境准备
```bash
# 确保已安装 ESP-IDF v5.1+
cd xiaozhi-esp32-1.9.2_2
```

### 2. 选择板子配置
```bash
# 方式1：使用环境变量
export XIAOZHI_BOARD=xiaozhi-pet

# 方式2：或在 menuconfig 中选择
idf.py menuconfig
# 导航到 Xiaozhi Configuration -> Board Selection
# 选择 "xiaozhi-pet"
```

### 3. 编译烧录
```bash
# 编译
idf.py build

# 烧录
idf.py -p /dev/ttyUSB0 flash

# 监控输出
idf.py monitor
```

### Windows用户
```cmd
set XIAOZHI_BOARD=xiaozhi-pet
idf.py build
idf.py -p COM3 flash monitor
```

## 功能说明

### 内置动作列表

| 动作ID | 动作名称 | 描述 |
|--------|---------|------|
| 0 | 放松趴下 | 前腿低，后腿高 |
| 1 | 坐下 | 前腿站立，后腿趴下 |
| 2 | 站立 | 所有腿90度中位 |
| 3 | 趴下 | 所有腿都降低 |
| 4 | 前进 | 对角线步态行走 |
| 5 | 后退 | 对角线步态倒退 |
| 6 | 左转 | 原地左转 |
| 7 | 右转 | 原地右转 |
| 8 | 摇摆 | 所有腿同步摆动 |
| 10 | 前跳 | 后腿发力前跳 |
| 11 | 后跳 | 前腿发力后跳 |
| 13 | 打招呼 | 坐下后挥前腿 |
| 14 | 伸懒腰 | 拉伸前后腿 |

### MCP协议控制

代码中已集成4个MCP工具，可通过AI语音或客户端调用：

#### 1. 执行动作
```json
{
  "tool": "self.pet.action",
  "arguments": {
    "action_id": 13,    // 动作ID (0-14)
    "repeat": 3         // 重复次数 (1-10)
  }
}
```

#### 2. 设置速度
```json
{
  "tool": "self.pet.set_speed",
  "arguments": {
    "speed": 50    // 延迟ms (20-200，越小越快)
  }
}
```

#### 3. 停止动作
```json
{
  "tool": "self.pet.stop",
  "arguments": {}
}
```

#### 4. 获取状态
```json
{
  "tool": "self.pet.get_status",
  "arguments": {}
}
```

### 语音控制示例

配置好云端后，可以通过语音控制：

- "小智，站起来" → 执行站立动作
- "小智，打个招呼" → 执行挥手动作
- "小智，走两步" → 执行前进动作
- "小智，坐下" → 执行坐下动作

## 硬件调试

### 舵机测试

启动后，设备会自动初始化舵机到90度（站立）位置。如果舵机没有响应：

1. 检查舵机供电（5V和GND）
2. 检查信号线是否连接到GPIO9-12
3. 查看串口日志：
```
I (1234) PetServo: Initializing pet servos on GPIO9-12
I (1235) PetServo: LEDC timer configured: 50Hz, 13-bit resolution
I (1236) PetServo: Servo 0 initialized on GPIO9 (channel 0)
...
I (1240) PetServo: All servos initialized to 90 degrees
```

### 按钮功能

- **单击BOOT按钮**：切换聊天状态
- **长按BOOT按钮**：执行"打招呼"动作（测试用）

## 代码结构

```
main/
├── pet/
│   ├── pet_servo.h/cc          # 舵机驱动（LEDC PWM）
│   ├── pet_actions.h/cc        # 动作库（13种动作）
│   └── pet_controller.h/cc     # 控制器（状态机+MCP）
└── boards/
    └── xiaozhi-pet/
        ├── config.h            # 硬件配置
        ├── xiaozhi_pet_board.cc # Board类实现
        ├── CMakeLists.txt      # 构建文件
        └── sdkconfig.xiaozhi-pet # SDK配置
```

## 常见问题

### 1. 舵机抖动
- 检查供电是否稳定（建议使用独立5V电源）
- 降低动作速度：`self.pet.set_speed(100)`

### 2. 编译错误
```
fatal error: pet/pet_controller.h: No such file or directory
```
- 确认 `XIAOZHI_BOARD=xiaozhi-pet` 环境变量已设置
- 检查 CMakeLists.txt 中的路径

### 3. 舵机角度不对
- 右侧舵机（2和4）已自动反向，如果仍不对称，调整 `pet_servo.cc:96` 的反向逻辑

## 扩展开发

### 添加新动作

编辑 `main/pet/pet_actions.cc`：

```cpp
void PetActions::ActionCustom() {
    servo_->SetAngle(SERVO_LEFT_FRONT, 45);
    Delay(100);
    servo_->SetAngle(SERVO_RIGHT_FRONT, 135);
    Delay(100);
    // ... 你的动作逻辑
}
```

在 `PerformAction()` 中添加case：
```cpp
case ACTION_CUSTOM:
    ActionCustom();
    break;
```

### 修改引脚

编辑 `main/boards/xiaozhi-pet/config.h`，修改引脚定义后重新编译。

## 贡献

欢迎提交Issue和PR！

## 许可

MIT License
