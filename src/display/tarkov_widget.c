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
    lv_coord_t buf_w = lv_area_get_width(draw_ctx->buf_area);
    lv_color_t *buf = (lv_color_t *)draw_ctx->buf;

    lv_coord_t y_start = LV_MAX(clip->y1, obj_coords.y1);
    lv_coord_t y_end   = LV_MIN(clip->y2, obj_coords.y2);
    lv_coord_t x_start = LV_MAX(clip->x1, obj_coords.x1);
    lv_coord_t x_end   = LV_MIN(clip->x2, obj_coords.x2);

    for (lv_coord_t y = y_start; y <= y_end; y++) {
        int img_y   = y - obj_coords.y1;
        int buf_row = (y - draw_ctx->buf_area->y1) * buf_w;

        for (lv_coord_t x = x_start; x <= x_end; x++) {
            int img_x   = x - obj_coords.x1;
            int img_off = (img_y * TARKOV_IMG_W + img_x) * 2;
            int buf_idx = buf_row + (x - draw_ctx->buf_area->x1);

            /* Image stored LE (lo byte first) — matches LVGL ARM framebuffer */
            buf[buf_idx].full = (uint16_t)tarkov_img_data[img_off] |
                                ((uint16_t)tarkov_img_data[img_off + 1] << 8);
        }
    }
}

static int tarkov_widget_init(void)
{
    lv_obj_t *screen = lv_scr_act();
    if (!screen) {
        return 0;
    }

    /* Full-screen object that draws raw pixels — no image decoder needed */
    lv_obj_t *bg = lv_obj_create(screen);
    lv_obj_set_size(bg, TARKOV_IMG_W, TARKOV_IMG_H);
    lv_obj_set_pos(bg, 0, 0);
    lv_obj_set_style_bg_opa(bg, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(bg, 0, 0);
    lv_obj_set_style_pad_all(bg, 0, 0);
    lv_obj_add_event_cb(bg, tarkov_draw_event, LV_EVENT_DRAW_MAIN, NULL);
    lv_obj_move_to_index(bg, 0);

    /*
     * ZMK's widget containers have opaque white backgrounds by default.
     * Make them transparent so the Tarkov image shows through beneath.
     * Start from index 1 to skip our own bg object at index 0.
     */
    uint32_t child_cnt = lv_obj_get_child_cnt(screen);
    for (uint32_t i = 1; i < child_cnt; i++) {
        lv_obj_t *child = lv_obj_get_child(screen, i);
        lv_obj_set_style_bg_opa(child, LV_OPA_TRANSP, 0);
    }

    return 0;
}

SYS_INIT(tarkov_widget_init, APPLICATION, CONFIG_IRONHAND_TARKOV_DISPLAY_INIT_PRIORITY);
