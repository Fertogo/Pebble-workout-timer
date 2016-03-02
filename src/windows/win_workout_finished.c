#include <pebble.h>
#include "win_workout_finished.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_24_bold;
static GFont s_res_bitham_30_black;
static TextLayer *s_textlayer_2;
static TextLayer *workout_finished;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_bitham_30_black = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  // s_textlayer_2
  s_textlayer_2 = text_layer_create(GRect(0, 9, 143, 28));
  text_layer_set_background_color(s_textlayer_2, GColorClear);
  text_layer_set_text(s_textlayer_2, "Congratulations!");
  text_layer_set_text_alignment(s_textlayer_2, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_2, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_2);
  
  // workout_finished
  workout_finished = text_layer_create(GRect(1, 42, 142, 119));
  text_layer_set_background_color(workout_finished, GColorClear);
  text_layer_set_text(workout_finished, "Workout Finished :)");
  text_layer_set_text_alignment(workout_finished, GTextAlignmentCenter);
  text_layer_set_font(workout_finished, s_res_bitham_30_black);
  layer_add_child(window_get_root_layer(s_window), (Layer *)workout_finished);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_2);
  text_layer_destroy(workout_finished);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_win_workout_finished(void) {
  initialise_ui();
  #ifdef PBL_COLOR
     window_set_background_color(s_window,GColorGreen);
  #endif
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_win_workout_finished(void) {
  window_stack_remove(s_window, true);
}
