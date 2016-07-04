#ifndef PEBBLE_TIMELORD_MAIN_H
#define PEBBLE_TIMELORD_MAIN_H

#include "description.h"

struct main_window_content {
    char *studio_name;
    char *show_name;
    uint32_t show_end;
    struct description_window_content description_window_content;
};

void main_window_init(void);

void main_window_deinit(void);

void main_window_show(struct main_window_content content);

void main_window_hide(void);

bool main_window_is_visible(void);

void main_window_update(struct main_window_content content);

static void main_window_load(Window *window);

static void main_window_unload(Window *window);

static void create_studio_layer(Layer *window_layer, GRect bounds);

static void create_show_time_layer(Layer *window_layer, GRect bounds);

static void create_show_name_layer(Layer *window_layer, GRect bounds);

static void vertical_align_show_name(void);

static void main_window_select_click_handler(ClickRecognizerRef recognizer, void *context);

static void main_window_up_click_handler(ClickRecognizerRef recognizer, void *context);

static void main_window_down_click_handler(ClickRecognizerRef recognizer, void *context);

static void main_window_click_config_provider(void *context);

#endif //PEBBLE_TIMELORD_MAIN_H
