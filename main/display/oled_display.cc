#include "oled_display.h"
#include "assets/lang_config.h"
#include "lazy_blink_gif.h"  // GIF animation for idle state
// #include "dynamic_eye_drawer.h"  // Not compatible with xiaozhi-pet

#include <string>
#include <algorithm>

#include <esp_log.h>
#include <esp_err.h>
#include <esp_lvgl_port.h>
#include <font_awesome.h>

#define TAG "OledDisplay"

LV_FONT_DECLARE(font_awesome_30_1);

OledDisplay::OledDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
    int width, int height, bool mirror_x, bool mirror_y, DisplayFonts fonts)
    : panel_io_(panel_io), panel_(panel), fonts_(fonts) {
    width_ = width;
    height_ = height;

    ESP_LOGI(TAG, "Initialize LVGL");
    lvgl_port_cfg_t port_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    port_cfg.task_priority = 1;
    port_cfg.task_stack = 6144;
#if CONFIG_SOC_CPU_CORES_NUM > 1
    port_cfg.task_affinity = 1;
#endif
    lvgl_port_init(&port_cfg);

    ESP_LOGI(TAG, "Adding OLED display");
    const lvgl_port_display_cfg_t display_cfg = {
        .io_handle = panel_io_,
        .panel_handle = panel_,
        .control_handle = nullptr,
        .buffer_size = static_cast<uint32_t>(width_ * height_),
        .double_buffer = false,
        .trans_size = 0,
        .hres = static_cast<uint32_t>(width_),
        .vres = static_cast<uint32_t>(height_),
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = mirror_x,
            .mirror_y = mirror_y,
        },
        .flags = {
            .buff_dma = 1,
            .buff_spiram = 0,
            .sw_rotate = 0,
            .full_refresh = 0,
            .direct_mode = 0,
        },
    };

    display_ = lvgl_port_add_disp(&display_cfg);
    if (display_ == nullptr) {
        ESP_LOGE(TAG, "Failed to add display");
        return;
    }

    if (height_ == 64) {
        SetupUI_128x64();
    } else {
        SetupUI_128x32();
    }

    /* Idle animation initialization disabled - requires u8g2 library
    // Initialize U8g2 for parametric eye drawing (Idle state only)
    // Note: This shares the same I2C bus as panel_io_
    u8g2_instance_ = dynamic_eye_drawer_init(0, -1, -1);  // Use existing I2C config

    // Initialize idle emotion controller with U8g2
    idle_controller_init(&idle_controller_, u8g2_instance_);
    ESP_LOGI(TAG, "Idle emotion controller initialized with parametric eyes");
    */
}

OledDisplay::~OledDisplay() {
    /* Idle animation cleanup disabled - requires idle_controller
    // Clean up idle emotion controller
    idle_controller_deinit(&idle_controller_);
    */

    // Clean up emotion canvas
    if (emotion_canvas_ != nullptr) {
        lv_obj_del(emotion_canvas_);
    }

    // Clean up GIF object
    if (gif_img_ != nullptr) {
        lv_obj_del(gif_img_);
    }

    if (content_ != nullptr) {
        lv_obj_del(content_);
    }
    if (status_bar_ != nullptr) {
        lv_obj_del(status_bar_);
    }
    if (side_bar_ != nullptr) {
        lv_obj_del(side_bar_);
    }
    if (container_ != nullptr) {
        lv_obj_del(container_);
    }

    if (panel_ != nullptr) {
        esp_lcd_panel_del(panel_);
    }
    if (panel_io_ != nullptr) {
        esp_lcd_panel_io_del(panel_io_);
    }
    lvgl_port_deinit();
}

bool OledDisplay::Lock(int timeout_ms) {
    return lvgl_port_lock(timeout_ms);
}

void OledDisplay::Unlock() {
    lvgl_port_unlock();
}

