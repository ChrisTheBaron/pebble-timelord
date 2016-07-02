#include <pebble.h>
#include "splash.h"

static Window *s_window;
static BitmapLayer *s_layer_bitmap;
static GBitmap *s_bitmap_refresh;

void splash_window_init(void) {
    s_window = window_create();
    window_set_background_color(s_window, GColorWhite);
    window_set_window_handlers(s_window, (WindowHandlers) {
            .load = splash_window_load,
            .unload = splash_window_unload
    });
}

void splash_window_deinit(void) {
    window_destroy(s_window);
}

void splash_window_show(void) {
    window_stack_push(s_window, false);
}

void splash_window_hide(void) {
    window_stack_remove(s_window, false);
}

bool splash_window_is_visible(void) {
    return window_stack_get_top_window() == s_window;
}

static void splash_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);

    GSize window_size = layer_get_bounds(window_get_root_layer(window)).size;

    s_bitmap_refresh = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LOGO_125);
    GSize icon_size = gbitmap_get_bounds(s_bitmap_refresh).size;

    s_layer_bitmap = bitmap_layer_create(
            GRect(
                    (window_size.w / 2 - icon_size.w / 2),
                    (window_size.h / 2 - icon_size.h / 2),
                    icon_size.w,
                    icon_size.h
            )
    );

    bitmap_layer_set_bitmap(s_layer_bitmap, s_bitmap_refresh);
    bitmap_layer_set_compositing_mode(s_layer_bitmap, GCompOpSet);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_layer_bitmap));
}

static void splash_window_unload(Window *window) {
    bitmap_layer_destroy(s_layer_bitmap);
    gbitmap_destroy(s_bitmap_refresh);
}

