#pragma once

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Lazy blink GIF animation for idle state
// 128x64 pixels, 43 frames @ 12fps, 3.6 seconds loop
extern const lv_img_dsc_t lazy_blink_gif;

#ifdef __cplusplus
}
#endif
