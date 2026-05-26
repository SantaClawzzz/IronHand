#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <lvgl.h>

#include "tarkov_img_data.h"

static lv_obj_t *tarkov_screen;
static lv_style_t tarkov_bg_style;
static bool tarkov_bg_style_initialized;

static void set_children_transparent(lv_obj_t *obj)
{
    uint32_t child_count = lv_obj_get_child_cnt(obj);
    for (uint32_t i = 0; i < child_count; i++) {
        lv_obj_t *child = lv_obj_get_child(obj, i);
        lv_obj_set_style_bg_opa(child, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_border_opa(child, LV_OPA_TRANSP, LV_PART_MAIN);
        set_children_transparent(child);
    }
}

static void tarkov_draw_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_DRAW_MAIN && code != LV_EVENT_DRAW_POST) {
        return;
    }

    lv_obj_t *obj = lv_event_get_target(e);
    lv_draw_ctx_t *draw_ctx = lv_event_get_draw_ctx(e);
    lv_area_t obj_coords;
    lv_obj_get_coords(obj, &obj_coords);

    const lv_area_t *clip = draw_ctx->clip_area;
    lv_coord_t buf_stride = lv_area_get_width(draw_ctx->buf_area);
    lv_color_t *buf = (lv_color_t *)draw_ctx->buf;

    lv_coord_t y0 = LV_MAX(clip->y1, obj_coords.y1);
    lv_coord_t y1 = LV_MIN(clip->y2, obj_coords.y2);
    lv_coord_t x0 = LV_MAX(clip->x1, obj_coords.x1);
    lv_coord_t x1 = LV_MIN(clip->x2, obj_coords.x2);

    for (lv_coord_t y = y0; y <= y1; y++) {
        int img_y = y - obj_coords.y1;
        int buf_row = (y - draw_ctx->buf_area->y1) * buf_stride;
        for (lv_coord_t x = x0; x <= x1; x++) {
            int img_x = x - obj_coords.x1;
            if (img_x < 0 || img_x >= TARKOV_IMG_W || img_y < 0 || img_y >= TARKOV_IMG_H) {
                continue;
            }

            int img_off = (img_y * TARKOV_IMG_W + img_x) * 2;
            int buf_idx = buf_row + (x - draw_ctx->buf_area->x1);
            if (img_off + 1 >= (int)sizeof(tarkov_img_data)) {
                continue;
            }

            uint16_t raw = (uint16_t)tarkov_img_data[img_off] |
                           ((uint16_t)tarkov_img_data[img_off + 1] << 8);
            uint16_t r5 = raw & 0x1F;
            uint16_t g6 = (raw >> 5) & 0x3F;
            uint16_t b5 = (raw >> 11) & 0x1F;
            lv_color_t px = lv_color_make(
                (uint8_t)((r5 * 255u) / 31u),
                (uint8_t)((g6 * 255u) / 63u),
                (uint8_t)((b5 * 255u) / 31u));
            buf[buf_idx] = px;
        }
    }
}

static void tarkov_apply(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(tarkov_work, tarkov_apply);

static void tarkov_apply(struct k_work *work)
{
    ARG_UNUSED(work);

    lv_obj_t *screen = lv_scr_act();
    if (!screen) {
        k_work_schedule(&tarkov_work, K_MSEC(1000));
        return;
    }

    if (!tarkov_bg_style_initialized) {
        lv_style_init(&tarkov_bg_style);
        lv_style_set_bg_img_src(&tarkov_bg_style, &tarkov_img_dsc);
        lv_style_set_bg_img_opa(&tarkov_bg_style, LV_OPA_COVER);
        lv_style_set_bg_img_tiled(&tarkov_bg_style, false);
        lv_style_set_bg_opa(&tarkov_bg_style, LV_OPA_COVER);
        tarkov_bg_style_initialized = true;
    }

    if (screen != tarkov_screen) {
        tarkov_screen = screen;
        lv_obj_add_event_cb(screen, tarkov_draw_event, LV_EVENT_DRAW_MAIN, NULL);
        lv_obj_add_event_cb(screen, tarkov_draw_event, LV_EVENT_DRAW_POST, NULL);
    }

    lv_obj_add_style(screen, &tarkov_bg_style, LV_PART_MAIN);
    lv_obj_set_style_border_opa(screen, LV_OPA_TRANSP, LV_PART_MAIN);
    set_children_transparent(screen);
    lv_obj_invalidate(screen);
    k_work_schedule(&tarkov_work, K_MSEC(1000));
}

static int tarkov_widget_init(void)
{
    k_work_schedule(&tarkov_work, K_MSEC(2000));
    return 0;
}

SYS_INIT(tarkov_widget_init, APPLICATION, 90);
