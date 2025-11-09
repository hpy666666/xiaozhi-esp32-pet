#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "display/oled_display.h"
#include "display/esplog_display.h"
#include "pet/pet_controller.h"
#include "application.h"
#include "button.h"
#include "config.h"

#include <esp_log.h>
#include <driver/i2c_master.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_sh1106.h>
#include <wifi_station.h>
#include <functional>

#define TAG "XiaozhiPetBoard"

LV_FONT_DECLARE(font_puhui_14_1);
LV_FONT_DECLARE(font_awesome_14_1);

class XiaozhiPetBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t i2c_bus_;
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;
    Button boot_button_;
    Display* display_ = nullptr;
    bool display_init_attempted_ = false;

    void InitializeI2c() {
        ESP_LOGI(TAG, "Initializing I2C for OLED");
        i2c_master_bus_config_t i2c_bus_cfg = {
            .i2c_port = (i2c_port_t)0,
            .sda_io_num = DISPLAY_I2C_SDA_PIN,
            .scl_io_num = DISPLAY_I2C_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        esp_err_t ret = i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus_);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create I2C master bus: %s", esp_err_to_name(ret));
            return;
        }
    }

    void InitializeDisplay() {
        ESP_LOGI(TAG, "Initializing SH1106 OLED display");

        // SH1106 I2C IO config (using default config from driver)
        esp_lcd_panel_io_i2c_config_t io_config = {
            .dev_addr = 0x3C,
            .on_color_trans_done = nullptr,
            .user_ctx = nullptr,
            .control_phase_bytes = 1,
            .dc_bit_offset = 6,
            .lcd_cmd_bits = 8,
            .lcd_param_bits = 8,
            .flags = {
                .dc_low_on_data = 0,
                .disable_control_phase = 0,
            },
            .scl_speed_hz = DISPLAY_I2C_FREQ_HZ,
        };

        esp_err_t ret = esp_lcd_new_panel_io_i2c_v2(i2c_bus_, &io_config, &panel_io_);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create panel IO: %s", esp_err_to_name(ret));
            return;
        }

        // SH1106 panel config (128x64 monochrome)
        esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = -1,
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
            .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
            .bits_per_pixel = 1,  // Monochrome
            .flags = {
                .reset_active_high = 0,
            },
            .vendor_config = nullptr,
        };

        // Use native SH1106 driver
        ret = esp_lcd_new_panel_sh1106(panel_io_, &panel_config, &panel_);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create SH1106 panel: %s", esp_err_to_name(ret));
            return;
        }

        ret = esp_lcd_panel_reset(panel_);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to reset panel: %s", esp_err_to_name(ret));
            return;
        }

        ret = esp_lcd_panel_init(panel_);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize panel: %s", esp_err_to_name(ret));
            return;
        }

        // SH1106 driver handles column offset (2) internally in draw_bitmap
        // No need to call set_gap manually

        // Fix display orientation: mirror both X and Y axis
        esp_lcd_panel_mirror(panel_, true, true);

        // Keep invert_color as false - let bitmap data control the display
        esp_lcd_panel_invert_color(panel_, false);
        esp_lcd_panel_disp_on_off(panel_, true);

        // Create 128x64 LVGL monochrome display wrapper
        // mirror_x=true, mirror_y=true to fix upside-down and mirrored text
        display_ = new OledDisplay(panel_io_, panel_, 128, 64, true, true,
            {&font_puhui_14_1, &font_awesome_14_1});
        ESP_LOGI(TAG, "SH1106 OLED display initialized successfully (128x64)");
    }

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            }
            app.ToggleChatState();
        });

        // 长按按钮执行打招呼动作（演示宠物功能）
        boot_button_.OnLongPress([this]() {
            ESP_LOGI(TAG, "Long press detected, pet says hello!");
            PetController::GetInstance().PerformAction(ACTION_HELLO);
        });
    }

    void InitializePet() {
        ESP_LOGI(TAG, "Initializing pet controller");
        PetController::GetInstance().Init();
        PetController::GetInstance().RegisterMcpTools();
        ESP_LOGI(TAG, "Pet controller initialized and MCP tools registered");

        // NOTE: Emotion test code disabled - emotion system now integrated into Application state machine
        // To re-enable testing, uncomment the code below:
        /*
        // Test emotion display after 3 seconds
        // Demo both state-based auto emotion and manual emotion control
        auto test_emotion = [this]() {
            vTaskDelay(pdMS_TO_TICKS(3000));

            if (display_ != nullptr) {
                OledDisplay* oled = static_cast<OledDisplay*>(display_);

                ESP_LOGI(TAG, "=== Testing state-based emotion display ===");

                // Test 1: State-based emotions
                DeviceState states[] = {
                    kDeviceStateIdle,
                    kDeviceStateListening,
                    kDeviceStateSpeaking,
                    kDeviceStateConnecting,
                    kDeviceStateStarting,
                    kDeviceStateFatalError
                };

                const char* state_names[] = {
                    "Idle",
                    "Listening",
                    "Speaking",
                    "Connecting",
                    "Starting",
                    "Error"
                };

                for (int i = 0; i < 6; i++) {
                    ESP_LOGI(TAG, "State %d/6: %s", i+1, state_names[i]);
                    oled->SetEmotionForState(states[i]);
                    vTaskDelay(pdMS_TO_TICKS(3000));
                }

                ESP_LOGI(TAG, "State-based test complete, clearing emotion");
                oled->ClearEmotion();
                vTaskDelay(pdMS_TO_TICKS(2000));

                ESP_LOGI(TAG, "=== Testing manual emotion control ===");

                // Test 2: Manual emotion control (all 8 emotions)
                Emotion emotions[] = {
                    EMOJI_HAPPY, EMOJI_SQUINT, EMOJI_EXCITED, EMOJI_LOVE,
                    EMOJI_SPEAKING, EMOJI_THINKING, EMOJI_SAD, EMOJI_SONFUSED
                };

                const char* emotion_names[] = {
                    "HAPPY", "SQUINT", "EXCITED", "LOVE",
                    "SPEAKING", "THINKING", "SAD", "CONFUSED"
                };

                for (int i = 0; i < 8; i++) {
                    ESP_LOGI(TAG, "Manual emotion %d/8: %s", i+1, emotion_names[i]);
                    oled->SetEmotion(emotions[i]);
                    vTaskDelay(pdMS_TO_TICKS(2000));
                }

                ESP_LOGI(TAG, "=== All tests complete ===");
                oled->ClearEmotion();
            }
            vTaskDelete(NULL);
        };

        // Create test task
        xTaskCreate([](void* param) {
            auto func = static_cast<std::function<void()>*>(param);
            (*func)();
            delete func;
        }, "emotion_test", 4096, new std::function<void()>(test_emotion), 1, nullptr);
        */
    }

