/* 
Loading Window | Custom Workout Timer. v4.0
Shows loading and error messages to user when necessary. 
Copyright © 2016 Fernando Trujano
                 trujano@mit.edu
*/

#include <pebble.h>
#include "win_loading.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_roboto_condensed_21;
static TextLayer *loading;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  s_res_roboto_condensed_21 = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
  // loading
  loading = text_layer_create(GRect(20, 20, 100, 125));
  text_layer_set_background_color(loading, GColorClear);
  text_layer_set_text(loading, "Loading...");
  text_layer_set_text_alignment(loading, GTextAlignmentCenter);
  text_layer_set_font(loading, s_res_roboto_condensed_21);
  layer_add_child(window_get_root_layer(s_window), (Layer *)loading);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(loading);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_win_loading(void) {
  initialise_ui();
  #ifdef PBL_COLOR
    window_set_background_color(s_window,GColorRajah );
  #endif
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void win_loading_show_error(char* error) { 
  text_layer_set_text(loading, error); 
  #ifdef PBL_COLOR
    window_set_background_color(s_window,GColorRed );
  #endif
}

void hide_win_loading(void) {
  window_stack_remove(s_window, true);
}
