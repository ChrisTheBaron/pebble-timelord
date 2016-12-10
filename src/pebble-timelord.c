#include <pebble.h>
#include "pebble-timelord.h"

#include "windows/main.h"
#include "windows/splash.h"
#include "windows/description.h"

#include "types.h"

GFont s_font_bold_30;
GFont s_font_semi_bold_22;
GFont s_font_semi_bold_20;

int main(void) {
    init();
    app_event_loop();
    deinit();
}

static void init() {
    // Load the custom fonts
    s_font_bold_30 = fonts_load_custom_font(
            resource_get_handle(RESOURCE_ID_FONT_OPEN_SANS_BOLD_30));
    s_font_semi_bold_22 = fonts_load_custom_font(
            resource_get_handle(RESOURCE_ID_FONT_OPEN_SANS_SEMI_BOLD_22));
    s_font_semi_bold_20 = fonts_load_custom_font(
            resource_get_handle(RESOURCE_ID_FONT_OPEN_SANS_SEMI_BOLD_20));

    // These are the windows we need straight away
    main_window_init();
    splash_window_init();

    splash_window_show();

    // These ones can wait
    description_window_init();

    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    // Register callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);

    // Open AppMessage
    const int inbox_size = 2048;
    const int outbox_size = 128;
    app_message_open(inbox_size, outbox_size);
}

static void deinit() {
    // Destroy Windows
    description_window_deinit();
    splash_window_deinit();
    main_window_deinit();
    // Unload GFont
    fonts_unload_custom_font(s_font_bold_30);
    fonts_unload_custom_font(s_font_semi_bold_22);
    fonts_unload_custom_font(s_font_semi_bold_20);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {

//    APP_LOG(APP_LOG_LEVEL_ERROR, "Update triggered!");

//  Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
//  Send the message!
    app_message_outbox_send();

}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

    Tuple *num_shows_tuple = dict_find(iterator, MESSAGE_KEY_NUM_SHOWS);

    int num_shows = 0;
    if (num_shows_tuple) {
        num_shows = num_shows_tuple->value->int8;
    }

    // Store incoming information
    w_main_content content = {
            .studio_name = calloc(10, sizeof(char)),
            .shows = calloc(num_shows, sizeof(show)),
            .num_shows = num_shows
    };

    Tuple *studio_tuple = dict_find(iterator, MESSAGE_KEY_CURRENT_STUDIO);

    // Check that we've received the data we need
    if (studio_tuple) {
        strcpy(content.studio_name, studio_tuple->value->cstring);
    }

    for (int i = 0; i < num_shows; i++) {

        content.shows[i].name = calloc(100, sizeof(char));
        content.shows[i].description = calloc(1000, sizeof(char));

        // Read tuples for data
        Tuple *name_tuple = dict_find(iterator, MESSAGE_KEY_SHOW_NAME + i);
        Tuple *start_tuple = dict_find(iterator, MESSAGE_KEY_SHOW_START + i);
        Tuple *end_tuple = dict_find(iterator, MESSAGE_KEY_SHOW_END + i);
        Tuple *desc_tuple = dict_find(iterator, MESSAGE_KEY_SHOW_DESC + i);

        if (name_tuple) {
            strcpy(content.shows[i].name, name_tuple->value->cstring);
        }

        if (desc_tuple) {
            strcpy(content.shows[i].description, desc_tuple->value->cstring);
        }

        if (start_tuple) {
            content.shows[i].start = (unsigned int) start_tuple->value->uint32;
        }

        if (end_tuple) {
            content.shows[i].finish = (unsigned int) end_tuple->value->uint32;
        }

    }

    if (!main_window_is_visible()) {
        // Show the main window
        main_window_show(content);
        splash_window_hide();
        APP_LOG(APP_LOG_LEVEL_INFO, "Showing main window");
    } else {
        main_window_update(content);
        APP_LOG(APP_LOG_LEVEL_INFO, "Don't need to show main window");
    }

}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void
outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
