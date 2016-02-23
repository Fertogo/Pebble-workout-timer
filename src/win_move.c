#include <pebble.h>
#include "win_move.h"

Move* move; 
void timer_move_controls_click_config_provider(void* context);
void reps_move_controls_click_config_provider(void* context);
void next_rep_click_handler(ClickRecognizerRef recognizer, void *context); 
void next_move_click_handler(ClickRecognizerRef recognizer, void *context); 
void stop_move_click_handler(ClickRecognizerRef recognizer, void *context); 
void pause_play_move_click_handler(ClickRecognizerRef recognizer, void *context); 


// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_18;
static GBitmap *s_res_stop_button;
static GBitmap *s_res_play_pause_button;
static GBitmap *s_res_next_button;
static GFont s_res_gothic_28;
static GFont s_res_roboto_condensed_21;
static GFont s_res_bitham_42_medium_numbers;
static TextLayer *s_textlayer_1;
static TextLayer *paused_text;
static TextLayer *move_name;
static TextLayer *move_value;
static ActionBarLayer *move_controls;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, 0);
  #endif
  
  s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  s_res_stop_button = gbitmap_create_with_resource(RESOURCE_ID_STOP_BUTTON);
  s_res_play_pause_button = gbitmap_create_with_resource(RESOURCE_ID_PLAY_PAUSE_BUTTON);
  s_res_next_button = gbitmap_create_with_resource(RESOURCE_ID_NEXT_BUTTON);
  s_res_gothic_28 = fonts_get_system_font(FONT_KEY_GOTHIC_28);
  s_res_roboto_condensed_21 = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
  s_res_bitham_42_medium_numbers = fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS);
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(2, 132, 121, 19));
  text_layer_set_text(s_textlayer_1, "Next: Pushups....");
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_1, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
  
  // paused_text
  paused_text = text_layer_create(GRect(32, 103, 69, 34));
  text_layer_set_text(paused_text, "Paused");
  text_layer_set_text_alignment(paused_text, GTextAlignmentCenter);
  text_layer_set_font(paused_text, s_res_gothic_28);
  layer_add_child(window_get_root_layer(s_window), (Layer *)paused_text);
  
  // move_name
  move_name = text_layer_create(GRect(8, -1, 108, 64));
  text_layer_set_text(move_name, "Workout Name");
  text_layer_set_text_alignment(move_name, GTextAlignmentCenter);
  text_layer_set_font(move_name, s_res_roboto_condensed_21);
  layer_add_child(window_get_root_layer(s_window), (Layer *)move_name);
  
  // move_value
  move_value = text_layer_create(GRect(7, 54, 111, 48));
  text_layer_set_text(move_value, "1:500");
  text_layer_set_text_alignment(move_value, GTextAlignmentCenter);
  text_layer_set_font(move_value, s_res_bitham_42_medium_numbers);
  layer_add_child(window_get_root_layer(s_window), (Layer *)move_value);
  
  // move_controls
  move_controls = action_bar_layer_create();
  action_bar_layer_add_to_window(move_controls, s_window);
  action_bar_layer_set_background_color(move_controls, GColorBlack);
  action_bar_layer_set_icon(move_controls, BUTTON_ID_UP, s_res_stop_button);
  action_bar_layer_set_icon(move_controls, BUTTON_ID_SELECT, s_res_play_pause_button);
  action_bar_layer_set_icon(move_controls, BUTTON_ID_DOWN, s_res_next_button);
  layer_add_child(window_get_root_layer(s_window), (Layer *)move_controls);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_1);
  text_layer_destroy(paused_text);
  text_layer_destroy(move_name);
  text_layer_destroy(move_value);
  action_bar_layer_destroy(move_controls);
  gbitmap_destroy(s_res_stop_button);
  gbitmap_destroy(s_res_play_pause_button);
  gbitmap_destroy(s_res_next_button);
}
// END AUTO-GENERATED UI CODE

void win_move_set_move(Move* move_win) { 
  win_move_hide_pause(); 
  move = move_win; 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Move name: %s", move->name); 
  text_layer_set_text(move_name, move->name); 
  if (move->type == MOVE_TYPE_TIMER) { 
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Timer move"); 
    action_bar_layer_set_icon(move_controls, BUTTON_ID_SELECT, s_res_play_pause_button);
    action_bar_layer_set_click_config_provider(move_controls, timer_move_controls_click_config_provider); 
  }
  if (move->type == MOVE_TYPE_REPS) { 
    APP_LOG(APP_LOG_LEVEL_DEBUG, "reps move"); 
    action_bar_layer_set_icon(move_controls, BUTTON_ID_SELECT, NULL);
    action_bar_layer_set_click_config_provider(move_controls, reps_move_controls_click_config_provider); 
  }
}

void timer_move_controls_click_config_provider(void* context) { 
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) next_move_click_handler); 
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) stop_move_click_handler); 
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) pause_play_move_click_handler); 
}
void reps_move_controls_click_config_provider(void* context) { 
  window_long_click_subscribe(BUTTON_ID_DOWN, 1000, (ClickHandler) next_move_click_handler, NULL); 
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) next_rep_click_handler); 
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) stop_move_click_handler); 
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) next_rep_click_handler); 
}

//Click Handlers
void next_rep_click_handler(ClickRecognizerRef recognizer, void *context) { 
  move_rep_next(move); 
} 
void next_move_click_handler(ClickRecognizerRef recognizer, void *context) { 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Next Button clicked"); 
  move_finish(move); 
}
void stop_move_click_handler(ClickRecognizerRef recognizer, void *context) { 
  move_stop(move); 
  hide_win_move(); 
} 
void pause_play_move_click_handler(ClickRecognizerRef recognizer, void *context) { 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Pause button clicked"); 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Move name: %s", move->name); 

  move_timer_pause_or_resume(move); 
} 

static void handle_window_unload(Window* window) {
  destroy_ui();
}


void win_move_set_value(char * new_value) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Winmove set value"); 
  text_layer_set_text(move_value, new_value); 
}

void show_win_move(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_win_move(void) {
  window_stack_remove(s_window, true);
}
void win_move_show_pause() { 
  text_layer_set_text(paused_text, "Paused");
} 
void win_move_hide_pause(){ 
  text_layer_set_text(paused_text, "\0"); //Empty String
}


