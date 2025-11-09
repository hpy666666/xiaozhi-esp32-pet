# Xiaozhi Pet - 快速烧录指南

## 方法1：使用 Python 脚本生成固件（推荐）

### 步骤：

1. **打开 ESP-IDF 命令行工具**
   - 开始菜单 → ESP-IDF → ESP-IDF 4.4 CMD (或你安装的版本)

2. **进入项目目录**
   ```cmd
   cd C:\Users\34376\Desktop\xiaozhi-esp32-1.9.2_2\xiaozhi-esp32-1.9.2_2
   ```

3. **运行烧录脚本**（已创建）
   ```cmd
   build_xiaozhi_pet.bat
   ```

4. **等待编译完成**
   - 编译需要3-5分钟
   - 完成后固件在 `releases\` 目录

5. **烧录固件**
   - 使用作者的烧录软件
   - 选择 `releases\vX.X.X_xiaozhi-pet.zip` 解压后的 `merged-binary.bin`
   - 烧录地址：`0x0`（从0地址开始）

---

## 方法2：直接使用 idf.py

### 如果上面的脚本失败，手动执行：

```cmd
# 1. 打开 ESP-IDF CMD

# 2. 进入项目目录
cd C:\Users\34376\Desktop\xiaozhi-esp32-1.9.2_2\xiaozhi-esp32-1.9.2_2

# 3. 设置环境变量
set CONFIG_BOARD_TYPE_XIAOZHI_PET=y

# 4. 设置目标
idf.py set-target esp32s3

# 5. 编译
idf.py build

# 6. 合并固件（单个bin文件）
idf.py merge-bin

# 7. 烧录（假设COM口是COM3）
idf.py -p COM3 flash
```

---

## 方法3：使用 release.py 脚本

```cmd
# 进入项目目录
cd C:\Users\34376\Desktop\xiaozhi-esp32-1.9.2_2\xiaozhi-esp32-1.9.2_2

# 生成固件
python scripts\release.py xiaozhi-pet
```

固件会生成到 `releases\vX.X.X_xiaozhi-pet.zip`

---

## 烧录说明

### 使用作者的烧录软件（最简单）

1. 解压 `releases\vX.X.X_xiaozhi-pet.zip`
2. 打开烧录软件
3. 选择 `merged-binary.bin`
4. 烧录地址：`0x0`
5. 点击烧录

### 使用 esptool（命令行）

```cmd
esptool.py -p COM3 -b 460800 --chip esp32s3 write_flash 0x0 build\merged-binary.bin
```

---

## 验证烧录

烧录完成后，打开串口监控（115200波特率）：

```
I (1234) PetServo: Initializing pet servos on GPIO9-12
I (1235) PetServo: LEDC timer configured: 50Hz, 13-bit resolution
I (1236) PetServo: Servo 0 initialized on GPIO9 (channel 0)
I (1237) PetServo: Servo 1 initialized on GPIO10 (channel 1)
I (1238) PetServo: Servo 2 initialized on GPIO11 (channel 2)
I (1239) PetServo: Servo 3 initialized on GPIO12 (channel 3)
I (1240) PetServo: All servos initialized to 90 degrees
```

看到这些日志说明烧录成功！舵机会自动移动到90度位置。

---

## 常见问题

### Q1: 编译失败 "CONFIG_BOARD_TYPE_XIAOZHI_PET not defined"
**A**: 需要先运行 `set CONFIG_BOARD_TYPE_XIAOZHI_PET=y` 或使用 menuconfig 配置

### Q2: 找不到 COM 口
**A**:
- Windows: 设备管理器查看端口号（如COM3）
- 确保USB驱动已安装（CP210x 或 CH340）

### Q3: 编译很慢
**A**: 第一次编译需要3-5分钟，后续增量编译很快（几十秒）

### Q4: 想用原版固件格式
**A**: 使用 `idf.py build` 后，所有分区bin文件在 `build/` 目录：
- bootloader.bin
- partition-table.bin
- xiaozhi-esp32.bin
- 等等

但推荐使用 `merged-binary.bin`，只需烧录一个文件！

---

## 下一步

烧录成功后：
1. 长按BOOT按钮（GPIO0）→ 测试"打招呼"动作
2. 配置WiFi和云端
3. 通过语音控制："小智，站起来"

详细使用说明见：`main/boards/xiaozhi-pet/README.md`
