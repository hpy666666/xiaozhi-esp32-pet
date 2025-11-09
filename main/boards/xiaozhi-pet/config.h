#ifndef CONFIG_H
#define CONFIG_H

// ========== 基础配置 ==========
#define BOARD_NAME "xiaozhi-pet"
#define BOARD_HAS_PSRAM 1

// ========== 音频配置 ==========
#define AUDIO_OUTPUT_MAX98357A
#define AUDIO_INPUT_INMP441

// MAX98357A 音频输出引脚
#define AUDIO_OUTPUT_BCLK_PIN 15
#define AUDIO_OUTPUT_WS_PIN 16
#define AUDIO_OUTPUT_DOUT_PIN 7

// INMP441 麦克风输入引脚
#define AUDIO_INPUT_BCLK_PIN 5
#define AUDIO_INPUT_WS_PIN 4
#define AUDIO_INPUT_DIN_PIN 6

#define AUDIO_INPUT_SAMPLE_RATE 16000
#define AUDIO_INPUT_REFERENCE 1100

// ========== 显示配置 ==========
#define DISPLAY_OLED_128x32

// OLED I2C引脚
#define DISPLAY_I2C_SDA_PIN GPIO_NUM_41
#define DISPLAY_I2C_SCL_PIN GPIO_NUM_42
#define DISPLAY_I2C_FREQ_HZ 400000

// ========== 舵机配置 ==========
#define PET_SERVO_ENABLED
#define PET_SERVO_COUNT 4

// 舵机引脚 (GPIO9-12)
#define PET_SERVO_PIN_1 9   // 左前腿
#define PET_SERVO_PIN_2 10  // 右前腿
#define PET_SERVO_PIN_3 11  // 左后腿
#define PET_SERVO_PIN_4 12  // 右后腿

// ========== 按键配置 ==========
#define BUTTON_BOOT_PIN 0

// ========== 网络配置 ==========
#define NETWORK_WIFI

// ========== 电源配置 ==========
// 无电池管理

#endif // CONFIG_H
