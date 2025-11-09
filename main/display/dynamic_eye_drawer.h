// Dynamic Eye Drawer - U8g2 Drawing Layer
// Ported from dog project's EyeDrawer
// Copyright: Based on work by Luis Llamas and Alastair Aitchison

#ifndef DYNAMIC_EYE_DRAWER_H
#define DYNAMIC_EYE_DRAWER_H

#include <u8g2.h>
#include "dynamic_eye.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize U8g2 for SH1106 OLED
 * @param i2c_port I2C port number
 * @param sda_pin SDA GPIO pin
 * @param scl_pin SCL GPIO pin
 * @return Pointer to initialized u8g2 instance (static, do not free)
 */
u8g2_t* dynamic_eye_drawer_init(int i2c_port, int sda_pin, int scl_pin);

/**
 * Draw an eye at specified center position
 * @param u8g2 U8g2 instance
 * @param centerX Eye center X coordinate
 * @param centerY Eye center Y coordinate
 * @param config Eye configuration (shape parameters)
 */
void dynamic_eye_draw(u8g2_t* u8g2, int16_t centerX, int16_t centerY, const EyeConfig* config);

/**
 * Draw both eyes (left and right)
 * @param u8g2 U8g2 instance
 * @param config Eye configuration (same for both eyes)
 */
void dynamic_eye_draw_both(u8g2_t* u8g2, const EyeConfig* config);

/**
 * Clear the display
 * @param u8g2 U8g2 instance
 */
void dynamic_eye_clear(u8g2_t* u8g2);

#ifdef __cplusplus
}
#endif

#endif // DYNAMIC_EYE_DRAWER_H
