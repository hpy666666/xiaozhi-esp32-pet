# Other Supported Boards

This directory contains board configurations for 92+ different ESP32 hardware platforms supported by the original xiaozhi-esp32 project.

## 关于本目录

本目录包含原 xiaozhi-esp32 项目支持的其他 92+ 个开发板配置。

由于本仓库专注于 **xiaozhi-pet（桌面宠物机器人）** 版本的开发，这些其他开发板的配置文件已移至此处存档。

## 主要开发板

主项目 `main/boards/` 目录只保留：
- **xiaozhi-pet** - 桌面宠物机器人配置（本项目主要开发版本）
- **common** - 公共代码和通用配置

## 支持的其他开发板

本目录包含以下开发板的完整配置：

### 官方开发板
- ESP-BOX, ESP-BOX-3, ESP-BOX-Lite
- ESP32-S3-LCD-EV-Board
- ESP-HI
- ESP-SparkBot
- ESP-SPOT-S3

### M5Stack 系列
- M5Stack CoreS3
- M5Stack Tab5
- AtomMatrix Echo Base
- Atoms3 Echo Base
- Atoms3R Echo Base

### Waveshare 系列
- 多款触摸屏和AMOLED显示屏开发板
- ESP32-C6 和 ESP32-P4 系列

### LilyGO 系列
- T-Camera Plus S3
- T-Circle S3
- T-Display S3 Pro

### 其他厂商
- 正点原子（ATK）系列
- 立创（LiChuang）系列
- MagiClick 系列
- Kevin 系列
- 以及更多...

## 如何使用其他开发板

如果你想使用本目录中的其他开发板配置：

1. **将对应的板子文件夹复制回** `main/boards/`
   ```bash
   cp -r docs/other-boards/your-board main/boards/
   ```

2. **编译时指定板子名称**
   ```bash
   python scripts/release.py your-board
   ```

3. **或手动编译**
   ```bash
   idf.py set-target esp32s3
   idf.py -DBOARD=your-board build
   ```

## 参考资源

- 原项目仓库: https://github.com/78/xiaozhi-esp32
- 开发板文档: [main/boards/README.md](../../main/boards/README.md)
- 支持的芯片: ESP32, ESP32-C3, ESP32-C6, ESP32-S3, ESP32-P4

## 贡献

如果你为某个开发板添加了新功能或修复了问题，欢迎提交 Pull Request！

---

**注意**: 本项目重点维护 xiaozhi-pet 版本。其他开发板的配置保留供参考，但可能不会频繁更新。如需最新的多板支持，请参考原项目 [xiaozhi-esp32](https://github.com/78/xiaozhi-esp32)。
