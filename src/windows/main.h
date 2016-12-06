#ifndef PEBBLE_TIMELORD_MAIN_H
#define PEBBLE_TIMELORD_MAIN_H

#include "../types.h"

typedef struct {
    char *studio_name;
    show *shows;
    int num_shows;
} w_main_content;

void main_window_init(void);

void main_window_deinit(void);

void main_window_show(w_main_content content);

void main_window_hide(void);

bool main_window_is_visible(void);

void main_window_update(w_main_content content);

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

static void update_show_time(uint32_t start, uint32_t end);

static void render_show_info();

#endif //PEBBLE_TIMELORD_MAIN_H
