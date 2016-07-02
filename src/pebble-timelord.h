#ifndef PEBBLE_TIMELORD_PEBBLE_TIMELORD_H
#define PEBBLE_TIMELORD_PEBBLE_TIMELORD_H

int main(void);

static void init();

static void deinit();

static void main_window_load(Window *window);

static void main_window_unload(Window *window);

static void tick_handler(struct tm *tick_time, TimeUnits units_changed);

static void inbox_received_callback(DictionaryIterator *iterator, void *context);

static void inbox_dropped_callback(AppMessageResult reason, void *context);

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);

static void outbox_sent_callback(DictionaryIterator *iterator, void *context);

static void create_studio_layer(Layer *window_layer, GRect bounds);

static void create_show_time_layer(Layer *window_layer, GRect bounds);

static void create_show_name_layer(Layer *window_layer, GRect bounds);

#endif //PEBBLE_TIMELORD_PEBBLE_TIMELORD_H