void OledDisplay::SetChatMessage(const char* role, const char* content) {
    DisplayLockGuard lock(this);
    if (chat_message_label_ == nullptr) {
        // Chat message display is disabled in simplified UI
        return;
    }

    // Replace all newlines with spaces
    std::string content_str = content;
    std::replace(content_str.begin(), content_str.end(), '\n', ' ');

    if (content_right_ == nullptr) {
        lv_label_set_text(chat_message_label_, content_str.c_str());
    } else {
        if (content == nullptr || content[0] == '\0') {
            lv_obj_add_flag(content_right_, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_label_set_text(chat_message_label_, content_str.c_str());
            lv_obj_remove_flag(content_right_, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void OledDisplay::SetupUI_128x64() {
    DisplayLockGuard lock(this);

    auto screen = lv_screen_active();
    lv_obj_set_style_text_font(screen, fonts_.text_font, 0);
    // Black background with blue text theme
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_obj_set_style_text_color(screen, lv_color_white(), 0);  // White = blue on monochrome OLED

    /* Container */
    container_ = lv_obj_create(screen);
    lv_obj_set_size(container_, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_flex_flow(container_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(container_, 0, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_pad_row(container_, 0, 0);
    lv_obj_set_style_bg_color(container_, lv_color_black(), 0);  // Black background

    /* Status bar - simplified, only WiFi and time */
    status_bar_ = lv_obj_create(container_);
    lv_obj_set_size(status_bar_, LV_HOR_RES, 16);
    lv_obj_set_style_border_width(status_bar_, 0, 0);
    lv_obj_set_style_pad_all(status_bar_, 0, 0);
    lv_obj_set_style_radius(status_bar_, 0, 0);
    lv_obj_set_style_bg_color(status_bar_, lv_color_black(), 0);  // Black background
    lv_obj_set_flex_flow(status_bar_, LV_FLEX_FLOW_ROW);

    // WiFi icon on the left
    network_label_ = lv_label_create(status_bar_);
    lv_label_set_text(network_label_, "");
    lv_obj_set_style_text_font(network_label_, fonts_.icon_font, 0);
    lv_obj_set_style_text_color(network_label_, lv_color_white(), 0);  // Blue text

    // Spacer to push time to the right
    lv_obj_t* spacer = lv_obj_create(status_bar_);
    lv_obj_set_flex_grow(spacer, 1);
    lv_obj_set_style_bg_opa(spacer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(spacer, 0, 0);

    // Time on the right (reuse status_label_ for time display)
    status_label_ = lv_label_create(status_bar_);
    lv_label_set_text(status_label_, "00:00");
    lv_obj_set_style_text_color(status_label_, lv_color_white(), 0);  // Blue text

    /* Content area - full space for emotion display (48 pixels high) */
    content_ = lv_obj_create(container_);
    lv_obj_set_scrollbar_mode(content_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(content_, 0, 0);
    lv_obj_set_style_pad_all(content_, 0, 0);
    lv_obj_set_style_border_width(content_, 0, 0);
    lv_obj_set_width(content_, LV_HOR_RES);
    lv_obj_set_flex_grow(content_, 1);
    lv_obj_set_style_bg_color(content_, lv_color_black(), 0);  // Black background

    // Create emotion label (will be hidden when showing bitmap emotion)
    content_left_ = content_;  // Reuse content_ as content_left_ for compatibility
    emotion_label_ = lv_label_create(content_);
    lv_obj_set_style_text_font(emotion_label_, &font_awesome_30_1, 0);
    lv_label_set_text(emotion_label_, FONT_AWESOME_MICROCHIP_AI);
    lv_obj_center(emotion_label_);
    lv_obj_set_style_text_color(emotion_label_, lv_color_white(), 0);  // Blue icon

    // Hide unused UI elements (set to null to avoid crashes)
    notification_label_ = nullptr;
    mute_label_ = nullptr;
    battery_label_ = nullptr;
    content_right_ = nullptr;
    chat_message_label_ = nullptr;
    side_bar_ = nullptr;

    // Low battery popup (keep for safety warnings)
    low_battery_popup_ = lv_obj_create(screen);
    lv_obj_set_scrollbar_mode(low_battery_popup_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(low_battery_popup_, LV_HOR_RES * 0.9, fonts_.text_font->line_height * 2);
    lv_obj_align(low_battery_popup_, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(low_battery_popup_, lv_color_black(), 0);
    lv_obj_set_style_radius(low_battery_popup_, 10, 0);
    low_battery_label_ = lv_label_create(low_battery_popup_);
    lv_label_set_text(low_battery_label_, Lang::Strings::BATTERY_NEED_CHARGE);
    lv_obj_set_style_text_color(low_battery_label_, lv_color_white(), 0);
    lv_obj_center(low_battery_label_);
    lv_obj_add_flag(low_battery_popup_, LV_OBJ_FLAG_HIDDEN);
}

void OledDisplay::SetupUI_128x32() {
    DisplayLockGuard lock(this);

    auto screen = lv_screen_active();
    lv_obj_set_style_text_font(screen, fonts_.text_font, 0);

    /* Container */
    container_ = lv_obj_create(screen);
    lv_obj_set_size(container_, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_flex_flow(container_, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(container_, 0, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_pad_column(container_, 0, 0);

    /* Emotion label on the left side */
    content_ = lv_obj_create(container_);
    lv_obj_set_size(content_, 32, 32);
    lv_obj_set_style_pad_all(content_, 0, 0);
    lv_obj_set_style_border_width(content_, 0, 0);
    lv_obj_set_style_radius(content_, 0, 0);

    emotion_label_ = lv_label_create(content_);
    lv_obj_set_style_text_font(emotion_label_, &font_awesome_30_1, 0);
    lv_label_set_text(emotion_label_, FONT_AWESOME_MICROCHIP_AI);
    lv_obj_center(emotion_label_);

    /* Right side */
    side_bar_ = lv_obj_create(container_);
    lv_obj_set_size(side_bar_, width_ - 32, 32);
    lv_obj_set_flex_flow(side_bar_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(side_bar_, 0, 0);
    lv_obj_set_style_border_width(side_bar_, 0, 0);
    lv_obj_set_style_radius(side_bar_, 0, 0);
    lv_obj_set_style_pad_row(side_bar_, 0, 0);

    /* Status bar */
    status_bar_ = lv_obj_create(side_bar_);
    lv_obj_set_size(status_bar_, width_ - 32, 16);
    lv_obj_set_style_radius(status_bar_, 0, 0);
    lv_obj_set_flex_flow(status_bar_, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(status_bar_, 0, 0);
    lv_obj_set_style_border_width(status_bar_, 0, 0);
    lv_obj_set_style_pad_column(status_bar_, 0, 0);

    status_label_ = lv_label_create(status_bar_);
    lv_obj_set_flex_grow(status_label_, 1);
    lv_obj_set_style_pad_left(status_label_, 2, 0);
    lv_label_set_text(status_label_, Lang::Strings::INITIALIZING);

    notification_label_ = lv_label_create(status_bar_);
    lv_obj_set_flex_grow(notification_label_, 1);
    lv_obj_set_style_pad_left(notification_label_, 2, 0);
    lv_label_set_text(notification_label_, "");
    lv_obj_add_flag(notification_label_, LV_OBJ_FLAG_HIDDEN);

    mute_label_ = lv_label_create(status_bar_);
    lv_label_set_text(mute_label_, "");
    lv_obj_set_style_text_font(mute_label_, fonts_.icon_font, 0);

    network_label_ = lv_label_create(status_bar_);
    lv_label_set_text(network_label_, "");
    lv_obj_set_style_text_font(network_label_, fonts_.icon_font, 0);

    battery_label_ = lv_label_create(status_bar_);
    lv_label_set_text(battery_label_, "");
    lv_obj_set_style_text_font(battery_label_, fonts_.icon_font, 0);

    chat_message_label_ = lv_label_create(side_bar_);
    lv_obj_set_size(chat_message_label_, width_ - 32, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_left(chat_message_label_, 2, 0);
    lv_label_set_long_mode(chat_message_label_, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(chat_message_label_, "");

    // 延迟一定的时间后开始滚动字幕
    static lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_delay(&a, 1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_obj_set_style_anim(chat_message_label_, &a, LV_PART_MAIN);
    lv_obj_set_style_anim_duration(chat_message_label_, lv_anim_speed_clamped(60, 300, 60000), LV_PART_MAIN);
}

// Emotion display functions
void OledDisplay::SetEmotion(Emotion emotion) {
    DisplayLockGuard lock(this);

    const EmotionInfo* info = get_emotion_info(emotion);
    if (info == nullptr) {
        ESP_LOGE(TAG, "Invalid emotion: %d", emotion);
        return;
    }

    ESP_LOGI(TAG, "Setting emotion: %s (%dx%d)", info->name, info->width, info->height);

    // Hide emotion_label_ (the font-awesome icon) but keep UI visible
    if (emotion_label_ != nullptr) {
        lv_obj_add_flag(emotion_label_, LV_OBJ_FLAG_HIDDEN);
    }

    // Create canvas if not exists
    if (emotion_canvas_ == nullptr) {
        emotion_canvas_ = lv_canvas_create(content_left_);  // Put canvas in content area

        // Allocate buffer for canvas (128x48 monochrome to fill content area)
        static uint8_t canvas_buf[128 * 48 / 8];  // 768 bytes
        lv_canvas_set_buffer(emotion_canvas_, canvas_buf, 128, 48, LV_COLOR_FORMAT_I1);

        // Set palette for I1 format: index 0 = black, index 1 = white (blue on OLED)
        lv_color32_t black = lv_color_to_32(lv_color_hex(0x000000), 0xFF);
        lv_color32_t white = lv_color_to_32(lv_color_hex(0xFFFFFF), 0xFF);
        lv_canvas_set_palette(emotion_canvas_, 0, black);
        lv_canvas_set_palette(emotion_canvas_, 1, white);

        // Position canvas to fill content area
        lv_obj_set_size(emotion_canvas_, 128, 48);
        lv_obj_align(emotion_canvas_, LV_ALIGN_CENTER, 0, 0);
    } else {
        lv_obj_remove_flag(emotion_canvas_, LV_OBJ_FLAG_HIDDEN);
    }

    // Clear canvas with black background (index 0)
    lv_canvas_fill_bg(emotion_canvas_, lv_color_hex(0x000000), LV_OPA_COVER);

    ESP_LOGI(TAG, "Drawing emotion bitmap to canvas (scaling %dx%d to 128x48)...", info->width, info->height);

    // Draw emotion bitmap pixel by pixel with scaling
    // Scale 128x64 emotion to 128x48 canvas (keep X, scale Y to 3/4)
    // image2cpp format: bitmap 1 = emotion lines (should be blue), bitmap 0 = background (black)
    for (uint16_t canvas_y = 0; canvas_y < 48; canvas_y++) {
        for (uint16_t canvas_x = 0; canvas_x < 128; canvas_x++) {
            // Map canvas coordinates to original bitmap coordinates
            uint16_t src_x = (canvas_x * info->width) / 128;  // Scale X
            uint16_t src_y = (canvas_y * info->height) / 48;  // Scale Y to 3/4

            // Get pixel from horizontal bitmap
            uint16_t src_byte_index = (src_y * ((info->width + 7) / 8)) + (src_x / 8);
            uint8_t src_bit_index = 7 - (src_x % 8);

            if (src_byte_index < info->size) {
                uint8_t pixel = (info->data[src_byte_index] >> src_bit_index) & 0x01;

                // If pixel is 1 in bitmap, draw it as white (which displays as blue on OLED)
                if (pixel) {
                    lv_canvas_set_px(emotion_canvas_, canvas_x, canvas_y, lv_color_hex(0xFFFFFF), LV_OPA_COVER);
                }
            }
        }
    }

    emotion_mode_active_ = true;
    ESP_LOGI(TAG, "Emotion displayed successfully on canvas");
}

void OledDisplay::SetEmotionForState(DeviceState state) {
    Emotion emotion = get_emotion_for_state(state);
    SetEmotion(emotion);
}

void OledDisplay::ClearEmotion() {
    DisplayLockGuard lock(this);

    // Hide emotion canvas
    if (emotion_canvas_ != nullptr) {
        lv_obj_add_flag(emotion_canvas_, LV_OBJ_FLAG_HIDDEN);
    }

    // Show emotion_label_ (font-awesome icon) again
    if (emotion_label_ != nullptr) {
        lv_obj_remove_flag(emotion_label_, LV_OBJ_FLAG_HIDDEN);
    }

    emotion_mode_active_ = false;
    ESP_LOGI(TAG, "Emotion cleared");
}

// Idle animation control (disabled - not compatible with xiaozhi-pet)
/*
void OledDisplay::StartIdleAnimation() {
    ESP_LOGI(TAG, "Starting idle animation");
    idle_controller_start(&idle_controller_);
}

void OledDisplay::StopIdleAnimation() {
    ESP_LOGI(TAG, "Stopping idle animation");
    idle_controller_stop(&idle_controller_);
    // Don't call ClearEmotion() here - let the next SetEmotion() handle the display
    // ClearEmotion();  // This causes black screen flash
}
*/

// GIF animation functions
void OledDisplay::ShowIdleGif() {
    DisplayLockGuard lock(this);

    ESP_LOGI(TAG, "Showing idle GIF animation");

    // Hide all UI elements
    if (container_ != nullptr) {
        lv_obj_add_flag(container_, LV_OBJ_FLAG_HIDDEN);
    }
    if (side_bar_ != nullptr) {
        lv_obj_add_flag(side_bar_, LV_OBJ_FLAG_HIDDEN);
    }

    // Create GIF image object if not exists
    if (gif_img_ == nullptr) {
        auto screen = lv_screen_active();
        gif_img_ = lv_gif_create(screen);
        lv_obj_set_size(gif_img_, 128, 64);
        lv_obj_align(gif_img_, LV_ALIGN_CENTER, 0, 0);
        lv_gif_set_src(gif_img_, &lazy_blink_gif);
    } else {
        // Show existing GIF
        lv_obj_remove_flag(gif_img_, LV_OBJ_FLAG_HIDDEN);
        // Restart GIF from beginning
        lv_gif_restart(gif_img_);
    }

    gif_mode_active_ = true;
    emotion_mode_active_ = false;  // Clear emotion mode if active
    ESP_LOGI(TAG, "Idle GIF animation started");
}

void OledDisplay::HideIdleGif() {
    DisplayLockGuard lock(this);

    ESP_LOGI(TAG, "Hiding idle GIF animation");

    // Hide GIF image
    if (gif_img_ != nullptr) {
        lv_obj_add_flag(gif_img_, LV_OBJ_FLAG_HIDDEN);
    }

    // Show normal UI elements
    if (container_ != nullptr) {
        lv_obj_remove_flag(container_, LV_OBJ_FLAG_HIDDEN);
    }
    if (side_bar_ != nullptr) {
        lv_obj_remove_flag(side_bar_, LV_OBJ_FLAG_HIDDEN);
    }

    gif_mode_active_ = false;
    ESP_LOGI(TAG, "Idle GIF animation hidden");
}

