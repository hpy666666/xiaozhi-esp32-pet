#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include "display.h"
#include "emotion_bitmaps.h"
#include "../device_state.h"
#include "emotion_manager.h"
// #include "idle_emotion_controller.h"  // Not compatible with xiaozhi-pet display system
// #include <u8g2.h>  // Not used - xiaozhi-pet uses esp_lcd

#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>

class OledDisplay : public Display {
private:
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;

    lv_obj_t* status_bar_ = nullptr;
    lv_obj_t* content_ = nullptr;
    lv_obj_t* content_left_ = nullptr;
    lv_obj_t* content_right_ = nullptr;
    lv_obj_t* container_ = nullptr;
    lv_obj_t* side_bar_ = nullptr;
    lv_obj_t* emotion_canvas_ = nullptr;  // Canvas for displaying emotion bitmaps
    lv_obj_t* gif_img_ = nullptr;  // GIF image object for idle animation

    bool emotion_mode_active_ = false;  // Track if emotion is currently displayed
    bool gif_mode_active_ = false;  // Track if GIF animation is currently displayed
    // IdleEmotionController idle_controller_;  // Idle animation controller (not compatible)
    // u8g2_t* u8g2_instance_ = nullptr;  // U8g2 for parametric eye drawing (not used)

    DisplayFonts fonts_;

    virtual bool Lock(int timeout_ms = 0) override;
    virtual void Unlock() override;

    void SetupUI_128x64();
    void SetupUI_128x32();

public:
    OledDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel, int width, int height, bool mirror_x, bool mirror_y,
                DisplayFonts fonts);
    ~OledDisplay();

    virtual void SetChatMessage(const char* role, const char* content) override;

    // Emotion display API
    void SetEmotion(Emotion emotion);
    void SetEmotionForState(DeviceState state);  // Auto-select emotion based on state
    void ClearEmotion();
    bool IsEmotionModeActive() const { return emotion_mode_active_; }

    // GIF animation API (for idle state)
    void ShowIdleGif();  // Show lazy blink GIF animation
    void HideIdleGif();  // Hide GIF animation
    bool IsGifModeActive() const { return gif_mode_active_; }

    // Idle animation control (not compatible with xiaozhi-pet display system)
    // void StartIdleAnimation();
    // void StopIdleAnimation();
};

#endif // OLED_DISPLAY_H
