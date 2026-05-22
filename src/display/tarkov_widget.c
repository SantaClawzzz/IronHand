#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <lvgl.h>

#include "tarkov_img_data.h"

static void tarkov_draw_event(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_DRAW_MAIN) {
        return;
    }

    lv_obj_t *obj       = lv_event_get_target(e);
    lv_draw_ctx_t *draw_ctx = lv_event_get_draw_ctx(e);

    lv_area_t obj_coords;
    lv_obj_get_coords(obj, &obj_coords);

    const lv_area_t *clip = draw_ctx->clip_area;
    lv_coord_t buf_w  = lv_area_get_width(draw_ctx->buf_area);
    lv_color_t *buf   = (lv_color_t *)draw_ctx->buf;

    lv_coord_t y0 = LV_MAX(clip->y1, obj_coords.y1);
    lv_coord_t y1 = LV_MIN(clip->y2, obj_coords.y2);
    lv_coord_t x0 = LV_MAX(clip->x1, obj_coords.x1);
    lv_coord_t x1 = LV_MIN(clip->x2, obj_coords.x2);

    for (lv_coord_t y = y0; y <= y1; y++) {
        int img_y   = y - obj_coords.y1;
        int buf_row = (y - draw_ctx->buf_area->y1) * buf_w;
        for (lv_coord_t x = x0; x <= x1; x++) {
            int img_x   = x - obj_coords.x1;
            int img_off = (img_y * TARKOV_IMG_W + img_x) * 2;
            int buf_idx = buf_row + (x - draw_ctx->buf_area->x1);
            buf[buf_idx].full = ((uint16_t)tarkov_img_data[img_off] << 8) |
                                (uint16_t)tarkov_img_data[img_off + 1];
        }
    }
}

static void tarkov_apply(struct k_work *work)
{
    lv_obj_t *screen = lv_scr_act();
    if (!screen) {
        return;
    }

    /* Draw the image in the screen's own DRAW_MAIN event — fires before children */
    lv_obj_add_event_cb(screen, tarkov_draw_event, LV_EVENT_DRAW_MAIN, NULL);

    /* Make every ZMK child widget background transparent so image shows through */
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
    /*
     * Delay long enough for ZMK to finish calling lv_scr_load().
     * lv_scr_act() returns the empty default screen until that call,
     * so any earlier hook lands on the wrong screen.
     */
    k_work_schedule(&tarkov_work, K_MSEC(2000));
    return 0;
}

SYS_INIT(tarkov_widget_init, APPLICATION, 90);
