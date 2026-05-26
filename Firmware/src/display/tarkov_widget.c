#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <lvgl.h>

#include "tarkov_img_data.h"

static void tarkov_draw_event(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_DRAW_MAIN) {
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
            uint16_t val = (r5 << 11) | (g6 << 5) | b5;
            buf[buf_idx].full = ((val & 0xFF) << 8) | (val >> 8);
        }
    }
}

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

    lv_obj_add_event_cb(screen, tarkov_draw_event, LV_EVENT_DRAW_MAIN, NULL);
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
