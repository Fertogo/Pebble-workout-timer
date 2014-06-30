//Custom Workout Timer for Pebble. By Fernando Trujano
//    trujano@mit.edu
// 6/30/2014

/* Todo: 
Save total # of workouts
Dynamically create menu items based on ^ 
Timer component (go to next, vibrate)

Ability to delete workouts
Pause button
*/

#include "pebble.h"
#include <stdlib.h> 
  
//Stuff for Menu **** 
#define NUM_MENU_SECTIONS 1
  




char *readFromStorage(int key) { 
  //Read from storage
  char * total; 
  total = "Error"; 
  if (persist_exists(key)){ 
    persist_read_string(key, total, 20);  
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Reading from storage");
    APP_LOG(APP_LOG_LEVEL_DEBUG,total);
    char * s = total;    
    return s; 
  }
  
  return "Error";
  
}
  

  
int NUM_FIRST_MENU_ITEMS = 0 ; //# of workout saved by user (Key 1 on internal storage)

static Window *window;

static MenuLayer *menu_layer;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;

    /* case 1:
      return NUM_SECOND_MENU_ITEMS; */

    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT; //Default Height
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "Add workouts from phone!");
      break;

   /* case 1:
      menu_cell_basic_header_draw(ctx, cell_layer, "One more");
      break; */
  }
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

  // Determine which section we're going to draw in
  switch (cell_index->section) {
    case 0:
      
    //Replacing switches with for loop to dynamically add items in the future
    for (int i=0; i<NUM_FIRST_MENU_ITEMS; i++){ 
      if (cell_index->row == i ){  
          menu_cell_basic_draw(ctx, cell_layer, "Basic Item", "With a subtitle", NULL);
      }
    }  
    break; 
    
    // Use the row to specify which item we'll draw
    /* switch (cell_index->row) {      
        case 0:
          // This is a basic menu item with a title and subtitle
          menu_cell_basic_draw(ctx, cell_layer, "Basic Item", "With a subtitle", NULL);
          break;

        case 1:
          menu_cell_basic_draw(ctx, cell_layer, "Icon Item", "Select to cycle", NULL);
          break;
      }
      break; */

   /* case 1:
      switch (cell_index->row) {
        case 0:
          // There is title draw for something more simple than a basic menu item
          menu_cell_title_draw(ctx, cell_layer, "Final Item");
          break;
      } */
   } 
}

// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
    switch (cell_index->section) {
    case 0:
  for (int i=0; i<NUM_FIRST_MENU_ITEMS; i++){ 
      if (cell_index->row == i ){ 
        //Convert int to string
        char buffer[10];
        snprintf(buffer, 10, "%d", i);  
        APP_LOG(APP_LOG_LEVEL_DEBUG, buffer);
        //Open workout #i
        char *s = readFromStorage(1);
        APP_LOG(APP_LOG_LEVEL_DEBUG, s ); 
      }
    }  
      break; 
    } 
}



void window_load(Window *window) {


  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}
// End menu stuf *******

static Window *window;

static TextLayer *text_layer;

static AppTimer *timer;

//Called when timer is finished
static void timer_callback(void *data) {
      APP_LOG(APP_LOG_LEVEL_DEBUG,"TIMER DONE!");
}

       enum {
            AKEY_NUMBER,
            AKEY_TEXT,
        };
        //Recieve message from js
        static void in_received_handler(DictionaryIterator *iter, void *context) {

          // Check for fields you expect to receive
          Tuple *text_tuple = dict_find(iter, AKEY_TEXT);
          // Act on the found fields received
          char *message; 
          message = "Error";
          if (text_tuple) {  
            message = text_tuple->value->cstring;
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Text: %s", message );
          }
          
          //Add message to storage     
          persist_write_string(1, message);
          APP_LOG(APP_LOG_LEVEL_DEBUG,"Message Added to Storage!");
        }

void window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(menu_layer); } 


int main(void) {
    APP_LOG(APP_LOG_LEVEL_DEBUG,"C Code Started for real");
    NUM_FIRST_MENU_ITEMS += 3; 
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    app_message_register_inbox_received((AppMessageInboxReceived) in_received_handler);
  
  /*
  window = window_create();
  window_stack_push(window, true  );
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  text_layer = text_layer_create(bounds);
  text_layer_set_text(text_layer, "Waiting for timer...");
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer)); */
  
  window = window_create(); 
  // Setup the window handlers
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true /* Animated */);

  timer = app_timer_register(1500 /* milliseconds */, timer_callback, NULL);
  // A timer can be canceled with `app_timer_cancel()`
  
    //Send Message
  /*
  app_message_register_inbox_received(in_received_handler);					 
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());	
	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
 	Tuplet value = TupletInteger(1, 1);
 	dict_write_tuplet(iter, &value);
 	app_message_outbox_send();
  */
  
  //Read from storage


  app_event_loop();

  text_layer_destroy(text_layer);
  window_destroy(window);
}