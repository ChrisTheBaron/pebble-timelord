#ifndef PEBBLE_TIMELORD_DESCRIPTION_H
#define PEBBLE_TIMELORD_DESCRIPTION_H

#include <pebble.h>
#include "../types.h"

void description_window_init(void);

void description_window_deinit(void);

void description_window_show(show show);

void description_window_hide(void);

bool description_window_is_visible(void);

static void description_window_update(show show);

static void description_window_load(Window *window);

static void description_window_unload(Window *window);

#endif //PEBBLE_TIMELORD_DESCRIPTION_H
