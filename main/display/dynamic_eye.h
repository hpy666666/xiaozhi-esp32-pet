// Dynamic Eye Configuration
// Parametric eye drawing system ported from dog project
// Copyright: Based on work by Luis Llamas (www.luisllamas.es)

#ifndef DYNAMIC_EYE_H
#define DYNAMIC_EYE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * EyeConfig: Parametric configuration for eye shape
 * All measurements in pixels
 */
typedef struct {
    int16_t OffsetX;          // Horizontal offset from center
    int16_t OffsetY;          // Vertical offset from center
    int16_t Height;           // Eye height (changes during blink)
    int16_t Width;            // Eye width
    float   Slope_Top;        // Top edge slope (-1.0 to 1.0)
    float   Slope_Bottom;     // Bottom edge slope (-1.0 to 1.0)
    int16_t Radius_Top;       // Top corner radius
    int16_t Radius_Bottom;    // Bottom corner radius
} EyeConfig;

/**
 * Preset eye configurations
 */

// Normal eye (fully open)
static const EyeConfig EyePreset_Normal = {
    .OffsetX = 0,
    .OffsetY = 0,
    .Height = 40,
    .Width = 40,
    .Slope_Top = 0,
    .Slope_Bottom = 0,
    .Radius_Top = 8,
    .Radius_Bottom = 8
};

// Half-closed eye
static const EyeConfig EyePreset_HalfClosed = {
    .OffsetX = 0,
    .OffsetY = 0,
    .Height = 25,
    .Width = 40,
    .Slope_Top = 0,
    .Slope_Bottom = 0,
    .Radius_Top = 6,
    .Radius_Bottom = 6
};

// Fully closed eye
static const EyeConfig EyePreset_Closed = {
    .OffsetX = 0,
    .OffsetY = 0,
    .Height = 5,
    .Width = 40,
    .Slope_Top = 0,
    .Slope_Bottom = 0,
    .Radius_Top = 2,
    .Radius_Bottom = 2
};

/**
 * Eye layout constants for 128x64 display
 */
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define EYE_DISTANCE  44      // Distance between eye centers
#define LEFT_EYE_X    42      // Left eye center X
#define RIGHT_EYE_X   86      // Right eye center X
#define EYE_Y         32      // Eye center Y

#ifdef __cplusplus
}
#endif

#endif // DYNAMIC_EYE_H
