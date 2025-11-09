// Auto-generated emotion bitmaps header
// DO NOT EDIT - Generated from parse_emotions.py

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Emotion types
typedef enum {
    // Basic emotions (static)
    EMOJI_HAPPY,
    EMOJI_SQUINT,
    EMOJI_EXCITED,
    EMOJI_LOVE,
    EMOJI_SPEAKING,
    EMOJI_THINKING,
    EMOJI_SAD,
    EMOJI_SONFUSED,

    // Idle animation emotions - Normal series
    EMOJI_IDLE_NORMAL_OPEN,
    EMOJI_IDLE_NORMAL_HALF,
    EMOJI_IDLE_NORMAL_CLOSED,

    // Idle animation emotions - Sleepy series
    EMOJI_IDLE_SLEEPY_OPEN,
    EMOJI_IDLE_SLEEPY_HALF,
    EMOJI_IDLE_SLEEPY_CLOSED,

    // Idle animation emotions - Bored series
    EMOJI_IDLE_BORED_OPEN,
    EMOJI_IDLE_BORED_HALF,
    EMOJI_IDLE_BORED_CLOSED,

    EMOTION_COUNT = 17
} Emotion;

// Emotion metadata
typedef struct {
    const char* name;
    const uint8_t* data;
    uint16_t width;
    uint16_t height;
    uint16_t size;
} EmotionInfo;

// Get emotion bitmap data
const EmotionInfo* get_emotion_info(Emotion emotion);

// Get emotion by name
Emotion get_emotion_by_name(const char* name);

#ifdef __cplusplus
}
#endif
