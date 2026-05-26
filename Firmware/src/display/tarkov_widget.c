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
            if (img_off + 1 >= (int)sizeof(tarkov_img_data)) continue;
            uint16_t raw = (uint16_t)tarkov_img_data[img_off] |
                           ((uint16_t)tarkov_img_data[img_off + 1] << 8);
            /* BGR panel + big-endian SPI: swap R/B channels then swap bytes */
            uint16_t r5 =  raw        & 0x1F;
            uint16_t g6 = (raw >>  5) & 0x3F;
            uint16_t b5 = (raw >> 11) & 0x1F;
            uint16_t val = (r5 << 11) | (g6 << 5) | b5;
            buf[buf_idx].full = ((val & 0xFF) << 8) | (val >> 8);
        }
    }
}

lv_obj_t *zmk_display_status_screen(void)
{
    lv_obj_t *screen = lv_obj_create(NULL);

    lv_obj_add_event_cb(screen, tarkov_draw_event, LV_EVENT_DRAW_MAIN, NULL);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);

    zmk_widget_output_status_init(&output_widget, screen);
    zmk_widget_layer_status_init(&layer_widget, screen);

    lv_obj_set_style_text_font(zmk_widget_output_status_obj(&output_widget),
                               lv_theme_get_font_small(screen),
                               LV_PART_MAIN);
    lv_obj_set_style_text_font(zmk_widget_layer_status_obj(&layer_widget),
                               lv_theme_get_font_small(screen),
                               LV_PART_MAIN);

    lv_obj_align(zmk_widget_output_status_obj(&output_widget), LV_ALIGN_TOP_LEFT,   0, 0);
    lv_obj_align(zmk_widget_layer_status_obj(&layer_widget),   LV_ALIGN_BOTTOM_MID, 0, 0);

    return screen;
}