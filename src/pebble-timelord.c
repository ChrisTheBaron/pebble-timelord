#include <pebble.h>
#include "pebble-timelord.h"
#include "windows/splash.h"
#include "windows/main.h"

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
    s_font_bold_30 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OPEN_SANS_BOLD_30));
    s_font_semi_bold_22 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OPEN_SANS_SEMI_BOLD_22));
    s_font_semi_bold_20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OPEN_SANS_SEMI_BOLD_20));

    main_window_init();
    main_window_show();

    splash_window_init();
    splash_window_show();

    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

    // Register callbacks
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);

    // Open AppMessage
    const int inbox_size = 128;
    const int outbox_size = 128;
    app_message_open(inbox_size, outbox_size);
}

static void deinit() {
    // Destroy Windows
    splash_window_deinit();
    main_window_deinit();
    // Unload GFont
    fonts_unload_custom_font(s_font_bold_30);
    fonts_unload_custom_font(s_font_semi_bold_22);
    fonts_unload_custom_font(s_font_semi_bold_20);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {

//    // Get weather update every 30 minutes
//    if (tick_time->tm_min % 5 == 0) {
//        // Begin dictionary
//        DictionaryIterator *iter;
//        app_message_outbox_begin(&iter);
//
//        // Add a key-value pair
//        dict_write_uint8(iter, 0, 0);
//
//        // Send the message!
//        app_message_outbox_send();
//    }

}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

    // Update the main window
    main_window_update(iterator, context);

    // and remove the splash screen
    splash_window_hide();

}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
