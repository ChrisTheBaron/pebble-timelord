#ifndef PEBBLE_TIMELORD_MAIN_H
#define PEBBLE_TIMELORD_MAIN_H

void main_window_init(void);

void main_window_deinit(void);

void main_window_show(void);

void main_window_hide(void);

bool main_window_is_visible(void);

void main_window_update(DictionaryIterator *iterator, void *context);

static void main_window_load(Window *window);

static void main_window_unload(Window *window);

static void create_studio_layer(Layer *window_layer, GRect bounds);

static void create_show_time_layer(Layer *window_layer, GRect bounds);

static void create_show_name_layer(Layer *window_layer, GRect bounds);

static void vertical_align_show_name(void);

#endif //PEBBLE_TIMELORD_MAIN_H
