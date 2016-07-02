#ifndef PEBBLE_TIMELORD_SPLASH_H
#define PEBBLE_TIMELORD_SPLASH_H

void splash_window_init(void);

void splash_window_deinit(void);

void splash_window_show(void);

void splash_window_hide(void);

bool splash_window_is_visible(void);

static void splash_window_load(Window *window);

static void splash_window_unload(Window *window);

#endif //PEBBLE_TIMELORD_SPLASH_H
