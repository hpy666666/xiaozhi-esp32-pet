// Dynamic Eye Drawer Implementation
// Ported from dog project's EyeDrawer
// Copyright: Based on work by Luis Llamas and Alastair Aitchison

#include "dynamic_eye_drawer.h"
#include "u8g2_esp32_hal.h"
#include <esp_log.h>
#include <math.h>

static const char* TAG = "DynamicEye";
static u8g2_t u8g2_instance;  // Static instance

// Helper: Fill rectangle
static void fill_rectangle(u8g2_t* u8g2, int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
    int32_t l = (x0 < x1) ? x0 : x1;
    int32_t r = (x0 > x1) ? x0 : x1;
    int32_t t = (y0 < y1) ? y0 : y1;
    int32_t b = (y0 > y1) ? y0 : y1;
    int32_t w = r - l;
    int32_t h = b - t;
    u8g2_DrawBox(u8g2, l, t, w, h);
}

// Helper: Fill triangular corner
static void fill_triangular_corner(u8g2_t* u8g2, int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
    u8g2_DrawTriangle(u8g2, x0, y0, x1, y1, x1, y0);
}

// Helper: Fill ellipse corner (Midpoint ellipse algorithm)
typedef enum { T_R, T_L, B_L, B_R } CornerType;

static void fill_ellipse_corner(u8g2_t* u8g2, CornerType corner, int16_t x0, int16_t y0, int32_t rx, int32_t ry) {
    if (rx < 2 || ry < 2) return;

    int32_t x, y;
    int32_t rx2 = rx * rx;
    int32_t ry2 = ry * ry;
    int32_t fx2 = 4 * rx2;
    int32_t fy2 = 4 * ry2;
    int32_t s;

    switch (corner) {
        case T_R:  // Top-right
            for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
                u8g2_DrawHLine(u8g2, x0, y0 - y, x);
                if (s >= 0) {
                    s += fx2 * (1 - y);
                    y--;
                }
                s += ry2 * ((4 * x) + 6);
            }
            for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
                u8g2_DrawHLine(u8g2, x0, y0 - y, x);
                if (s >= 0) {
                    s += fy2 * (1 - x);
                    x--;
                }
                s += rx2 * ((4 * y) + 6);
            }
            break;

        case B_R:  // Bottom-right
            for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
                u8g2_DrawHLine(u8g2, x0, y0 + y - 1, x);
                if (s >= 0) {
                    s += fx2 * (1 - y);
                    y--;
                }
                s += ry2 * ((4 * x) + 6);
            }
            for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
                u8g2_DrawHLine(u8g2, x0, y0 + y - 1, x);
                if (s >= 0) {
                    s += fy2 * (1 - x);
                    x--;
                }
                s += rx2 * ((4 * y) + 6);
            }
            break;

        case T_L:  // Top-left
            for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
                u8g2_DrawHLine(u8g2, x0 - x, y0 - y, x);
                if (s >= 0) {
                    s += fx2 * (1 - y);
                    y--;
                }
                s += ry2 * ((4 * x) + 6);
            }
            for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
                u8g2_DrawHLine(u8g2, x0 - x, y0 - y, x);
                if (s >= 0) {
                    s += fy2 * (1 - x);
                    x--;
                }
                s += rx2 * ((4 * y) + 6);
            }
            break;

        case B_L:  // Bottom-left
            for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
                u8g2_DrawHLine(u8g2, x0 - x, y0 + y - 1, x);
                if (s >= 0) {
                    s += fx2 * (1 - y);
                    y--;
                }
                s += ry2 * ((4 * x) + 6);
            }
            for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
                u8g2_DrawHLine(u8g2, x0 - x, y0 + y, x);
                if (s >= 0) {
                    s += fy2 * (1 - x);
                    x--;
                }
                s += rx2 * ((4 * y) + 6);
            }
            break;
    }
}

