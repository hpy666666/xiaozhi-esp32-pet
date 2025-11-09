// Idle Emotion Controller - Parametric Eye Blinking
// Replaces bitmap-based blinking with dynamic eye drawing

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <esp_timer.h>
#include <u8g2.h>
#include "dynamic_eye.h"

/**
 * Idle animation controller
 * Manages automatic blinking with parametric eyes
 */
typedef struct {
    u8g2_t* u8g2;                      // U8g2 instance
    esp_timer_handle_t blink_timer;    // Random blink trigger timer
    esp_timer_handle_t frame_timer;    // Blink animation frame timer
    int current_frame;                 // Current frame of blink animation (0-4)
    bool is_active;                    // Is idle animation running
    bool is_blinking;                  // Is currently playing blink animation
} IdleEmotionController;

/**
 * Initialize idle emotion controller
 * @param ctrl Controller instance
 * @param u8g2 U8g2 instance for drawing
 */
void idle_controller_init(IdleEmotionController* ctrl, u8g2_t* u8g2);

/**
 * Start idle animation (blinking eyes)
 */
void idle_controller_start(IdleEmotionController* ctrl);

/**
 * Stop idle animation
 */
void idle_controller_stop(IdleEmotionController* ctrl);

/**
 * Clean up controller resources
 */
void idle_controller_deinit(IdleEmotionController* ctrl);

#ifdef __cplusplus
}
#endif
