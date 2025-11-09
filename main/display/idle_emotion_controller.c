// Idle Emotion Controller Implementation - Parametric Blinking

#include "idle_emotion_controller.h"
#include "dynamic_eye_drawer.h"
#include <esp_log.h>
#include <esp_random.h>

static const char* TAG = "IdleController";

// Timing constants
#define BLINK_INTERVAL_MIN_MS 3000   // Minimum time between blinks (3 seconds)
#define BLINK_INTERVAL_MAX_MS 5000   // Maximum time between blinks (5 seconds)
#define BLINK_FRAME_DURATION_MS 80   // Duration of each blink frame (80ms)
#define BLINK_TOTAL_FRAMES 5         // Total frames in blink animation

// Blink animation sequence configurations
static const EyeConfig* blink_sequence[BLINK_TOTAL_FRAMES] = {
    &EyePreset_Normal,      // Frame 0: Eyes open
    &EyePreset_HalfClosed,  // Frame 1: Eyes half-closed
    &EyePreset_Closed,      // Frame 2: Eyes fully closed
    &EyePreset_HalfClosed,  // Frame 3: Eyes half-closed
    &EyePreset_Normal       // Frame 4: Eyes open
};

// Get random blink interval
static uint32_t get_random_blink_interval() {
    uint32_t range = BLINK_INTERVAL_MAX_MS - BLINK_INTERVAL_MIN_MS;
    uint32_t random_offset = esp_random() % (range + 1);
    return BLINK_INTERVAL_MIN_MS + random_offset;
}

// Forward declarations
static void schedule_next_blink(IdleEmotionController* ctrl);

// Blink frame timer callback - advances blink animation
static void frame_timer_callback(void* arg) {
    IdleEmotionController* ctrl = (IdleEmotionController*)arg;

    if (!ctrl->is_blinking) {
        return;
    }

    // Display current frame
    const EyeConfig* config = blink_sequence[ctrl->current_frame];
    dynamic_eye_draw_both(ctrl->u8g2, config);

    ESP_LOGD(TAG, "Blink frame %d/5", ctrl->current_frame + 1);

    // Advance to next frame
    ctrl->current_frame++;

    // Check if blink sequence is complete
    if (ctrl->current_frame >= BLINK_TOTAL_FRAMES) {
        esp_timer_stop(ctrl->frame_timer);  // Stop frame timer
        ctrl->is_blinking = false;
        ctrl->current_frame = 0;
        ESP_LOGD(TAG, "Blink completed");
        return;
    }
}

// Blink timer callback - triggers a new blink animation
static void blink_timer_callback(void* arg) {
    IdleEmotionController* ctrl = (IdleEmotionController*)arg;

    if (!ctrl->is_active || ctrl->is_blinking) {
        return;
    }

    ESP_LOGD(TAG, "Triggering blink animation");

    // Start blink animation
    ctrl->is_blinking = true;
    ctrl->current_frame = 0;

    // Display first frame immediately
    dynamic_eye_draw_both(ctrl->u8g2, blink_sequence[0]);
    ctrl->current_frame = 1;

    // Start frame timer for remaining frames
    esp_err_t ret = esp_timer_start_periodic(ctrl->frame_timer, BLINK_FRAME_DURATION_MS * 1000);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start frame timer: %s", esp_err_to_name(ret));
        ctrl->is_blinking = false;
        return;
    }

    // Schedule next blink
    schedule_next_blink(ctrl);
}

// Schedule next blink with random interval
static void schedule_next_blink(IdleEmotionController* ctrl) {
    if (!ctrl->is_active) {
        return;
    }

    uint32_t interval_ms = get_random_blink_interval();
    ESP_LOGD(TAG, "Next blink in %lu ms", interval_ms);

    esp_timer_stop(ctrl->blink_timer);
    esp_timer_start_once(ctrl->blink_timer, interval_ms * 1000);
}

void idle_controller_init(IdleEmotionController* ctrl, u8g2_t* u8g2) {
    ctrl->u8g2 = u8g2;
    ctrl->is_active = false;
    ctrl->is_blinking = false;
    ctrl->current_frame = 0;

    // Create frame timer (for blink animation)
    esp_timer_create_args_t frame_timer_args = {
        .callback = frame_timer_callback,
        .arg = ctrl,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "idle_frame_timer",
        .skip_unhandled_events = false
    };
    esp_err_t ret = esp_timer_create(&frame_timer_args, &ctrl->frame_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create frame timer: %s", esp_err_to_name(ret));
    }

    // Create blink timer (for random blink triggering)
    esp_timer_create_args_t blink_timer_args = {
        .callback = blink_timer_callback,
        .arg = ctrl,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "idle_blink_timer",
        .skip_unhandled_events = false
    };
    ret = esp_timer_create(&blink_timer_args, &ctrl->blink_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create blink timer: %s", esp_err_to_name(ret));
    }

    ESP_LOGI(TAG, "Idle controller initialized with parametric eyes");
}

void idle_controller_start(IdleEmotionController* ctrl) {
    if (ctrl->is_active) {
        ESP_LOGW(TAG, "Idle controller already active");
        return;
    }

    ESP_LOGI(TAG, "Starting parametric idle animation");

    ctrl->is_active = true;
    ctrl->is_blinking = false;
    ctrl->current_frame = 0;

    // Display initial eyes (open)
    dynamic_eye_draw_both(ctrl->u8g2, &EyePreset_Normal);

    // Schedule first blink
    schedule_next_blink(ctrl);
}

void idle_controller_stop(IdleEmotionController* ctrl) {
    if (!ctrl->is_active) {
        return;
    }

    ESP_LOGI(TAG, "Stopping parametric idle animation");

    ctrl->is_active = false;
    ctrl->is_blinking = false;

    // Stop all timers
    esp_timer_stop(ctrl->blink_timer);
    esp_timer_stop(ctrl->frame_timer);

    // Clear display
    dynamic_eye_clear(ctrl->u8g2);
}

void idle_controller_deinit(IdleEmotionController* ctrl) {
    idle_controller_stop(ctrl);

    if (ctrl->blink_timer != NULL) {
        esp_timer_delete(ctrl->blink_timer);
        ctrl->blink_timer = NULL;
    }

    if (ctrl->frame_timer != NULL) {
        esp_timer_delete(ctrl->frame_timer);
        ctrl->frame_timer = NULL;
    }

    ESP_LOGI(TAG, "Idle controller deinitialized");
}