// Main drawing function (ported from dog project)
void dynamic_eye_draw(u8g2_t* u8g2, int16_t centerX, int16_t centerY, const EyeConfig* config) {
    // Amount by which corners will be shifted up/down based on requested "slope"
    int32_t delta_y_top = config->Height * config->Slope_Top / 2.0;
    int32_t delta_y_bottom = config->Height * config->Slope_Bottom / 2.0;

    // Full extent of the eye, after accounting for slope added at top and bottom
    int32_t totalHeight = config->Height + delta_y_top - delta_y_bottom;

    // Calculate inside corners of eye (TL, TR, BL, and BR) before any slope or rounded corners
    int32_t TLc_y = centerY + config->OffsetY - config->Height/2 + config->Radius_Top - delta_y_top;
    int32_t TLc_x = centerX + config->OffsetX - config->Width/2 + config->Radius_Top;
    int32_t TRc_y = centerY + config->OffsetY - config->Height/2 + config->Radius_Top + delta_y_top;
    int32_t TRc_x = centerX + config->OffsetX + config->Width/2 - config->Radius_Top;
    int32_t BLc_y = centerY + config->OffsetY + config->Height/2 - config->Radius_Bottom - delta_y_bottom;
    int32_t BLc_x = centerX + config->OffsetX - config->Width/2 + config->Radius_Bottom;
    int32_t BRc_y = centerY + config->OffsetY + config->Height/2 - config->Radius_Bottom + delta_y_bottom;
    int32_t BRc_x = centerX + config->OffsetX + config->Width/2 - config->Radius_Bottom;

    // Calculate interior extents
    int32_t min_c_x = (TLc_x < BLc_x) ? TLc_x : BLc_x;
    int32_t max_c_x = (TRc_x > BRc_x) ? TRc_x : BRc_x;
    int32_t min_c_y = (TLc_y < TRc_y) ? TLc_y : TRc_y;
    int32_t max_c_y = (BLc_y > BRc_y) ? BLc_y : BRc_y;

    // Fill eye center
    fill_rectangle(u8g2, min_c_x, min_c_y, max_c_x, max_c_y);

    // Fill eye outwards to meet edges of rounded corners
    fill_rectangle(u8g2, TRc_x, TRc_y, BRc_x + config->Radius_Bottom, BRc_y); // Right
    fill_rectangle(u8g2, TLc_x - config->Radius_Top, TLc_y, BLc_x, BLc_y); // Left
    fill_rectangle(u8g2, TLc_x, TLc_y - config->Radius_Top, TRc_x, TRc_y); // Top
    fill_rectangle(u8g2, BLc_x, BLc_y, BRc_x, BRc_y + config->Radius_Bottom); // Bottom

    // Draw slanted edges at top of eye
    if (config->Slope_Top > 0) {
        fill_triangular_corner(u8g2, TLc_x, TLc_y - config->Radius_Top, TRc_x, TRc_y - config->Radius_Top);
    } else if (config->Slope_Top < 0) {
        fill_triangular_corner(u8g2, TRc_x, TRc_y - config->Radius_Top, TLc_x, TLc_y - config->Radius_Top);
    }

    // Draw slanted edges at bottom of eye
    if (config->Slope_Bottom > 0) {
        fill_triangular_corner(u8g2, BRc_x + config->Radius_Bottom, BRc_y + config->Radius_Bottom,
                                     BLc_x - config->Radius_Bottom, BLc_y + config->Radius_Bottom);
    } else if (config->Slope_Bottom < 0) {
        fill_triangular_corner(u8g2, BLc_x - config->Radius_Bottom, BLc_y + config->Radius_Bottom,
                                     BRc_x + config->Radius_Bottom, BRc_y + config->Radius_Bottom);
    }

    // Draw rounded corners
    if (config->Radius_Top > 0) {
        fill_ellipse_corner(u8g2, T_L, TLc_x, TLc_y, config->Radius_Top, config->Radius_Top);
        fill_ellipse_corner(u8g2, T_R, TRc_x, TRc_y, config->Radius_Top, config->Radius_Top);
    }
    if (config->Radius_Bottom > 0) {
        fill_ellipse_corner(u8g2, B_L, BLc_x, BLc_y, config->Radius_Bottom, config->Radius_Bottom);
        fill_ellipse_corner(u8g2, B_R, BRc_x, BRc_y, config->Radius_Bottom, config->Radius_Bottom);
    }
}

void dynamic_eye_draw_both(u8g2_t* u8g2, const EyeConfig* config) {
    u8g2_ClearBuffer(u8g2);
    u8g2_SetDrawColor(u8g2, 1);  // White pixels (blue on OLED)

    // Draw left and right eyes
    dynamic_eye_draw(u8g2, LEFT_EYE_X, EYE_Y, config);
    dynamic_eye_draw(u8g2, RIGHT_EYE_X, EYE_Y, config);

    u8g2_SendBuffer(u8g2);
}

void dynamic_eye_clear(u8g2_t* u8g2) {
    u8g2_ClearBuffer(u8g2);
    u8g2_SendBuffer(u8g2);
}

u8g2_t* dynamic_eye_drawer_init(int i2c_port, int sda_pin, int scl_pin) {
    ESP_LOGI(TAG, "Initializing U8g2 for SH1106 (I2C port=%d, SDA=%d, SCL=%d)", i2c_port, sda_pin, scl_pin);

    // Initialize U8g2 for SH1106 128x64 OLED in I2C mode
    u8g2_Setup_sh1106_i2c_128x64_noname_f(
        &u8g2_instance,
        U8G2_R0,  // Rotation
        u8g2_esp32_i2c_byte_cb,  // Byte callback
        u8g2_esp32_gpio_and_delay_cb  // GPIO and delay callback
    );

    // Set I2C parameters
    u8g2_SetI2CAddress(&u8g2_instance, 0x3C << 1);  // SH1106 I2C address (0x3C)

    // Initialize display
    u8g2_InitDisplay(&u8g2_instance);
    u8g2_SetPowerSave(&u8g2_instance, 0);  // Wake up display
    u8g2_ClearBuffer(&u8g2_instance);
    u8g2_SendBuffer(&u8g2_instance);

    ESP_LOGI(TAG, "U8g2 initialized successfully");
    return &u8g2_instance;
}
