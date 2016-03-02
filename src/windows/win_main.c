/* 
Main Window | Custom Workout Timer. v4.0
Displays saved workouts and requests them from phone when selected
Copyright © 2016 Fernando Trujano
                 trujano@mit.edu
*/

#include <pebble.h>
#include "common.h"

#include "win_main.h"
#include "message_helper.h"
#include "windows/win_instructions.h"

#include "storage.h"

void win_main_parse_workouts();
char * workouts_info[50];
int num_workouts = 0;

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static MenuLayer *menu_workouts;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, 0);
  #endif

  // menu_workouts
  menu_workouts = menu_layer_create(GRect(0, 0, 144, 168));
  menu_layer_set_click_config_onto_window(menu_workouts, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)menu_workouts);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(menu_workouts);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

// MENU CODE
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return num_workouts;
    default:
      return 0;
  }
}
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return MENU_CELL_BASIC_HEADER_HEIGHT;
      break;
    default:
      return MENU_CELL_BASIC_HEADER_HEIGHT;
  }
}
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      menu_cell_basic_header_draw(ctx, cell_layer, "Add timers from phone!");
      break;
  }
}
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

  switch(cell_index->section){
    case 0:
      menu_cell_basic_draw(ctx, cell_layer, workouts_info[cell_index->row], "TestMenu", NULL);
      break;
  }
}
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  LOG("Button Clicked: %i", cell_index->row);
  LOG("Workout requested %s", workouts_info[cell_index->row]);
  char* workout_title = workouts_info[cell_index->row];
  workout_request_workout(workout_title);
  menu_layer_reload_data(menu_workouts); //Reload the menu
  vibes_short_pulse();
}

/**
* Extra initialization for menu
*/ 
void initialize_menu(void) {
  #ifdef PBL_COLOR
     menu_layer_set_highlight_colors(menu_workouts,GColorCobaltBlue ,GColorWhite );
  #endif
  scroll_layer_set_shadow_hidden(menu_layer_get_scroll_layer(menu_workouts), false);
  menu_layer_set_callbacks(menu_workouts, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });
}

/**
* Checks to see if main window is ready to be shown. Otherwise, it shows approrpriate window.
*/
void win_main_init() {
  initialise_ui();
  if (storage_get_workout(0) == NULL) {
    show_win_instructions();
  }
  else show_win_main();
}

void show_win_main(void) {

  win_main_parse_workouts();
  initialize_menu();


  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_win_main(void) {
  window_stack_remove(s_window, true);
}

/**
* Refreshes data on the menu
*/
void win_main_refresh(void) {
  if (!window_is_loaded(s_window)){
    //This happens on first use when instruction window was shown instead.
    show_win_main();
    hide_win_instructions();
  }

  win_main_parse_workouts();
  menu_layer_reload_data(menu_workouts); //Reload the menu
}

/**
* Parses workouts stored in persistent storage for easy display by MenuLayer. 
*/
void win_main_parse_workouts() {
  LOG("Win main parse workout");

  int workout_index = 0;
  char * workoutInfo = storage_get_workout(workout_index);

  while (workoutInfo != NULL) {
    LOG("WorkoutInfo: %s", workoutInfo);

    if (workouts_info[workout_index]) free(workouts_info[workout_index]); //Avoid memory leaks

    workouts_info[workout_index]= malloc(sizeof(char)*(strlen(workoutInfo)+1)); // Save workout titles
    strcpy(workouts_info[workout_index],  workoutInfo);

    workout_index++;
    workoutInfo = storage_get_workout(workout_index);

  }
  num_workouts = workout_index;
}
