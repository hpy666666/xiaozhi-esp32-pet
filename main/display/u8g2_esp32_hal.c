// U8g2 ESP32 Hardware Abstraction Layer Implementation
// Provides I2C communication for SH1106 OLED displays

#include "u8g2_esp32_hal.h"
#include <esp_log.h>
#include <driver/i2c_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char* TAG = "U8g2HAL";

// External I2C bus handle (shared with LVGL)
extern i2c_master_bus_handle_t i2c_bus_handle;

// I2C device handle for U8g2
static i2c_master_dev_handle_t u8g2_i2c_dev = NULL;

/**
 * I2C byte callback for U8g2
 * Handles I2C START, STOP, byte transmission, etc.
 */
uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    static uint8_t buffer[128];
    static uint8_t buf_idx = 0;

    switch(msg) {
        case U8X8_MSG_BYTE_INIT:
            // Initialize I2C device handle (use existing bus)
            if (u8g2_i2c_dev == NULL && i2c_bus_handle != NULL) {
                i2c_device_config_t dev_cfg = {
                    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
                    .device_address = u8x8_GetI2CAddress(u8x8) >> 1,  // U8g2 uses 8-bit address
                    .scl_speed_hz = 400000,  // 400kHz
                };
                esp_err_t ret = i2c_master_bus_add_device(i2c_bus_handle, &dev_cfg, &u8g2_i2c_dev);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to add I2C device: %s", esp_err_to_name(ret));
                    return 0;
                }
                ESP_LOGI(TAG, "U8g2 I2C device initialized (addr=0x%02X)", dev_cfg.device_address);
            }
            break;

        case U8X8_MSG_BYTE_START_TRANSFER:
            // Start of transmission - reset buffer
            buf_idx = 0;
            break;

        case U8X8_MSG_BYTE_SEND:
            // Accumulate bytes in buffer
            {
                uint8_t* data = (uint8_t*)arg_ptr;
                for (uint8_t i = 0; i < arg_int; i++) {
                    buffer[buf_idx++] = data[i];
                }
            }
            break;

        case U8X8_MSG_BYTE_END_TRANSFER:
            // Send accumulated buffer via I2C
            if (u8g2_i2c_dev != NULL && buf_idx > 0) {
                esp_err_t ret = i2c_master_transmit(u8g2_i2c_dev, buffer, buf_idx, 1000 / portTICK_PERIOD_MS);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "I2C transmit failed: %s", esp_err_to_name(ret));
                    return 0;
                }
            }
            buf_idx = 0;
            break;

        case U8X8_MSG_BYTE_SET_DC:
            // Not used for I2C
            break;

        default:
            return 0;
    }
    return 1;
}

/**
 * GPIO and delay callback for U8g2
 * Handles microsecond/millisecond delays
 */
uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch(msg) {
        case U8X8_MSG_DELAY_NANO:
            // Nanoseconds delay (not precisely supported, use microseconds)
            esp_rom_delay_us(arg_int == 0 ? 0 : 1);
            break;

        case U8X8_MSG_DELAY_100NANO:
            // 100 nanoseconds
            esp_rom_delay_us(arg_int == 0 ? 0 : 1);
            break;

        case U8X8_MSG_DELAY_10MICRO:
            // 10 microseconds
            esp_rom_delay_us(arg_int * 10);
            break;

        case U8X8_MSG_DELAY_MILLI:
            // Milliseconds
            vTaskDelay(pdMS_TO_TICKS(arg_int));
            break;

        case U8X8_MSG_DELAY_I2C:
            // I2C clock delay (5us for 100kHz)
            esp_rom_delay_us(5);
            break;

        case U8X8_MSG_GPIO_I2C_CLOCK:
        case U8X8_MSG_GPIO_I2C_DATA:
            // GPIO operations (not used for hardware I2C)
            break;

        default:
            return 0;
    }
    return 1;
}
