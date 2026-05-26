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

static void set_children_transparent(lv_obj_t *obj)
{
    uint32_t child_count = lv_obj_get_child_cnt(obj);
    for (uint32_t i = 0; i < child_count; i++) {
        lv_obj_t *child = lv_obj_get_child(obj, i);
        lv_obj_set_style_bg_opa(child, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_border_opa(child, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(child, LV_OPA_TRANSP, LV_PART_ITEMS);
        lv_obj_set_style_border_opa(child, LV_OPA_TRANSP, LV_PART_ITEMS);
        set_children_transparent(child);
    }
}

static void tarkov_apply(struct k_work *work)
{
    ARG_UNUSED(work);

    lv_obj_t *screen = lv_scr_act();
    if (!screen) {
        return;
    }

    lv_style_init(&tarkov_bg_style);
    lv_style_set_bg_img_src(&tarkov_bg_style, &tarkov_img_dsc);
    lv_obj_add_style(screen, &tarkov_bg_style, 0);
    set_children_transparent(screen);
    lv_obj_invalidate(screen);
}

static K_WORK_DELAYABLE_DEFINE(tarkov_work, tarkov_apply);

static int tarkov_widget_init(void)
{
    k_work_schedule(&tarkov_work, K_MSEC(2000));
    return 0;
}

SYS_INIT(tarkov_widget_init, APPLICATION, 90);
