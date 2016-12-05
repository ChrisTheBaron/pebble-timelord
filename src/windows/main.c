#include <pebble.h>
#include <math.h>
#include "main.h"
#include "description.h"

static const int show_time_height = 26;
static const int studio_name_height = 40;

static Window *s_window;

extern GFont s_font_bold_30;
extern GFont s_font_semi_bold_22;
extern GFont s_font_semi_bold_20;

static TextLayer *s_studio_layer;
static TextLayer *s_show_time_layer;
static TextLayer *s_show_name_layer;

static struct description_window_content s_desc_content;

void main_window_init(void) {
    // Create main Window element and assign to pointer
    s_window = window_create();
    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_window, (WindowHandlers) {
            .load = main_window_load,
            .unload = main_window_unload
    });
    // Set the window click listeners
    window_set_click_config_provider(s_window, main_window_click_config_provider);
}

void main_window_deinit(void) {
    window_destroy(s_window);
}

void main_window_show(struct main_window_content content) {
    window_stack_push(s_window, false);
    main_window_update(content);
}

void main_window_hide(void) {
    window_stack_remove(s_window, false);
}

bool main_window_is_visible(void) {
    return window_stack_get_top_window() == s_window;
}

void main_window_update(struct main_window_content content) {
    // Store the description content for later use
    s_desc_content = content.description_window_content;

    text_layer_set_text(s_studio_layer, content.studio_name);

    text_layer_set_text(s_show_name_layer, content.show_name);
    vertical_align_show_name();

    update_show_time(content.show_start, content.show_end);
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

static void create_studio_layer(Layer *window_layer, GRect bounds) {
    // Create the TextLayer with specific bounds
    s_studio_layer = text_layer_create(GRect(0, 0, bounds.size.w, studio_name_height));

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
    s_show_name_layer = text_layer_create(
            GRect(
                    0,
                    studio_name_height,
                    bounds.size.w,
                    bounds.size.h - studio_name_height - show_time_height
            ));

    // Style the text
    text_layer_set_background_color(s_show_name_layer, GColorClear);
    text_layer_set_text_color(s_show_name_layer, GColorBlack);
    text_layer_set_text_alignment(s_show_name_layer, GTextAlignmentCenter);
    text_layer_set_font(s_show_name_layer, s_font_semi_bold_20);
    text_layer_set_overflow_mode(s_show_name_layer, GTextOverflowModeWordWrap);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_show_name_layer));
}

static void create_show_time_layer(Layer *window_layer, GRect bounds) {
    // Create temperature Layer
    s_show_time_layer = text_layer_create(
            GRect(
                    0,
                    bounds.size.h - show_time_height,
                    bounds.size.w,
                    show_time_height
            ));

    // Style the text
    text_layer_set_background_color(s_show_time_layer, GColorClear);
    text_layer_set_text_color(s_show_time_layer, GColorBlack);
    text_layer_set_text_alignment(s_show_time_layer, GTextAlignmentCenter);
    // We're not setting the font size here because it depends on the data we get

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(s_show_time_layer));
}

/**
 * "Vertically Aligns" the show name as much as it can.
 *
 * Assumes that we can show a maximum height of
 * (bounds.size.h - studio_name_height - show_time_height).
 */
static void vertical_align_show_name(void) {
    // Get information about the Window
    Layer *window_layer = window_get_root_layer(s_window);
    GRect bounds = layer_get_bounds(window_layer);
    // Calculate the amount of space we can take up
    int max_show_name_height = bounds.size.h - studio_name_height - show_time_height;
    GSize actual_size = text_layer_get_content_size(s_show_name_layer);
    // Make sure we don't go over the max amount
    int wanted_height = (int) fmin(actual_size.h, max_show_name_height);
    // Calculate the top position of the layer in the middle of the window
    int layer_position = (bounds.size.h / 2 - wanted_height / 2);
    // We can afford to make the layer as big as we can to stop characters being cut off
    int layer_height = bounds.size.h - layer_position - show_time_height;
    layer_set_frame(text_layer_get_layer(s_show_name_layer), GRect(
            0,
            layer_position,
            bounds.size.w,
            layer_height
    ));
}

static void main_window_select_click_handler(ClickRecognizerRef recognizer, void *context) {
    description_window_show(s_desc_content);
}

static void main_window_up_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void main_window_down_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void main_window_click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, main_window_select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, main_window_up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, main_window_down_click_handler);
}

static void update_show_time(uint32_t start, uint32_t end) {

    APP_LOG(APP_LOG_LEVEL_INFO, "Start: %d, End: %d", (int) start, (int) end);

    static char buffer[15];

    text_layer_set_font(s_show_time_layer, s_font_semi_bold_22);

    // If start is 0 then we don't have a start time.
    // This is probably the current show. We'll just
    // assume we can say the show started 'Now'.
    if (start) {
        time_t start_time = (time_t) start;
        struct tm *tick_time = localtime(&start_time);
        strftime(buffer, sizeof(buffer), "%H:%M - ", tick_time);
    } else {
        snprintf(buffer, sizeof(buffer), "Now - ");
    }

    // If end is 0 then we don't have an end time.
    // I.E. Jukebox "The End of Time" at term ends.
    if (end) {
        char end_buff[6];
        time_t end_time = (time_t) end;
        struct tm *tick_time = localtime(&end_time);
        text_layer_set_font(s_show_time_layer, s_font_semi_bold_22);
        strftime(end_buff, sizeof(end_buff), "%H:%M", tick_time);
        strcat(buffer, end_buff);
    } else {
        text_layer_set_font(s_show_time_layer, s_font_semi_bold_20);
        strcat(buffer, "Forever");
    }

    text_layer_set_text(s_show_time_layer, buffer);

}