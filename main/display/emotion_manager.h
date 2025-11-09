// Emotion Manager - Maps device states to emotions
// Manages automatic emotion display based on system state

#pragma once

#include "emotion_bitmaps.h"

#ifdef __cplusplus
extern "C" {
#include "../device_state.h"
#else
// Forward declaration for C files - define DeviceState as int
typedef int DeviceState;
#endif

// Get the appropriate emotion for a device state
Emotion get_emotion_for_state(DeviceState state);

// Get emotion for pet action (optional, for future use)
Emotion get_emotion_for_pet_action(int action_id);

#ifdef __cplusplus
}
#endif
