#include <pebble.h>
#include "description.h"
#include "../types.h"

static Window *s_window;

static ScrollLayer *s_scroll_layer;
static TextLayer *s_name_layer;
static TextLayer *s_description_layer;

static const int max_scroll_height = 10000;

extern GFont s_font_semi_bold_20;
extern GFont s_font_semi_bold_22;

void description_window_init(void) {
    s_window = window_create();
    window_set_background_color(s_window, GColorWhite);
    window_set_window_handlers(s_window, (WindowHandlers) {
            .load = description_window_load,
            .unload = description_window_unload
    });
}

void description_window_deinit(void) {
    window_destroy(s_window);
}

void description_window_show(show show) {
    window_stack_push(s_window, false);
    description_window_update(show);
}

void description_window_hide(void) {
    window_stack_remove(s_window, false);
}

bool description_window_is_visible(void) {
    return window_stack_get_top_window() == s_window;
}

void description_window_update(show show) {
    Layer *window_layer = window_get_root_layer(s_window);
    GRect bounds = layer_get_frame(window_layer);
    text_layer_set_text(s_name_layer, show.name);
    text_layer_set_text(s_description_layer, show.description);
    GSize max_size_name = text_layer_get_content_size(s_name_layer);
    // This adds some spacing between the two layers
    max_size_name.h += 5;
    GSize max_size_desc = text_layer_get_content_size(s_description_layer);
    text_layer_set_size(s_name_layer, max_size_name);
    layer_set_frame(text_layer_get_layer(s_description_layer), GRect(
            0,
            max_size_name.h,
            bounds.size.w,
            max_size_desc.h
    ));
    scroll_layer_set_content_size(s_scroll_layer,
                                  GSize(bounds.size.w, max_size_name.h + max_size_desc.h + 5));
}

static void description_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);
    GRect max_text_bounds = GRect(0, 0, bounds.size.w, max_scroll_height);

    // Initialize the scroll layer
    s_scroll_layer = scroll_layer_create(bounds);

    // This binds the scroll layer to the window so that up and down map to scrolling
    // You may use scroll_layer_set_callbacks to add or override interactivity
    scroll_layer_set_click_config_onto_window(s_scroll_layer, window);

    // Initialize the text layer
    s_name_layer = text_layer_create(max_text_bounds);

    // Style the text
    text_layer_set_background_color(s_name_layer, GColorClear);
    text_layer_set_text_color(s_name_layer, GColorBlack);
    text_layer_set_text_alignment(s_name_layer, GTextAlignmentLeft);
    text_layer_set_font(s_name_layer, s_font_semi_bold_22);
    text_layer_set_overflow_mode(s_name_layer, GTextOverflowModeWordWrap);

    // Initialize the text layer
    s_description_layer = text_layer_create(max_text_bounds);

    // Style the text
    text_layer_set_background_color(s_description_layer, GColorClear);
    text_layer_set_text_color(s_description_layer, GColorBlack);
    text_layer_set_text_alignment(s_description_layer, GTextAlignmentLeft);
    text_layer_set_font(s_description_layer, s_font_semi_bold_20);
    text_layer_set_overflow_mode(s_description_layer, GTextOverflowModeWordWrap);

    // Add the layers for display
    scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_name_layer));
    scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_description_layer));
    layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));
}

static void description_window_unload(Window *window) {
    text_layer_destroy(s_description_layer);
    text_layer_destroy(s_name_layer);
    scroll_layer_destroy(s_scroll_layer);
}

