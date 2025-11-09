// U8g2 ESP32 Hardware Abstraction Layer
// Provides I2C and GPIO callbacks for U8g2 library

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <u8g2.h>

/**
 * I2C byte callback for U8g2
 * Handles I2C communication with the display
 */
uint8_t u8g2_esp32_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

/**
 * GPIO and delay callback for U8g2
 * Handles delays and GPIO operations
 */
uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#ifdef __cplusplus
}
#endif
