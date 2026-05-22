#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <lvgl.h>

#include "tarkov_img_data.h"

static const lv_img_dsc_t tarkov_img_dsc = {
    .header = {
        .cf = LV_IMG_CF_TRUE_COLOR,
        .always_zero = 0,
        .reserved = 0,
        .w = TARKOV_IMG_W,
        .h = TARKOV_IMG_H,
    },
    .data_size = TARKOV_IMG_W * TARKOV_IMG_H * 2,
    .data = tarkov_img_data,
};

static int tarkov_widget_init(void)
{
    lv_obj_t *screen = lv_scr_act();
    if (!screen) {
        return 0;
    }

    lv_obj_t *img = lv_img_create(screen);
    lv_img_set_src(img, &tarkov_img_dsc);
    lv_obj_set_pos(img, 0, 0);
    /* Place behind all other children so ZMK widgets remain visible */
    lv_obj_move_to_index(img, 0);

    return 0;
}

/* Run after ZMK display (APPLICATION + 1) so lv_scr_act() is populated */
SYS_INIT(tarkov_widget_init, APPLICATION, CONFIG_IRONHAND_TARKOV_DISPLAY_INIT_PRIORITY);
