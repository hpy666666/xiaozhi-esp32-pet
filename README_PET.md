# Xiaozhi ESP32 Pet - 桌面宠物机器人

基于 [xiaozhi-esp32](https://github.com/78/xiaozhi-esp32) 的桌面宠物机器人改进版本，增加了舵机控制和动态表情系统。

![项目封面](docs/xiaozhi-pet-cover.jpg)

## ✨ 新增特性

相比原版xiaozhi-esp32，本项目新增了以下功能：

### 🤖 舵机控制系统
- **4个舵机驱动**: 使用ESP32的LEDC PWM控制4个SG90舵机
- **13种预定义动作**: 站立、趴下、坐下、前进、后退、左转、右转、挥手、摇头、点头等
- **MCP协议集成**: 通过语音指令控制宠物动作
- **分时启动优化**: 解决多舵机同时启动的电流冲击问题

### 😊 动态表情系统
- **慵懒眨眼动画**: 基于LVGL v9的GIF动画，支持自动循环播放
- **多种眨眼曲线**:
  - lazy曲线：打瞌睡效果（分段式，有故事感）
  - gaussian曲线：高斯分布（最真实，符合人眼运动）
  - sine曲线：正弦曲线（最平滑，经典动画）
  - smootherstep曲线：游戏引擎标准
- **18种静态位图表情**: 配合设备状态和宠物动作切换
- **LVGL Canvas显示**: 表情与UI共存，解决了反色问题

### ⚡ 电源管理优化
- **MT3608升压方案**: 3.7V锂电池升压至5V供电
- **舵机分时启动**: 间隔50ms依次启动，避免电压跌落
- **电流优化策略**:
  - 启动瞬间: 1个舵机800-1000mA
  - 运动中: 200-400mA
  - 保持位置: 50-150mA
- **Brownout保护**: 防止因电流过大导致系统重启

## 📸 演示

### 硬件组成
- ESP32-S3-N16R8 主控芯片
- SH1106 OLED显示屏 (128×64)
- INMP441 麦克风
- MAX98357 音频功放
- 4个SG90舵机
- MT3608升压模块
- 3.7V锂电池

### 动作演示
- 待机: 播放慵懒眨眼动画
- 聆听: 耳朵竖起（舵机动作） + 开心表情
- 说话: 嘴巴开合表情
- 挥手: 前腿摆动 + 兴奋表情
- 睡觉: 趴下 + 眯眼表情

## 🛠️ 硬件要求

### 必需硬件
- ESP32-S3开发板 (推荐N16R8版本)
- SH1106 OLED显示屏 (I2C接口)
- INMP441 MEMS麦克风
- MAX98357A I2S音频功放
- 4个SG90微型舵机
- MT3608升压模块（DC-DC 3.7V→5V）
- 18650锂电池 (3.7V)
- TP4056充电模块

### 引脚定义

详见 `main/boards/xiaozhi-pet/config.h`

```c
// 音频输出
AUDIO_OUTPUT_BCLK_PIN = 15
AUDIO_OUTPUT_WS_PIN = 16
AUDIO_OUTPUT_DOUT_PIN = 7

// 麦克风输入
AUDIO_INPUT_BCLK_PIN = 5
AUDIO_INPUT_WS_PIN = 4
AUDIO_INPUT_DIN_PIN = 6

// OLED显示屏 (I2C)
OLED_SDA_PIN = 41
OLED_SCL_PIN = 42

// 舵机控制 (PWM)
SERVO_PIN_1 = 9   // 左前腿
SERVO_PIN_2 = 10  // 右前腿
SERVO_PIN_3 = 11  // 左后腿
SERVO_PIN_4 = 12  // 右后腿
```

### PCB设计
PCB设计文件位于 `hardware/` 目录（如有）

电源设计要点：
- 每个舵机旁添加220μF电解电容
- OLED I2C需要4.7kΩ上拉电阻
- 麦克风和扬声器保持5cm以上距离
- 舵机信号线建议添加10kΩ上拉电阻

## 🚀 快速开始

### 1. 安装开发环境

**Windows**:
```bash
# 安装ESP-IDF v5.4+
# 参考: https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/
```

**Linux/macOS**:
```bash
# 安装ESP-IDF
git clone -b v5.4 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32s3
. ./export.sh
```

### 2. 克隆项目

```bash
git clone https://github.com/你的用户名/xiaozhi-esp32-pet.git
cd xiaozhi-esp32-pet
```

### 3. 编译和烧录

```bash
# 激活ESP-IDF环境（如果还没激活）
. ~/esp/esp-idf/export.sh  # Linux/macOS
# 或
%USERPROFILE%\esp\esp-idf\export.bat  # Windows

# 编译xiaozhi-pet板型
cd xiaozhi-esp32-1.9.2_2
python scripts/release.py xiaozhi-pet

# 烧录到设备
idf.py -p COM5 flash monitor  # 替换COM5为你的端口号
```

### 4. 配置网络和服务器

首次启动后，设备会创建Wi-Fi热点用于配置：
- SSID: `Xiaozhi_XXXXXX`
- 密码: `12345678`

连接热点后访问 `http://192.168.4.1` 进行配置。

## 📖 项目结构

```
xiaozhi-esp32-1.9.2_2/
├── main/
│   ├── boards/
│   │   └── xiaozhi-pet/          # 宠物版硬件配置
│   │       ├── config.h           # 引脚定义
│   │       ├── config.json        # 板型配置
│   │       └── xiaozhi_pet_board.cc
│   ├── pet/                       # 舵机控制模块
│   │   ├── pet_servo.h/cc         # 舵机驱动
│   │   ├── pet_actions.h/cc       # 动作库
│   │   └── pet_controller.h/cc    # MCP集成
│   ├── display/                   # 显示模块
│   │   ├── oled_display.h/cc      # OLED驱动
│   │   ├── emotion_bitmaps.h/c    # 静态表情
│   │   ├── emotion_manager.h/c    # 表情映射
│   │   └── lazy_blink_gif.c       # 动态GIF动画
│   └── ...
├── scripts/
│   └── release.py                 # 编译脚本
├── docs/                          # 文档
└── README.md
```

## 🎨 动画制作工具

项目包含了动画生成工具 `generate_lazy_blink.py`，可以生成不同风格的眨眼动画：

```bash
# 生成打瞌睡曲线动画
python generate_lazy_blink.py lazy

# 生成高斯曲线动画（最真实）
python generate_lazy_blink.py gaussian

# 生成正弦曲线动画（最平滑）
python generate_lazy_blink.py sine

# 生成游戏引擎标准曲线
python generate_lazy_blink.py smootherstep
```

生成的MP4文件需要经过以下步骤转换：
1. 使用 [ezgif.com](https://ezgif.com/video-to-gif) 转换为GIF
2. 使用 [LVGL Image Converter](https://lvgl.io/tools/imageconverter) 转换为C数组
3. 替换 `main/display/lazy_blink_gif.c`

## 🔧 常见问题

### Q: 连接两个以上舵机就自动重启？
A: 这是电源问题。解决方案：
1. 确保使用了分时启动代码（已在本项目中实现）
2. 检查MT3608升压模块输出能力（建议5V 3A以上）
3. 每个舵机旁添加220μF电解电容

### Q: OLED显示屏不显示？
A: 检查I2C连接和上拉电阻：
- SDA/SCL是否接对
- 是否添加了4.7kΩ上拉电阻到3.3V

### Q: 表情显示反色？
A: 已在本项目中修复，确保使用最新代码。

### Q: 如何修改动作？
A: 编辑 `main/pet/pet_actions.cc`，修改动作序列中的舵机角度。

## 🤝 贡献

欢迎提交Issue和Pull Request！

### 开发规范
- 使用Google C++代码风格
- 添加清晰的注释
- 测试后再提交

## 📝 更新日志

### v1.9.2-pet (2025-11-09)
- ✨ 新增4舵机控制系统
- ✨ 新增13种宠物动作
- ✨ 新增慵懒眨眼GIF动画
- ✨ 新增4种专业眨眼曲线
- 🐛 修复LVGL Canvas反色问题
- 🐛 修复多舵机同时启动导致重启的问题
- ⚡ 优化舵机分时启动策略
- 📝 完善文档和注释

## 📄 开源协议

本项目继承原项目的MIT协议，允许任何人免费使用或用于商业用途。

## 🙏 致谢

- [虾哥的 xiaozhi-esp32 项目](https://github.com/78/xiaozhi-esp32) - 原始项目
- [ESP-IDF](https://github.com/espressif/esp-idf) - ESP32开发框架
- [LVGL](https://lvgl.io/) - 轻量级图形库
- [dog-master](https://github.com/xxx/dog-master) - 动态眼睛设计灵感（待确认链接）

## 📧 联系方式

- Issues: [项目Issues页面](https://github.com/你的用户名/xiaozhi-esp32-pet/issues)
- 原项目QQ群: 1011329060

---

**Star History**

如果这个项目对你有帮助，请给个Star ⭐️

[![Star History Chart](https://api.star-history.com/svg?repos=你的用户名/xiaozhi-esp32-pet&type=Date)](https://star-history.com/#你的用户名/xiaozhi-esp32-pet&Date)
