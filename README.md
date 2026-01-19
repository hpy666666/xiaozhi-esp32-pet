# Xiaozhi ESP32 Pet - 桌面宠物机器人 🐾

> 🔗 **Fork 自** [xiaozhi-esp32](https://github.com/78/xiaozhi-esp32) | 专注于 **xiaozhi-pet 桌面宠物机器人** 版本的开发和优化

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v5.4+-blue.svg)](https://github.com/espressif/esp-idf)
[![LVGL](https://img.shields.io/badge/LVGL-v9-green.svg)](https://lvgl.io/)

**本项目基于原版 xiaozhi-esp32，专注于桌面宠物机器人的开发。增加了4舵机控制系统和动态表情系统，让小智真正"动"起来！**

> 💡 **注意**: 本仓库只维护 **xiaozhi-pet** 板子配置。如需其他开发板支持（ESP-BOX、M5Stack等），请参考原项目 [xiaozhi-esp32](https://github.com/78/xiaozhi-esp32) 或查看 [docs/other-boards/](docs/other-boards/)。

## 📸 项目展示

<div align="center">

### 整体外观
![项目封面](docs/pet/cover.jpg)

### 硬件细节
<table>
  <tr>
    <td><img src="docs/pet/hardware-left.jpg" width="360"/><br/>左侧视角 - 舵机布局</td>
    <td><img src="docs/pet/hardware-right.jpg" width="360"/><br/>右侧视角 - OLED屏幕</td>
  </tr>
</table>

> 💡 **提示**: 点击图片可查看大图

</div>

---

## 📖 目录

- [新增特性](#-新增特性)
- [硬件需求](#-硬件需求)
  - [硬件设计文件](#-硬件设计文件)
- [快速开始](#-快速开始)
- [动画制作工具](#-动画制作工具)
- [常见问题](#-常见问题)
- [贡献指南](#-贡献指南)
- [致谢](#-致谢)

---

## ✨ 新增特性

相比原版 [xiaozhi-esp32](https://github.com/78/xiaozhi-esp32)，本项目新增了以下功能：

### 🤖 舵机控制系统
- ✅ **4个舵机驱动**: 使用ESP32的LEDC PWM控制4个SG90舵机
- ✅ **13种预定义动作**: 站立、趴下、坐下、前进、后退、左转、右转、挥手、摇头、点头等
- ✅ **MCP协议集成**: 通过语音指令控制宠物动作
- ✅ **分时启动优化**: 解决多舵机同时启动的电流冲击问题

### 😊 动态表情系统
- ✅ **慵懒眨眼GIF动画**: 基于LVGL v9的GIF动画，5.8秒循环，70帧
- ✅ **4种专业眨眼曲线**:
  - `lazy` - 打瞌睡效果（分段式，有故事感）
  - `gaussian` - 高斯分布（最真实，符合人眼运动）
  - `sine` - 正弦曲线（最平滑，经典动画）
  - `smootherstep` - 游戏引擎标准曲线
- ✅ **18种静态位图表情**: 配合设备状态和宠物动作自动切换
- ✅ **LVGL Canvas显示**: 表情与UI共存，解决反色问题

### ⚡ 电源管理优化
- ✅ **MT3608升压方案**: 3.7V锂电池升压至5V供电
- ✅ **舵机分时启动**: 间隔50ms依次启动，避免电压跌落
- ✅ **电流优化策略**: 峰值电流从2300mA降至1300mA
- ✅ **Brownout保护**: 防止因电流过大导致系统重启

---

## 🛠️ 硬件需求

### 必需硬件

| 组件 | 型号 | 数量 | 说明 |
|------|------|------|------|
| 主控芯片 | ESP32-S3-N16R8 | 1 | 16MB Flash + 8MB PSRAM |
| OLED显示屏 | SH1106 (128×64) | 1 | I2C接口 |
| 麦克风 | INMP441 | 1 | I2S数字麦克风 |
| 音频功放 | MAX98357A | 1 | I2S功放 |
| 舵机 | SG90 | 4 | 微型舵机 |
| 升压模块 | MT3608 | 1 | 3.7V→5V, 2A+ |
| 锂电池 | 18650 | 1 | 3.7V, 2000mAh+ |
| 充电模块 | TP4056 | 1 | 1A充电 |

### 引脚定义

详见 [`main/boards/xiaozhi-pet/config.h`](main/boards/xiaozhi-pet/config.h)

```c
// 音频输出 (I2S)
AUDIO_OUTPUT_BCLK_PIN = 15
AUDIO_OUTPUT_WS_PIN = 16
AUDIO_OUTPUT_DOUT_PIN = 7

// 麦克风输入 (I2S)
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

### 电路设计要点

- 每个舵机旁添加 **220μF电解电容**
- OLED I2C需要 **4.7kΩ上拉电阻** 到3.3V
- 麦克风和扬声器保持 **5cm以上距离**
- 舵机信号线建议添加 **10kΩ上拉电阻**

### 📐 硬件设计文件

本项目提供完整的原理图和PCB设计文件（嘉立创EDA格式），方便大家自制硬件。

#### 设计文件

所有硬件设计文件位于 [`hardware/`](hardware/) 目录：

| 文件 | 说明 |
|------|------|
| [`ProDoc_Schematic1_2026-01-19.epro`](hardware/ProDoc_Schematic1_2026-01-19.epro) | 完整原理图工程文件（嘉立创EDA） |
| [`ProDoc_PCB2_2026-01-19.epro`](hardware/ProDoc_PCB2_2026-01-19.epro) | 完整PCB工程文件（嘉立创EDA） |
| [`SCH_Schematic1_1-P1_2026-01-19.png`](hardware/SCH_Schematic1_1-P1_2026-01-19.png) | 原理图预览图片 |
| [`pcb图片.png`](hardware/pcb图片.png) | PCB布局预览图片 |

#### 原理图和PCB预览

<div align="center">

**原理图**
![原理图](hardware/SCH_Schematic1_1-P1_2026-01-19.png)

**PCB布局**
![PCB布局](hardware/pcb图片.png)

</div>

#### 设计说明

- **设计软件**: 嘉立创EDA
- **PCB层数**: 2层板
- **PCB厚度**: 1.6mm
- **铜箔厚度**: 1oz
- **制造商**: 推荐使用嘉立创、华秋等国内PCB厂商

#### 关键设计要点

- 每个舵机旁添加 **220μF电解电容**，防止电压跌落
- OLED I2C需要 **4.7kΩ上拉电阻** 到3.3V
- 麦克风和扬声器保持 **5cm以上距离**，避免啸叫
- 舵机信号线建议添加 **10kΩ上拉电阻**

> 💡 **提示**: 所有硬件设计文件均可免费使用，遵循 MIT 开源协议。如果你对硬件设计进行了改进，欢迎提交PR分享！

---

## 🚀 快速开始

### 方式1：使用预编译固件（推荐新手）

#### 1. 下载固件

访问 [Releases 页面](https://github.com/hpy666666/xiaozhi-esp32-pet/releases) 下载最新的 `v1.9.2_xiaozhi-pet.zip`

#### 2. 解压文件

```
v1.9.2_xiaozhi-pet/
├── bootloader.bin
├── partition-table.bin
├── xiaozhi.bin
└── flash_download.txt  (烧录说明)
```

#### 3. 烧录固件

**Windows用户** - 使用 ESP Flash Download Tool:
1. 下载工具: [Flash Download Tool](https://www.espressif.com/en/support/download/other-tools)
2. 选择芯片: ESP32-S3
3. 设置烧录地址:
   ```
   bootloader.bin       -> 0x0
   partition-table.bin  -> 0x8000
   xiaozhi.bin          -> 0x10000
   ```
4. 选择串口（如COM5），波特率921600
5. 点击 START 开始烧录

**跨平台用户** - 使用 esptool.py:
```bash
pip install esptool

esptool.py --chip esp32s3 --port COM5 --baud 921600 \
  write_flash 0x0 bootloader.bin \
  0x8000 partition-table.bin \
  0x10000 xiaozhi.bin
```

#### 4. 配置网络

首次启动后，设备会创建Wi-Fi热点:
- **SSID**: `Xiaozhi_XXXXXX`
- **密码**: `12345678`

连接热点后访问 `http://192.168.4.1` 配置网络和服务器。

---

### 方式2：自行编译（开发者）

#### 1. 安装 ESP-IDF

**Windows**:
```bash
# 下载并安装 ESP-IDF v5.4+
# 参考: https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/windows-setup.html
```

**Linux/macOS**:
```bash
git clone -b v5.4 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32s3
. ./export.sh
```

#### 2. 克隆项目

```bash
git clone https://github.com/hpy666666/xiaozhi-esp32-pet.git
cd xiaozhi-esp32-pet
```

#### 3. 编译和烧录

```bash
# 使用release脚本编译
python scripts/release.py xiaozhi-pet

# 或手动编译
idf.py set-target esp32s3
idf.py build

# 烧录到设备
idf.py -p COM5 flash monitor
```

退出监视器: `Ctrl + ]`

---

## 🎨 动画制作工具

项目包含完整的动画生成工具，可以自己制作眨眼动画！

### 工具链

```
Python脚本生成MP4 → ezgif转GIF → LVGL转C数组 → 集成到项目
```

### 第1步：生成MP4动画

使用项目中的 `generate_lazy_blink.py`（需要Python 3.7+）:

```bash
# 安装依赖
pip install pillow opencv-python numpy

# 生成不同曲线的动画
python generate_lazy_blink.py lazy          # 打瞌睡曲线
python generate_lazy_blink.py gaussian      # 高斯曲线（推荐）
python generate_lazy_blink.py sine          # 正弦曲线
python generate_lazy_blink.py smootherstep  # 游戏标准曲线
```

生成的MP4文件:
```
lazy_blink_lazy.mp4
lazy_blink_gaussian.mp4
lazy_blink_sine.mp4
lazy_blink_smootherstep.mp4
```

### 第2步：转换为GIF

访问 **[ezgif.com - Video to GIF](https://ezgif.com/video-to-gif)**

1. 上传生成的 `.mp4` 文件
2. 设置参数:
   - **Size**: `128x64` (必须匹配OLED分辨率)
   - **Frame rate**: `12 fps`
   - **Method**: `Optimize` (压缩文件大小)
3. 点击 "Convert to GIF"
4. 下载生成的 `.gif` 文件

### 第3步：转换为C数组

访问 **[LVGL Image Converter](https://lvgl.io/tools/imageconverter)**

1. 上传 `.gif` 文件
2. 设置参数:
   - **Name**: `lazy_blink_gif`
   - **Color format**: `CF_RAW` (GIF动画必须用RAW)
   - **Output format**: `C array`
   - **LVGL version**: `v9` (重要！)
3. 点击 "Convert"
4. 下载生成的 `.c` 文件

### 第4步：集成到项目

1. 将下载的 `.c` 文件重命名为 `lazy_blink_gif.c`
2. 复制到 `main/display/` 目录
3. 修改文件格式为LVGL v9（如需要）:

```c
// 确保使用v9格式
const lv_image_dsc_t lazy_blink_gif = {
    .header = {
        .cf = LV_COLOR_FORMAT_RAW,  // v9格式
        .w = 128,
        .h = 64,
    },
    .data_size = 6873,
    .data = lazy_blink_gif_map,
};
```

4. 重新编译烧录即可

---

## 🔧 开发工具和资源

### 在线工具

| 工具 | 用途 | 链接 |
|------|------|------|
| ezgif | MP4转GIF | https://ezgif.com/video-to-gif |
| LVGL Image Converter | GIF转C数组 | https://lvgl.io/tools/imageconverter |
| image2lcd | 位图转数组（备用） | http://www.image2lcd.com/ |
| ESP Flash Tool | Windows烧录工具 | https://www.espressif.com/en/support/download/other-tools |

### 开发文档

| 文档 | 链接 |
|------|------|
| ESP-IDF 官方文档 | https://docs.espressif.com/projects/esp-idf/ |
| LVGL v9 文档 | https://docs.lvgl.io/master/ |
| xiaozhi-esp32 原项目 | https://github.com/78/xiaozhi-esp32 |
| 自定义开发板指南 | [main/boards/README.md](main/boards/README.md) |
| MCP协议文档 | [docs/mcp-protocol.md](docs/mcp-protocol.md) |

### Python依赖

```bash
# 动画生成工具依赖
pip install pillow        # 图像处理
pip install opencv-python # 视频生成
pip install numpy         # 数值计算
```

---

## 📂 项目结构

```
xiaozhi-esp32-pet/
│
├── main/
│   ├── boards/
│   │   └── xiaozhi-pet/          # 🌟 宠物版硬件配置
│   ├── pet/                       # 🌟 舵机控制模块
│   │   ├── pet_servo.cc/h         # 舵机PWM驱动
│   │   ├── pet_actions.cc/h       # 13种动作库
│   │   └── pet_controller.cc/h    # MCP协议集成
│   ├── display/                   # 🌟 显示模块
│   │   ├── oled_display.cc/h      # OLED驱动
│   │   ├── emotion_bitmaps.c/h    # 18种静态表情
│   │   ├── emotion_manager.c/h    # 表情映射
│   │   └── lazy_blink_gif.c       # 🌟 慵懒眨眼GIF
│   ├── audio/                     # 音频模块
│   ├── network/                   # 网络通信
│   └── application.cc             # 主程序
│
├── scripts/
│   └── release.py                 # 编译打包脚本
│
├── docs/                          # 文档和图片
├── generate_lazy_blink.py         # 🌟 动画生成工具
└── README.md                      # 本文件
```

---

## 🐛 常见问题

### Q1: 连接两个以上舵机就自动重启？

**原因**: 电源电流不足，多个舵机同时启动导致电压跌落触发brownout。

**解决方案**:
1. ✅ 本项目已实现**分时启动优化**（间隔50ms）
2. 确保MT3608升压模块输出能力 ≥ 2A
3. 每个舵机旁添加220μF电解电容
4. 检查电池容量（建议 ≥ 2000mAh）

### Q2: OLED显示屏不显示或反色？

**检查清单**:
- [ ] I2C引脚是否接对（SDA/SCL）
- [ ] 是否添加4.7kΩ上拉电阻到3.3V
- [ ] I2C地址是否正确（SH1106通常是0x3C）
- [ ] 电源是否稳定（3.3V）

**反色问题**: 本项目已修复LVGL Canvas反色问题，确保使用最新代码。

### Q3: GIF动画不播放？

**检查步骤**:
1. 确认GIF文件格式为 `CF_RAW`（不是CF_INDEXED）
2. 确认使用LVGL v9格式（`lv_image_dsc_t`）
3. 检查sdkconfig中是否启用GIF支持:
   ```
   CONFIG_LV_USE_GIF=y
   ```
4. 重新编译完整项目

### Q4: 如何修改动作？

编辑 `main/pet/pet_actions.cc`，修改动作序列中的舵机角度:

```cpp
// 示例：修改挥手动作
ActionSequence wave_action = {
    {90, 45, 90, 90},   // 第1帧：右前腿抬起
    {90, 90, 90, 90},   // 第2帧：放下
    {90, 45, 90, 90},   // 第3帧：再次抬起
    {90, 90, 90, 90},   // 第4帧：放下
};
```

### Q5: 烧录后无法连接串口监视器？

**Windows用户**:
1. 安装CH340或CP2102驱动
2. 检查设备管理器中的COM端口号
3. 尝试更换USB线（需要支持数据传输）

**所有平台**:
```bash
# 查看可用端口
idf.py monitor --list-ports

# 指定端口和波特率
idf.py -p COM5 -b 115200 monitor
```

---

## 🤝 贡献指南

欢迎提交Issue和Pull Request！

### 开发规范
- 使用Google C++代码风格
- 添加清晰的注释（中英文均可）
- 测试后再提交
- 提交信息格式: `feat:` / `fix:` / `docs:` / `refactor:`

### 提交PR步骤
1. Fork本项目
2. 创建新分支: `git checkout -b feature/your-feature`
3. 提交修改: `git commit -m 'feat: Add some feature'`
4. 推送分支: `git push origin feature/your-feature`
5. 提交Pull Request

---

## 📝 更新日志

### v1.9.3 (2026-01-19)
- 📦 精简项目结构，专注xiaozhi-pet版本开发
- 🗂️ 整理文档结构，移动92个其他板子配置到 `docs/other-boards/`
- 🔧 更新硬件设计文件（嘉立创EDA格式）

### v1.9.2-pet (2025-11-09)
- 🎉 初始发布
- ✨ 新增4舵机控制系统（13种动作）
- ✨ 新增慵懒眨眼GIF动画（4种曲线）
- ✨ 新增18种静态位图表情
- ⚡ 优化电源管理（分时启动）
- 🐛 修复LVGL Canvas反色问题
- 🐛 修复多舵机同时启动导致重启的问题
- 📝 完善文档和代码注释

---

## 📄 开源协议

本项目继承原项目的 [MIT License](LICENSE)，允许任何人免费使用或用于商业用途。

---

## 🙏 致谢

- **原项目作者**: [78/虾哥](https://github.com/78) - [xiaozhi-esp32](https://github.com/78/xiaozhi-esp32) 开源项目
- **开发框架**: [ESP-IDF](https://github.com/espressif/esp-idf) by Espressif Systems
- **图形库**: [LVGL](https://lvgl.io/) - 轻量级嵌入式图形库
- **PCB打板**: [嘉立创](https://www.jlc.com/) - 感谢嘉立创提供的免费打板服务
- **灵感来源**: 各种桌面宠物机器人和开源硬件社区

---

## 📧 联系方式

- **Issues**: [项目Issues页面](https://github.com/hpy666666/xiaozhi-esp32-pet/issues)
- **原项目QQ群**: 1011329060

---

## ⭐ Star History

如果这个项目对你有帮助，请给个Star支持一下！

[![Star History Chart](https://api.star-history.com/svg?repos=hpy666666/xiaozhi-esp32-pet&type=Date)](https://star-history.com/#hpy666666/xiaozhi-esp32-pet&Date)

---

**Made with ❤️ by hpy666666**
