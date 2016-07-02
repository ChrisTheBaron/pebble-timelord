#ifndef PEBBLE_TIMELORD_PEBBLE_TIMELORD_H
#define PEBBLE_TIMELORD_PEBBLE_TIMELORD_H

int main(void);

static void init();

static void deinit();

static void tick_handler(struct tm *tick_time, TimeUnits units_changed);

static void inbox_received_callback(DictionaryIterator *iterator, void *context);

static void inbox_dropped_callback(AppMessageResult reason, void *context);

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);

static void outbox_sent_callback(DictionaryIterator *iterator, void *context);

#endif //PEBBLE_TIMELORD_PEBBLE_TIMELORD_H
