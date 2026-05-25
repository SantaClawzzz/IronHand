#include <zephyr/kernel.h>
#include <lvgl.h>
#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/layer_status.h>

#include "tarkov_img_data.h"

static struct zmk_widget_output_status output_widget;
static struct zmk_widget_layer_status layer_widget;

static lv_img_dsc_t tarkov_dsc = {
    .header = {
        .cf = LV_IMG_CF_TRUE_COLOR,
        .w = TARKOV_IMG_W,
        .h = TARKOV_IMG_H,
    },
    .data_size = TARKOV_IMG_W * TARKOV_IMG_H * 2,
    .data = tarkov_img_data,
};

static void set_bg_transp(lv_obj_t *obj)
{
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_img_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
    for (uint32_t i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        set_bg_transp(lv_obj_get_child(obj, i));
    }
}

lv_obj_t *zmk_display_status_screen(void)
{
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_size(screen, TARKOV_IMG_W, TARKOV_IMG_H);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);

    lv_obj_t *bg = lv_img_create(screen);
    lv_img_set_src(bg, &tarkov_dsc);
    lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);
    
    zmk_widget_output_status_init(&output_widget, screen);
    zmk_widget_layer_status_init(&layer_widget, screen);

    lv_obj_align(zmk_widget_output_status_obj(&output_widget), LV_ALIGN_TOP_LEFT,   0, 0);
    lv_obj_align(zmk_widget_layer_status_obj(&layer_widget),   LV_ALIGN_BOTTOM_MID, 0, 0);

    for (uint32_t i = 0; i < lv_obj_get_child_cnt(screen); i++) {
        set_bg_transp(lv_obj_get_child(screen, i));
    }

    return screen;
}
