/* 
Move Window | Custom Workout Timer. v4.0
Displays Move information during a workout. Handles ActionBar actions. 
Copyright © 2016 Fernando Trujano
                 trujano@mit.edu
*/

#include <pebble.h>
#include "win_move.h"
#include "win_main.h"
#include "common.h"
#include "workout.h"


#define NEXT_PADDING 3 //Padding between "next" and move name.


Move* move;
void timer_move_controls_click_config_provider(void* context);
void reps_move_controls_click_config_provider(void* context);
void next_rep_click_handler(ClickRecognizerRef recognizer, void *context);
void next_move_click_handler(ClickRecognizerRef recognizer, void *context);
void stop_move_click_handler(ClickRecognizerRef recognizer, void *context);
void back_click_handler(ClickRecognizerRef recognizer, void *context); 
void pause_play_move_click_handler(ClickRecognizerRef recognizer, void *context);

void setup_timer_move();
void setup_rep_move();
void initialized_custom_ui();
static StatusBarLayer * status_bar;
static GFont s_res_gothic_24_bold;
static GFont s_res_gothic_18_bold;


// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_roboto_bold_subset_49;
static GFont s_res_gothic_14;
static GFont s_res_roboto_condensed_21;
static GBitmap *s_res_stop_button;
static GBitmap *s_res_play_pause_button;
static GBitmap *s_res_next_button;
static TextLayer *move_value;
static TextLayer *next_move_name;
static TextLayer *move_name;
static ActionBarLayer *move_controls;
static TextLayer *next;
static TextLayer *paused_text;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  s_res_roboto_bold_subset_49 = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_res_roboto_condensed_21 = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
  s_res_stop_button = gbitmap_create_with_resource(RESOURCE_ID_STOP_BUTTON);
  s_res_play_pause_button = gbitmap_create_with_resource(RESOURCE_ID_PLAY_PAUSE_BUTTON);
  s_res_next_button = gbitmap_create_with_resource(RESOURCE_ID_NEXT_BUTTON);
  // move_value
  move_value = text_layer_create(GRect(0, 68, 115, 49));
  text_layer_set_background_color(move_value, GColorClear);
  text_layer_set_text(move_value, "1:50");
  text_layer_set_text_alignment(move_value, GTextAlignmentCenter);
  text_layer_set_font(move_value, s_res_roboto_bold_subset_49);
  layer_add_child(window_get_root_layer(s_window), (Layer *)move_value);
  
  // next_move_name
  next_move_name = text_layer_create(GRect(37, 147, 85, 15));
  text_layer_set_background_color(next_move_name, GColorClear);
  text_layer_set_text(next_move_name, "Workout dsg fg sfdg ");
  text_layer_set_font(next_move_name, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)next_move_name);
  
  // move_name
  move_name = text_layer_create(GRect(0, 20, 115, 57));
  text_layer_set_background_color(move_name, GColorClear);
  text_layer_set_text(move_name, "Text layer");
  text_layer_set_text_alignment(move_name, GTextAlignmentCenter);
  text_layer_set_font(move_name, s_res_roboto_condensed_21);
  layer_add_child(window_get_root_layer(s_window), (Layer *)move_name);
  
  // move_controls
  move_controls = action_bar_layer_create();
  action_bar_layer_add_to_window(move_controls, s_window);
  action_bar_layer_set_background_color(move_controls, GColorBlack);
  action_bar_layer_set_icon(move_controls, BUTTON_ID_UP, s_res_stop_button);
  action_bar_layer_set_icon(move_controls, BUTTON_ID_SELECT, s_res_play_pause_button);
  action_bar_layer_set_icon(move_controls, BUTTON_ID_DOWN, s_res_next_button);
  layer_add_child(window_get_root_layer(s_window), (Layer *)move_controls);
  
  // next
  next = text_layer_create(GRect(4, 147, 35, 18));
  text_layer_set_background_color(next, GColorClear);
  text_layer_set_text(next, "Next:");
  layer_add_child(window_get_root_layer(s_window), (Layer *)next);
  
  // paused_text
  paused_text = text_layer_create(GRect(0, 121, 115, 26));
  text_layer_set_background_color(paused_text, GColorClear);
  text_layer_set_text(paused_text, "Paused");
  text_layer_set_text_alignment(paused_text, GTextAlignmentCenter);
  text_layer_set_font(paused_text, s_res_roboto_condensed_21);
  layer_add_child(window_get_root_layer(s_window), (Layer *)paused_text);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(move_value);
  text_layer_destroy(next_move_name);
  text_layer_destroy(move_name);
  action_bar_layer_destroy(move_controls);
  text_layer_destroy(next);
  text_layer_destroy(paused_text);
  gbitmap_destroy(s_res_stop_button);
  gbitmap_destroy(s_res_play_pause_button);
  gbitmap_destroy(s_res_next_button);
}
// END AUTO-GENERATED UI CODE

/**
* Sets the move this window should be displaying
* @param Move* move_win: Move to set
*/
void win_move_set_move(Move* move_win) {
  win_move_hide_pause();
  move = move_win;
  LOG("Move name: %s", move->name);
  text_layer_set_text(move_name, move->name);
  text_layer_set_font(move_name, s_res_roboto_condensed_21);
  //Adjust text based on font
  int16_t max_height = layer_get_frame(text_layer_get_layer(move_name)).size.h;
  if (text_layer_get_content_size(move_name).h >= max_height) {
     text_layer_set_font(move_name, s_res_gothic_24_bold);
     max_height = layer_get_frame(text_layer_get_layer(move_name)).size.h;
     if (text_layer_get_content_size(move_name).h >= max_height)  text_layer_set_font(move_name, s_res_gothic_18_bold);
  }

  //Type specific setup
  if (move->type == MOVE_TYPE_TIMER) setup_timer_move();
  if (move->type == MOVE_TYPE_REPS)  setup_rep_move();
}

