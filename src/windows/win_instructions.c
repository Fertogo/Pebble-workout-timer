/* 
Instructions Window | Custom Workout Timer. v4.0
Shows Instructions to user on first launch
Copyright © 2016 Fernando Trujano
                 trujano@mit.edu
*/

#include <pebble.h>
#include "win_instructions.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_24_bold;
static TextLayer *instructions_text;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  // instructions_text
  instructions_text = text_layer_create(GRect(5, 8, 135, 155));
  text_layer_set_background_color(instructions_text, GColorClear);
  text_layer_set_text(instructions_text, "Use your phone to add workouts. On the pebble app, find this timer app and click on the settings icon. ");
  text_layer_set_font(instructions_text, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)instructions_text);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(instructions_text);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_win_instructions(void) {
  initialise_ui();
  #ifdef PBL_COLOR
    window_set_background_color(s_window,GColorCyan );
  #endif
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_win_instructions(void) {
  window_stack_remove(s_window, true);
}
