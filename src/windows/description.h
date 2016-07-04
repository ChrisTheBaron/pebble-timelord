#ifndef PEBBLE_TIMELORD_DESCRIPTION_H
#define PEBBLE_TIMELORD_DESCRIPTION_H

struct description_window_content {
    char *show_name;
    char *show_description;
};

void description_window_init(void);

void description_window_deinit(void);

void description_window_show(struct description_window_content content);

void description_window_hide(void);

bool description_window_is_visible(void);

static void description_window_update(struct description_window_content content);

static void description_window_load(Window *window);

static void description_window_unload(Window *window);

#endif //PEBBLE_TIMELORD_DESCRIPTION_H
