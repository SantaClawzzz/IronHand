#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <lvgl.h>

#include "tarkov_img_data.h"

static const lv_img_dsc_t tarkov_img_dsc = {
    .header = {
        .cf          = LV_IMG_CF_TRUE_COLOR,
        .always_zero = 0,
        .reserved    = 0,
        .w           = TARKOV_IMG_W,
        .h           = TARKOV_IMG_H,
    },
    .data_size = TARKOV_IMG_W * TARKOV_IMG_H * 2,
    .data      = tarkov_img_data,
};

static lv_style_t tarkov_bg_style;

static int tarkov_widget_init(void)
{
    lv_obj_t *screen = lv_scr_act();
    if (!screen) {
        return 0;
    }

    /*
     * Set the Tarkov image as the screen background via LVGL styles.
     * lv_style_set_bg_img_src uses the core draw engine (lv_draw_img),
     * not the lv_img widget, so no LV_USE_IMG Kconfig needed.
     */
    lv_style_init(&tarkov_bg_style);
    lv_style_set_bg_img_src(&tarkov_bg_style, &tarkov_img_dsc);
    lv_obj_add_style(screen, &tarkov_bg_style, 0);

    return 0;
}

SYS_INIT(tarkov_widget_init, APPLICATION, CONFIG_IRONHAND_TARKOV_DISPLAY_INIT_PRIORITY);
