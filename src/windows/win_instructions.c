#include <pebble.h>
#include "win_instructions.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static TextLayer *instructions_text;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  // instructions_text
  instructions_text = text_layer_create(GRect(20, 20, 100, 20));
  text_layer_set_text(instructions_text, "Instructions: ");
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
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_win_instructions(void) {
  window_stack_remove(s_window, true);
}
