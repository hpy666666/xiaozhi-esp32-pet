// Emotion Manager Implementation
#include "emotion_manager.h"
#include "../device_state.h"  // Need the actual enum values
#include <esp_log.h>

static const char* TAG = "EmotionManager";

// Map device states to appropriate emotions
Emotion get_emotion_for_state(DeviceState state) {
    switch (state) {
        case kDeviceStateIdle:  // 8
            return EMOJI_HAPPY;  // Idle: happy/content face

        case kDeviceStateListening:  // 10
            return EMOJI_HAPPY;  // Listening: happy face (changed from EMOJI_EXCITED)

        case kDeviceStateSpeaking:  // 11
            return EMOJI_SPEAKING;  // Speaking: mouth open

        case kDeviceStateConnecting:  // 9
        case kDeviceStateActivating:  // 13
            return EMOJI_THINKING;  // Processing: thinking face

        case kDeviceStateStarting:  // 6
            return EMOJI_SQUINT;  // Starting: waking up

        case kDeviceStateWifiConfiguring:  // 7
            return EMOJI_SONFUSED;  // Configuring: confused/waiting

        case kDeviceStateUpgrading:  // 12
            return EMOJI_THINKING;  // Upgrading: processing

        case kDeviceStateFatalError:  // 15
            return EMOJI_SAD;  // Error: sad face

        case kDeviceStateAudioTesting:  // 14
            return EMOJI_LOVE;  // Testing: playful

        case kDeviceStateUnknown:  // 5
        default:
            return EMOJI_HAPPY;  // Default: neutral/happy
    }
}

// Map pet actions to emotions (for future expansion)
Emotion get_emotion_for_pet_action(int action_id) {
    // Action IDs from pet_actions.h:
    // 0=stand, 1=lie, 2=sit, 3=wave, 4=forward, 5=backward, etc.

    switch (action_id) {
        case 3:  // Wave/Hello
            return EMOJI_EXCITED;

        case 1:  // Lie down
            return EMOJI_SQUINT;  // Sleepy

        case 4:  // Forward
        case 5:  // Backward
        case 6:  // Turn left
        case 7:  // Turn right
            return EMOJI_HAPPY;  // Moving: happy

        case 8:  // Shake head
            return EMOJI_SONFUSED;  // Confused

        case 9:  // Nod
            return EMOJI_LOVE;  // Agreeing

        default:
            return EMOJI_HAPPY;
    }
}