/**
* Prepare window for a timer move to begin.
*/
void setup_timer_move() {
  LOG("Timer move");
  window_set_background_color(s_window, GColorWhite);
  action_bar_layer_set_icon(move_controls, BUTTON_ID_SELECT, s_res_play_pause_button);

  text_layer_set_text_color(paused_text, GColorBlack);
  text_layer_set_text_color(move_name, GColorBlack);
  text_layer_set_text_color(move_value, GColorBlack);
  text_layer_set_text_color(next_move_name, GColorBlack);
  text_layer_set_text_color(next, GColorBlack);

  action_bar_layer_set_click_config_provider(move_controls, timer_move_controls_click_config_provider);
}

/**
* Prepare window for a rep move to begin.
*/
void setup_rep_move() {
  LOG("reps move");
  window_set_background_color(s_window, GColorBlack);
  action_bar_layer_set_icon(move_controls, BUTTON_ID_SELECT, NULL);

  text_layer_set_text_color(paused_text, GColorWhite);
  text_layer_set_text_color(move_name, GColorWhite);
  text_layer_set_text_color(move_value, GColorWhite);
  text_layer_set_text_color(next_move_name, GColorWhite);
  text_layer_set_text_color(next, GColorWhite);

  text_layer_set_text(paused_text, "Rep Move");

  action_bar_layer_set_click_config_provider(move_controls, reps_move_controls_click_config_provider);
}

void timer_move_controls_click_config_provider(void* context) {
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) next_move_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) stop_move_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) pause_play_move_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, (ClickHandler) back_click_handler);
}
void reps_move_controls_click_config_provider(void* context) {
  window_long_click_subscribe(BUTTON_ID_DOWN, 1000, (ClickHandler) next_move_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) next_rep_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) stop_move_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) next_rep_click_handler);  
  window_single_click_subscribe(BUTTON_ID_BACK, (ClickHandler) back_click_handler);
}

//Click Handlers
void next_rep_click_handler(ClickRecognizerRef recognizer, void *context) {
  move_rep_next(move);
}
void next_move_click_handler(ClickRecognizerRef recognizer, void *context) {
  LOG("Next Button clicked");
  move_finish(move);
}
void stop_move_click_handler(ClickRecognizerRef recognizer, void *context) {
  move_stop(move);
  workout_stop(move->workout);
  hide_win_move();
}
void pause_play_move_click_handler(ClickRecognizerRef recognizer, void *context) {
  LOG("Pause button clicked");
  LOG("Move name: %s", move->name);

  move_timer_pause_or_resume(move);
}

void back_click_handler(ClickRecognizerRef recognizer, void *context) { 
  LOG("Back button clicked"); 
  hide_win_main(); 
  workout_save_current_move(move); 
  move_stop(move); 
  hide_win_move();
} 



static void handle_window_unload(Window* window) {
  status_bar_layer_destroy(status_bar);
  destroy_ui();
}

/**
* Set the value of the moved displayed in the window 
* @param char* move_value: value to show to the user
*/
void win_move_set_value(char * new_value) {
  LOG("Winmove set value");
  text_layer_set_text(move_value, new_value);
}

/**
* Displays given move name to users as next move
* @param char* name: move to display. 
*/
void win_move_set_next_move_name(char* name){
  text_layer_set_text(next_move_name, name);

  //center next move
  Layer *window_layer = window_get_root_layer(s_window);
  int16_t window_size = layer_get_bounds(window_layer).size.w - ACTION_BAR_WIDTH;
  int16_t next_size   = text_layer_get_content_size(next).w;
  int16_t move_size   = text_layer_get_content_size(next_move_name).w;

  int16_t next_margin = (window_size - (next_size + move_size))/2;
  int16_t move_margin = next_margin + next_size + NEXT_PADDING;

  GRect old_next_frame = layer_get_frame(text_layer_get_layer(next));
  GRect old_move_frame = layer_get_frame(text_layer_get_layer(next_move_name));

  GRect new_next_frame = GRect(next_margin, old_next_frame.origin.y, old_next_frame.size.w, old_next_frame.size.h);
  GRect new_move_frame = GRect(move_margin, old_move_frame.origin.y, old_move_frame.size.w, old_move_frame.size.h);

  layer_set_frame(text_layer_get_layer(next), new_next_frame);
  layer_set_frame(text_layer_get_layer(next_move_name), new_move_frame);

}

void initialize_custom_ui() {
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_overflow_mode(move_name, GTextOverflowModeWordWrap);
  status_bar = status_bar_layer_create();
}

void show_win_move(void) {
  initialise_ui();
  initialize_custom_ui();
  layer_add_child(window_get_root_layer(s_window), status_bar_layer_get_layer(status_bar));
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_win_move(void) {
  window_stack_remove(s_window, true);
}

/**
* Show paused message to the user
*/
void win_move_show_pause() {
  text_layer_set_text(paused_text, "Paused");
}

/**
* Hide paused message from user
*/ 
void win_move_hide_pause(){
  text_layer_set_text(paused_text, "\0"); //Empty String
}


