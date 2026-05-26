#include <zephyr/kernel.h>
#include <lvgl.h>
#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/layer_status.h>

#include "tarkov_img_data.h"

static struct zmk_widget_output_status output_widget;
static struct zmk_widget_layer_status layer_widget;

static void tarkov_draw_event(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_DRAW_MAIN) return;

    lv_draw_ctx_t *draw_ctx = lv_event_get_draw_ctx(e);
    lv_coord_t buf_stride = lv_area_get_width(draw_ctx->buf_area);
    lv_color_t *buf = (lv_color_t *)draw_ctx->buf;
    lv_color_t white = lv_color_make(0xFF, 0xFF, 0xFF);

    const lv_area_t *clip = draw_ctx->clip_area;
    lv_coord_t y0 = clip->y1;
    lv_coord_t y1 = clip->y2;
    lv_coord_t x0 = clip->x1;
    lv_coord_t x1 = clip->x2;

    for (lv_coord_t y = y0; y <= y1; y++) {
        int buf_row = (y - draw_ctx->buf_area->y1) * buf_stride;
        for (lv_coord_t x = x0; x <= x1; x++) {
            int buf_idx = buf_row + (x - draw_ctx->buf_area->x1);
            buf[buf_idx] = white;
        }
    }
}

static void tarkov_apply(struct k_work *work)
{
    lv_obj_t *screen = lv_scr_act();
    if (!screen) {
        return;
    }

    lv_obj_add_event_cb(screen, tarkov_draw_event, LV_EVENT_DRAW_MAIN, NULL);

    uint32_t cnt = lv_obj_get_child_cnt(screen);
    for (uint32_t i = 0; i < cnt; i++) {
        lv_obj_t *child = lv_obj_get_child(screen, i);
        lv_obj_set_style_bg_opa(child, LV_OPA_TRANSP, 0);
    }

    lv_obj_invalidate(screen);
}

static K_WORK_DELAYABLE_DEFINE(tarkov_work, tarkov_apply);

static int tarkov_widget_init(void)
{
    k_work_schedule(&tarkov_work, K_MSEC(2000));
    return 0;
}

SYS_INIT(tarkov_widget_init, APPLICATION, 90);

lv_obj_t *zmk_display_status_screen(void)
{
    lv_obj_t *screen = lv_obj_create(NULL);

    lv_obj_add_event_cb(screen, tarkov_draw_event, LV_EVENT_DRAW_MAIN, NULL);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);

    zmk_widget_output_status_init(&output_widget, screen);
    zmk_widget_layer_status_init(&layer_widget, screen);

    const lv_font_t *font = lv_theme_get_font_small(lv_scr_act());
    if (font) {
        lv_obj_set_style_text_font(zmk_widget_output_status_obj(&output_widget),
                                   font,
                                   LV_PART_MAIN);
        lv_obj_set_style_text_font(zmk_widget_layer_status_obj(&layer_widget),
                                   font,
                                   LV_PART_MAIN);
    }

    lv_obj_align(zmk_widget_output_status_obj(&output_widget), LV_ALIGN_TOP_LEFT,   0, 0);
    lv_obj_align(zmk_widget_layer_status_obj(&layer_widget),   LV_ALIGN_BOTTOM_MID, 0, 0);

    return screen;
}