public:
    XiaozhiPetBoard() : boot_button_(static_cast<gpio_num_t>(BUTTON_BOOT_PIN)),
                        display_(nullptr),
                        display_init_attempted_(false) {
        ESP_LOGI(TAG, "Creating Xiaozhi Pet Board");
    }

    ~XiaozhiPetBoard() {
        if (display_) delete display_;
    }

    std::string GetBoardType() override {
        return BOARD_NAME;
    }

    Display* GetDisplay() override {
        if (!display_ && !display_init_attempted_) {
            display_init_attempted_ = true;
            InitializeI2c();
            InitializeDisplay();

            // 如果OLED初始化失败，使用EspLogDisplay作为后备
            if (!display_) {
                ESP_LOGW(TAG, "OLED not available, using EspLogDisplay fallback");
                display_ = new EspLogDisplay();
            }
        }
        return display_;
    }

    void StartNetwork() override {
        ESP_LOGI(TAG, "Starting WiFi network");
        InitializeButtons();

        // 初始化宠物控制系统
        InitializePet();

        WifiBoard::StartNetwork();
    }

    AudioCodec* GetAudioCodec() override {
        // 使用外部音频编解码器（MAX98357A + INMP441）
        // Simplex模式: 麦克风和扬声器使用不同的I2S接口
        static NoAudioCodecSimplex codec(
            AUDIO_INPUT_SAMPLE_RATE,   // 输入采样率 16000Hz
            16000,                      // 输出采样率 16000Hz
            static_cast<gpio_num_t>(AUDIO_OUTPUT_BCLK_PIN),  // 扬声器 BCLK
            static_cast<gpio_num_t>(AUDIO_OUTPUT_WS_PIN),    // 扬声器 WS
            static_cast<gpio_num_t>(AUDIO_OUTPUT_DOUT_PIN),  // 扬声器 DOUT
            static_cast<gpio_num_t>(AUDIO_INPUT_BCLK_PIN),   // 麦克风 SCK
            static_cast<gpio_num_t>(AUDIO_INPUT_WS_PIN),     // 麦克风 WS
            static_cast<gpio_num_t>(AUDIO_INPUT_DIN_PIN)     // 麦克风 DIN
        );
        return &codec;
    }
};

DECLARE_BOARD(XiaozhiPetBoard);
