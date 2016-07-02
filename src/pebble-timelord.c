#include <pebble.h>
#include "pebble-timelord.h"

static Window *s_main_window;

static TextLayer *s_studio_layer;
static TextLayer *s_show_time_layer;
static TextLayer *s_show_name_layer;

static GFont s_font_bold_30;
static GFont s_font_semi_bold_22;
static GFont s_font_semi_bold_20;

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

    // Create main Window element and assign to pointer
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
            .load = main_window_load,
            .unload = main_window_unload
    });

    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);

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
    // Destroy Window
    window_destroy(s_main_window);
    // Unload GFont
    fonts_unload_custom_font(s_font_bold_30);
    fonts_unload_custom_font(s_font_semi_bold_22);
    fonts_unload_custom_font(s_font_semi_bold_20);
}

static void main_window_load(Window *window) {
    // Get information about the Window
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    create_studio_layer(window_layer, bounds);
    create_show_time_layer(window_layer, bounds);
    create_show_name_layer(window_layer, bounds);
}

static void main_window_unload(Window *window) {
    text_layer_destroy(s_studio_layer);
    text_layer_destroy(s_show_time_layer);
    text_layer_destroy(s_show_name_layer);
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

    // Store incoming information
    static char studio_buffer[10];
    static char name_buffer[100];
    static char end_buffer[15];

    // Read tuples for data
    Tuple *studio_tuple = dict_find(iterator, MESSAGE_KEY_CURRENT_STUDIO);
    Tuple *name_tuple = dict_find(iterator, MESSAGE_KEY_CURRENT_SHOW_NAME);
    Tuple *end_tuple = dict_find(iterator, MESSAGE_KEY_CURRENT_SHOW_END);

    // Check that we've received the data we need
    if (studio_tuple) {
        snprintf(studio_buffer, sizeof(studio_buffer), "%s", studio_tuple->value->cstring);
        text_layer_set_text(s_studio_layer, studio_buffer);
    }

    if (name_tuple) {
        snprintf(name_buffer, sizeof(name_buffer), "%s", name_tuple->value->cstring);
        text_layer_set_text(s_show_name_layer, name_buffer);
    }

    if (end_tuple) {
        uint32_t end_value = (uint32_t) end_tuple->value->uint32;
        // If end_value is 0 then we don't have an end time. I.E. Jukebox "The End of Time"
        if (end_value) {
            time_t end_time = (time_t) end_value;
            time_t temp = time(&end_time);
            struct tm *tick_time = localtime(&temp);
            text_layer_set_font(s_show_time_layer, s_font_semi_bold_22);
            strftime(end_buffer, sizeof(end_buffer), "Now - %I:%M", tick_time);
        } else {
            text_layer_set_font(s_show_time_layer, s_font_semi_bold_20);
            snprintf(end_buffer, sizeof(end_buffer), "Now - Forever");
        }
        text_layer_set_text(s_show_time_layer, end_buffer);
    }

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

static void create_studio_layer(Layer *window_layer, GRect bounds) {
    // Create the TextLayer with specific bounds
    s_studio_layer = text_layer_create(GRect(0, 0, bounds.size.w, 50));

    // Style the text
    text_layer_set_background_color(s_studio_layer, GColorClear);
    text_layer_set_text_color(s_studio_layer, GColorBlack);
    text_layer_set_text_alignment(s_studio_layer, GTextAlignmentCenter);
    text_layer_set_font(s_studio_layer, s_font_bold_30);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_studio_layer));
}

static void create_show_name_layer(Layer *window_layer, GRect bounds) {
    // Create temperature Layer
    s_show_name_layer = text_layer_create(GRect(0, 40, bounds.size.w, 75));

    // Style the text
    text_layer_set_background_color(s_show_name_layer, GColorClear);
    text_layer_set_text_color(s_show_name_layer, GColorBlack);
    text_layer_set_text_alignment(s_show_name_layer, GTextAlignmentCenter);
    text_layer_set_font(s_show_name_layer, s_font_semi_bold_22);
    text_layer_set_overflow_mode(s_show_name_layer, GTextOverflowModeWordWrap);
    text_layer_set_text(s_show_name_layer, "Loading...");

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_show_name_layer));
}

static void create_show_time_layer(Layer *window_layer, GRect bounds) {
    // Create temperature Layer
    s_show_time_layer = text_layer_create(GRect(0, bounds.size.h - 26, bounds.size.w, 26));

    // Style the text
    text_layer_set_background_color(s_show_time_layer, GColorClear);
    text_layer_set_text_color(s_show_time_layer, GColorBlack);
    text_layer_set_text_alignment(s_show_time_layer, GTextAlignmentCenter);
    // We're not setting the font size here because it depends on the data we get

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_show_time_layer));
}