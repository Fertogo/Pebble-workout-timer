//Custom Workout Timer for Pebble. v 3.0
//By Fernando Trujano
//    trujano@mit.edu
// 04/29/2015

#include "pebble.h"
#include<stdlib.h>
#include<string.h> 
  
#define WAKEUP_REASON 0
#define PERSIST_KEY_WAKEUP_ID 337
#define PERSIST_KEY_WAKEUP_NAME 338
#define PERSIST_KEY_WAKEUP_TYPE 339
#define PERSIST_KEY_WAKEUP_VALUE 340

#define PERSIST_PAUSE_KEY 341
#define PERSIST_NEXT_MOVE_KEY 342
#define PERSIST_NEXT_MOVE_TIME_KEY 343
#define PERSIST_NEXT_MOVE_TYPE_KEY 344

  
//Message Types
#define DONE_KEY 0
#define WORKOUT_KEY 1
#define RESUME_KEY 2
#define RESTORE_KEY 3
  
static Window *window;
static TextLayer *text_layer;
char * currentMoveType; 
static AppTimer *timer;
static MenuLayer *menu_layer;

char nextMoveName[100]; 
char nextMoveTime[5];
char nextMoveType[6];

char upNext[110];
bool nextMoveLoaded = false; 
bool lastMove = false; 

bool jsReady = false; 
bool needRestore = false; //Whether the phone needs to restore the workout before advancing

void sendMessage(int , char*); 
void showInstructions();
void showEndCard(); 
void createTimer(); 
void finishWorkout(); 
void advanceToMove(); 
void advanceToNextMove(); 

void createReps();

char* strtok(); 

static Window *ins_window; //Instructions Window

static WakeupId s_wakeup_id;
  
#define NUM_MENU_SECTIONS 1
int NUM_FIRST_MENU_ITEMS = 0 ; //# of workout saved by user (Key 1 on internal storage) // Set on main 
bool instructions = false; 

char * workout_names[20]; //Save up to 20 workouts

/* Reads a string from the Pebble Internal storage
*  @param key Key under which desired file was saved
*  @return A string representing the contents of the file. 
*          returns string "Error" if unable to find file.
*/
char *readFromStorage(int key) { 
  char * total; 
  total = "Error"; 
  if (persist_exists(key)){ 
    persist_read_string(key, total, persist_get_size(key));
    char * s = total;  
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Read From Storage: Key: %i , Value: %s", key,s);
    return s; 
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Reading from storage: Key %i not found", key);
  return "Error";  
}

/*
* Keeps waiting to send a message until JS on the phone is ready
* @param data Data to be send once phone is ready in format ("type,message")
*/

static AppTimer *jsReadyTimer;
static void jsReadyTimer_callback(void *data){ 
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Trying to send message, phone isn't ready yet!");

    //Wait until JS is ready
    if (jsReady) { 
      int type; 
      char * message;
      type = atoi(data);
      message = strchr(data, ',') + sizeof(char);
      sendMessage(type,message);  
    }
  else { //Wait 1/10 seconds
    jsReadyTimer = app_timer_register(100 /* milliseconds */, jsReadyTimer_callback, data);  
  }
  
}

void sent_message_callback(DictionaryIterator *iterator, void *context){ 
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Message actually sent");
}

void failed_message_callback(DictionaryIterator *iterator, AppMessageResult reason,  void *context){ 
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Message failed to send! %i ", reason);
}

void dropped_message_callback(AppMessageResult reason,  void *context){ 
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Message dropped!:  %i ", reason);
}

/*
* Sends a message to the phone. Does not send the message until phone is ready to recieve.
* @param type Type of message being sent
* @param message Message to send to the phone
*/
char data[80];
void sendMessage(int type, char* message) { 

  //Block until JS is ready 
  if (!jsReady){
    char strType[2]; 
    snprintf(strType,10, "%d", type); 
    strcpy(data, strType);
    strcat(data, ","); 
    strcat(data, message);  //Put data in "type,message" format
  
    jsReadyTimer = app_timer_register(1 /* milliseconds */ , jsReadyTimer_callback, data);  
  }
  
 else {  
   APP_LOG(APP_LOG_LEVEL_DEBUG,"Sending message... %s %i", message, type);
   app_message_register_outbox_sent(sent_message_callback);
   app_message_register_outbox_failed(failed_message_callback);
   app_message_register_inbox_dropped(dropped_message_callback); 
   DictionaryIterator *iter;
   app_message_outbox_begin(&iter);
   dict_write_cstring(iter, type, message);
   int result = app_message_outbox_send();
   switch (result){ 
      case APP_MSG_OK:  
        APP_LOG(APP_LOG_LEVEL_DEBUG,"OK");
        APP_LOG(APP_LOG_LEVEL_DEBUG,"Message \"%s\" of type %i sent to phone", message, type);
        break; 
      case APP_MSG_BUSY:  //There are pending messages (in or out) that need to be processed first, try resending in a bit
        APP_LOG(APP_LOG_LEVEL_DEBUG,"BUSY");
        char strType[2]; 
        snprintf(strType,10, "%d", type); 
        strcpy(data, strType);
        strcat(data, ","); 
        strcat(data, message);  //Put data in "type,message" format
        jsReadyTimer = app_timer_register(100 , jsReadyTimer_callback, data);  
      break; 
   }
 }
}


/*
* Clears all workouts and perist keys from Pebble's Internal Memory. 
*/
void clearMemory() { 
    for (int i =0; i<NUM_FIRST_MENU_ITEMS; i++ ) { 
      persist_delete(i); //Delete workout
    }
    persist_delete(PERSIST_KEY_WAKEUP_ID);
    persist_delete(PERSIST_KEY_WAKEUP_NAME);
    persist_delete(PERSIST_KEY_WAKEUP_TYPE);
    persist_delete(PERSIST_KEY_WAKEUP_VALUE);

    persist_delete(PERSIST_PAUSE_KEY);
    persist_delete(PERSIST_NEXT_MOVE_KEY);
    persist_delete(PERSIST_NEXT_MOVE_TIME_KEY);
      persist_delete(PERSIST_NEXT_MOVE_TYPE_KEY);

    APP_LOG(APP_LOG_LEVEL_DEBUG,"Memory Cleared Successfully");
    NUM_FIRST_MENU_ITEMS = 0; 
    persist_write_string(0,"0");
}

/*
* Redraws the main menu and send a short pulse vibe
*/
void updateMenu(){ 
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Updating Menu...");
  NUM_FIRST_MENU_ITEMS = atoi(readFromStorage(0));
  APP_LOG(APP_LOG_LEVEL_DEBUG,"NUM_FIRST_MENU_ITEMS= %i", NUM_FIRST_MENU_ITEMS);
  //Populate workout_names array
  for (int i = 0; i<NUM_FIRST_MENU_ITEMS; i++) { 
    char * temp = readFromStorage(i+1); 
    //workout_names[i] =   temp;   // This does not work because of issues with pointers. (Every element becomes the last)
    if (workout_names[i]) free(workout_names[i]); //Avoid memory leaks
    workout_names[i]= malloc(sizeof(char)*(strlen(temp)+1)); // Save workout titles
    strcpy(workout_names[i],  temp);
  }  
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Done with for loop! ");
  menu_layer_reload_data(menu_layer); //Reload the menu 
  vibes_short_pulse(); 
}


static Window *window;
static Window *loading_window; 

//Stuff for Menu **** 
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT; //Default Height
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      menu_cell_basic_header_draw(ctx, cell_layer, "Add timers from phone!");
      break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
 // APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing Menu...");
  switch (cell_index->section) {
    case 0:  
    //Replacing switches with for loop to dynamically add items
    for (int i=0; i<NUM_FIRST_MENU_ITEMS; i++){ 
      if (cell_index->row == i ){     
          char *s = workout_names[i];//Get Workout Title  
          menu_cell_basic_draw(ctx, cell_layer, s, "Click to start workout!", NULL);
      }
    }  
    break;    
   } 
}

// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  switch (cell_index->section) {
    case 0:
      for (int i=0; i<NUM_FIRST_MENU_ITEMS; i++){ 
          if (cell_index->row == i ){ 
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Button Clicked: %i", i);
            char *workout = readFromStorage(i+1);//Get Workout Title
            sendMessage(WORKOUT_KEY , workout); 
            
            //clearMemory(); 
            window_stack_push(loading_window, true); 
            window_stack_remove(window, false); 
          }
      }  
      break; 
  } 
}

void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  menu_layer = menu_layer_create(bounds);

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
// End menu stuff

static TextLayer *loading_text; 
void loading_window_load(Window *loading_window){ 
      APP_LOG(APP_LOG_LEVEL_DEBUG,"Loading Window Load"); 

   Layer *loading_window_layer = window_get_root_layer(loading_window);
    GRect bounds = layer_get_frame(loading_window_layer);

    loading_text = text_layer_create(GRect(0, 10, bounds.size.w /* width */, 80 /* height */));
    if (bluetooth_connection_service_peek()) text_layer_set_text(loading_text, "Loading...");
  
    else text_layer_set_text(loading_text, "Please connect Pebble to Phone then try again.");
    text_layer_set_font(loading_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(loading_text, GTextAlignmentCenter);
    layer_add_child(loading_window_layer, text_layer_get_layer(loading_text));
}

void loading_window_unload(Window *loading_window){ 

}

int timer_time = 0; 
static TextLayer *timer_text; 
static TextLayer *paused_text; 
char * time_str = "";

static void time_window_disappear(Window *window){ 
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Timer Window Disappeared"); 
}

/*
* Keeps track of the main workout timer
* Called every one second when a timer is running
*
*/
static void timer_callback(void *data) {
//      APP_LOG(APP_LOG_LEVEL_DEBUG,"Restore?: %d", needRestore);
    //if (persist_exists(PERSIST_KEY_WAKEUP_ID))   APP_LOG(APP_LOG_LEVEL_DEBUG,"WAKEUP WILL HAPPEN");

    if (timer_time==0) { 
      advanceToNextMove(); 
      //vibes_long_pulse(); //Vibrate Pebble 
      vibes_short_pulse();
    } 
    else { 
      timer_time--;    
      //Convert int to string 
      snprintf(time_str, 10, "%d", timer_time);
      text_layer_set_text(timer_text, time_str); //Update the time
      APP_LOG(APP_LOG_LEVEL_DEBUG,"Time left: %s",time_str);
      //APP_LOG(APP_LOG_LEVEL_DEBUG,"One Second!");
      timer = app_timer_register(1000 /* milliseconds */, timer_callback, NULL);
    }
}

//Pause the timer 
bool paused = false; 
void pause_click_handler(ClickRecognizerRef recognizer, void *context) { 
  
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Pause Button clicked"); 
  if (paused) { 
    persist_delete(PERSIST_PAUSE_KEY); 
    text_layer_set_text(paused_text, "\0"); //Empty String
    timer = app_timer_register(1000 /* milliseconds */, timer_callback, NULL);
    paused = false; 
    
  } 
  else { 
    app_timer_cancel(timer);
   // wakeup_cancel(s_wakeup_id); 
    persist_write_int(PERSIST_PAUSE_KEY, timer_time);

    text_layer_set_text(paused_text, "Paused");
    paused = true; 
  }
}

//Cancels the current timer
void stop_click_handler(ClickRecognizerRef recognizer, void *context) { 
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Stop Button clicked");
    paused = false; 
    persist_delete(PERSIST_PAUSE_KEY); 
    app_timer_cancel(timer);
    
  wakeup_cancel(s_wakeup_id); 
    window_stack_pop(true); 
    window_stack_push(window, false); 
}

//Cancels the current timer and goes to the next workout
void next_click_handler(ClickRecognizerRef recognizer, void *context) { 
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Next Button clicked");
  advanceToNextMove(); 
}

//Schedule the wakup.
static TextLayer *title_text; 

void timer_back_click_handler(ClickRecognizerRef recognizer, void *context){ 
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Back Button clicked");

  window_stack_pop(true); 
  time_t future_time = time(NULL) + atoi(text_layer_get_text(timer_text));
  s_wakeup_id = wakeup_schedule(future_time, 0, true); //Create Wakeup Timer
  persist_write_int(PERSIST_KEY_WAKEUP_ID, s_wakeup_id); // Save wakeup id! 
  persist_write_string( PERSIST_KEY_WAKEUP_NAME, text_layer_get_text(title_text));  //Save move name  
  persist_write_string( PERSIST_KEY_WAKEUP_TYPE, "time"); 

}

void rep_back_click_handler(ClickRecognizerRef recognizer, void *context){ 
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Back Rep Button clicked");
  window_stack_pop(true);
  persist_write_string( PERSIST_KEY_WAKEUP_NAME, text_layer_get_text(title_text));  //Save move name  
  persist_write_string( PERSIST_KEY_WAKEUP_TYPE, "reps"); 
  persist_write_string( PERSIST_KEY_WAKEUP_VALUE, text_layer_get_text(timer_text));  //Save move name  

}

void next_rep_click_handler(ClickRecognizerRef recognizer, void *context) { 
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Next Rep Button clicked");
  int repsLeft = atoi(text_layer_get_text(timer_text)) -1; 
  if (repsLeft >= 1){  
      snprintf(time_str, 10, "%d", repsLeft); 
      text_layer_set_text(timer_text, time_str); //Update reps
  }
  else advanceToNextMove(); 
}

void timerwindow_click_config_provider(void *context){ 
    window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, pause_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 1000, stop_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 1000, next_click_handler);
    window_single_click_subscribe(BUTTON_ID_BACK, timer_back_click_handler);
}

void repwindow_click_config_provider(void *context){ 
    window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, next_rep_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 1000, stop_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 1000, next_rep_click_handler);
    window_long_click_subscribe(BUTTON_ID_DOWN, 500, next_click_handler, NULL);
    window_single_click_subscribe(BUTTON_ID_BACK, rep_back_click_handler);
}

//Create Timer Function 
static Window *timer_window; 
// static TextLayer *title_text; 
static TextLayer *next_move_text; 

ActionBarLayer *action_bar;
static GBitmap *stopButton;
static GBitmap *playPauseButton;
static GBitmap *nextButton;
#ifdef PBL_PLATFORM_APLITE 
  InverterLayer * inverterLayer; 
#endif

void timer_window_init(){ 

       timer_window = window_create(); 
      window_set_window_handlers(timer_window, (WindowHandlers) {
        .disappear = time_window_disappear,
      });
        APP_LOG(APP_LOG_LEVEL_DEBUG,"Loading Timer Window");

      Layer *timer_window_layer = window_get_root_layer(timer_window);

    GRect bounds = layer_get_frame(timer_window_layer);

    title_text = text_layer_create(GRect(0, 10, bounds.size.w /* width */, 60 /* height */));
  
    text_layer_set_font(title_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));    

    text_layer_set_text_alignment(title_text, GTextAlignmentCenter);
    text_layer_set_overflow_mode(title_text, GTextOverflowModeWordWrap);
    layer_add_child(timer_window_layer, text_layer_get_layer(title_text));

    timer_text = text_layer_create(GRect(0, 60, bounds.size.w /* width */, 30 /* height */));
    text_layer_set_font(timer_text, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
    text_layer_set_text_alignment(timer_text, GTextAlignmentCenter);
    layer_add_child(timer_window_layer, text_layer_get_layer(timer_text));
  
    paused_text = text_layer_create(GRect(0, 120, bounds.size.w /* width */, 30 /* height */));
    text_layer_set_font(paused_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(paused_text, GTextAlignmentCenter);
    layer_add_child(timer_window_layer, text_layer_get_layer(paused_text));
  
    next_move_text = text_layer_create(GRect(0, 100, bounds.size.w /* width */, 30 /* height */));
    text_layer_set_font(next_move_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(next_move_text, GTextAlignmentCenter);
    layer_add_child(timer_window_layer, text_layer_get_layer(next_move_text));
  
    action_bar = action_bar_layer_create(); 
    action_bar_layer_add_to_window(action_bar, timer_window);

    stopButton = gbitmap_create_with_resource(RESOURCE_ID_STOP_BUTTON);
    playPauseButton = gbitmap_create_with_resource(RESOURCE_ID_PLAY_PAUSE_BUTTON);
    nextButton = gbitmap_create_with_resource(RESOURCE_ID_NEXT_BUTTON);
    action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, stopButton);
    action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, nextButton);

  #ifdef PBL_PLATFORM_APLITE 
    inverterLayer =  inverter_layer_create(bounds);
    layer_add_child(timer_window_layer, inverter_layer_get_layer(inverterLayer));
  #endif

}

char nameCopy[100]; 
char repsCopy[5];
/*
* Creates the main timer Window and starts the timer
* @param name Name of the move to be displayed
* @param char* representing the number of seconds for that move
* @param bool true if next workout should be reps, false if time
* @param bool true if next mvoe should be requested
*/
void createTimer(char* name, char* time, int reps, int getNext) { 

   strcpy(nameCopy, name);
    name = nameCopy;
      APP_LOG(APP_LOG_LEVEL_DEBUG,"NAME: %s",nameCopy);
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Creating Timer with name: %s and time: %s",name, time);
  
    window_stack_remove(timer_window, false); 
  
    window_stack_remove(loading_window, false); 
  
    window_stack_push(timer_window, true);
    Layer *timer_window_layer = window_get_root_layer(timer_window);

    text_layer_set_text(title_text, name);
    text_layer_set_text(timer_text, "");
    text_layer_set_text(paused_text, "\0"); //Empty String
    text_layer_set_text(next_move_text, "\0"); //Empty String

    if (getNext) sendMessage(DONE_KEY, ""); //load the next move

    if (reps){ 
      createReps(name, time); 
      return;
    }
  
    else { 
        //Set ActionBar
      #ifdef PBL_PLATFORM_APLITE 
        layer_set_hidden(inverter_layer_get_layer(inverterLayer), true); 
      #endif
        action_bar_layer_set_click_config_provider(action_bar, timerwindow_click_config_provider);
        action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, playPauseButton);

        timer_time = atoi(time);
        if (paused) { 
              text_layer_set_text(paused_text, "Paused");
        }
        else { 
          timer = app_timer_register(1 /* milliseconds */, timer_callback, NULL);  
        }  
    }
}

void createReps(char* name, char* reps) { 
    strcpy(repsCopy,reps);
    reps = repsCopy;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Looks like I got some reps! ");
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Creating Reps with name: %s and reps: %s",name, reps);
    text_layer_set_text(timer_text, reps);
    text_layer_set_text(paused_text, "Rep Mode"); 
  
    #ifdef PBL_PLATFORM_APLITE 
      layer_set_hidden(inverter_layer_get_layer(inverterLayer), false);
    #endif

    action_bar_layer_set_click_config_provider(action_bar, repwindow_click_config_provider);
    action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, NULL);
}

void window_unload(Window *window) {
  if (timer != NULL) app_timer_cancel(timer);
  // Destroy the menu layer
  menu_layer_destroy(menu_layer); 
} 

/*
* Adds a single workout to the Pebble. 
* @param name of the workout
*/
void addWorkout(char* title){ 

      APP_LOG(APP_LOG_LEVEL_DEBUG, "adding workout!");

      int totalworkouts = atoi(readFromStorage(0));
      NUM_FIRST_MENU_ITEMS = totalworkouts; //Update NUM_FIRST_MENU_ITEMS
      //Add message to storage and increase workout counter 
      persist_write_string(NUM_FIRST_MENU_ITEMS+1, title); // Add to Totalworkouts +1 
      APP_LOG(APP_LOG_LEVEL_DEBUG,"Message Added to Storage!");
      APP_LOG(APP_LOG_LEVEL_DEBUG,"I just added workout %s to index %i", title, NUM_FIRST_MENU_ITEMS+1); 

      if (instructions) { //First workout in list
          APP_LOG(APP_LOG_LEVEL_DEBUG, "Creating Menu Window");
          //Create Window 
          window = window_create(); 
          window_set_window_handlers(window, (WindowHandlers) {
            .load = window_load,
            .unload = window_unload,
          });
          window_stack_push(window, true /* Animated */);
          instructions = false; 
      }

      //Convert int to string
      char buffer[10];
      snprintf(buffer, 10, "%d", NUM_FIRST_MENU_ITEMS+1);  
      persist_write_string(0, buffer); //Increment workouts

      APP_LOG(APP_LOG_LEVEL_DEBUG,"Total Workouts %s", buffer); 

}

enum {
      TYPE_KEY,
      MESSAGE_KEY,
      MESSAGE2_KEY,
      MESSAGE3_KEY,
   };

static char* getFromDict(DictionaryIterator *iter, uint32_t key){ 
    Tuple *text_tuple = dict_find(iter, key);
    char *item; 
    item = "Error";
    if (text_tuple) {  
      item = text_tuple->value->cstring;
     }
    return item; 
}

//Recieve message from js
static void in_received_handler(DictionaryIterator *iter, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "WatchApp Recieved Message!"); 

    char * type = getFromDict(iter, TYPE_KEY);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Type: %s", type );

    char* message = getFromDict(iter, MESSAGE_KEY); 
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Text: %s", message );

    char *message2 = getFromDict(iter, MESSAGE2_KEY); 
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message 2 Text: %s", message2 );
  
    char *message3 = getFromDict(iter, MESSAGE3_KEY); 
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message 3 Text: %s", message3 );

  
    if (strcmp(type,"workouts") == 0) { 
        clearMemory(); 
    
        APP_LOG(APP_LOG_LEVEL_DEBUG, "adding workouts!");
        char * workouttitle;
  
        workouttitle = strtok (message,",");
        while (workouttitle != NULL){
          APP_LOG(APP_LOG_LEVEL_DEBUG, workouttitle);
          addWorkout(workouttitle); 
          workouttitle = strtok (NULL, ",");
        }  
      
        updateMenu(); 
        window_stack_remove(ins_window, false); 
    }
      
    else if (strcmp(type,"end") == 0) { 
      if (window_stack_get_top_window() == timer_window){   //Move is currently in progress
        lastMove = true; 
        text_layer_set_text(next_move_text, "Last Move!"); 
      }
      else finishWorkout(); 
    }
  
    else if (strcmp(type,"ready") == 0){ 
        APP_LOG(APP_LOG_LEVEL_DEBUG,"JS READY C KNOWS "); 
        jsReady = true; 
    }
  
    else if (strcmp(type, "move") == 0){ //It is a Timer. Type = "move", message = movename, message2 = duration, message3 = moveType
        APP_LOG(APP_LOG_LEVEL_DEBUG,"Timer message recieved with value %s", message); 
        
        if (window_stack_get_top_window() == timer_window){ //Move is currently in progress
          APP_LOG(APP_LOG_LEVEL_DEBUG,"================ Got Next Move: %s", message); 

          strcpy(nextMoveName, message);
          strcpy(nextMoveTime, message2);
          strcpy(nextMoveType, message3);

          nextMoveLoaded = true; 
                 
          persist_write_string( PERSIST_NEXT_MOVE_KEY, message); //Save next move
          persist_write_string( PERSIST_NEXT_MOVE_TIME_KEY, message2); //Save next move's time
          persist_write_string( PERSIST_NEXT_MOVE_TYPE_KEY, message3); //Save next move's time
     
          strcpy(upNext, "next: ");
          strcat(upNext, message);
          APP_LOG(APP_LOG_LEVEL_DEBUG,"NEXT : %s", upNext); 
          text_layer_set_text(next_move_text, upNext); 
        }
        else {
          char * moveName = message; 
          char * moveDuration = message2; 
          char * moveType = message3; 
          advanceToMove(moveName, moveDuration, moveType);           
      }           
    } 
 }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  

/**
* Advance current workout to specified move
* @param moveName name of the move to advance to
* @param moveDuration time (in seconds) of move duration
* @param getNext (boolean) true if Pebble should request the next workout from the phone
**/
void advanceToMove(char * moveName, char *moveDuration, char* moveType, int getNext){ 
    time_t future_time = time(NULL) + atoi(moveDuration);
    currentMoveType = moveType; 
    wakeup_cancel_all();
    vibes_long_pulse();
    int movetype =  (strcmp(moveType,"reps") == 0 );
    createTimer(moveName,moveDuration, movetype, getNext);
}

/**
* Advances to the next move in the workout 
**/
void advanceToNextMove(){ 
    APP_LOG(APP_LOG_LEVEL_DEBUG,"-- Advancing to next move --");
    paused = false; 

    persist_delete(PERSIST_NEXT_MOVE_KEY);
    persist_delete(PERSIST_NEXT_MOVE_TIME_KEY);  
    persist_delete(PERSIST_NEXT_MOVE_TYPE_KEY);  

    app_timer_cancel(timer);
    window_stack_pop(false); 
    window_stack_push(loading_window, true); 

    if (lastMove) {
      finishWorkout(); 
      lastMove = false; 
      return; 
      }

    if (needRestore) { 
      needRestore = false; 
      if (nextMoveLoaded)  sendMessage(RESUME_KEY, "true");
      else sendMessage(RESUME_KEY, "void");
    } 

    else if (nextMoveLoaded){
        nextMoveLoaded = false; 
        char * name = nextMoveName; 
        char * time = nextMoveTime;
        char * type = nextMoveType; 
        advanceToMove(name, time, type, true);         
    }
    light_enable_interaction(); 
}


static void wakeup_handler(WakeupId id, int32_t reason) {
  // The app has woken!
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Wakey, Wakey. %i", (int)reason);
  persist_delete(PERSIST_KEY_WAKEUP_ID);
  //psleep(300); 
  vibes_long_pulse(); //Vibrate Pebble 
  
  window_stack_push(loading_window, false); 
  
  if (persist_exists(PERSIST_NEXT_MOVE_KEY)){  
    APP_LOG(APP_LOG_LEVEL_DEBUG, "There was a move preloaded!");
    char * moveName = readFromStorage(PERSIST_NEXT_MOVE_KEY);
    strcpy(nextMoveName, moveName );
    strcpy(nextMoveTime, readFromStorage(PERSIST_NEXT_MOVE_TIME_KEY));
    strcpy(nextMoveType, readFromStorage(PERSIST_NEXT_MOVE_TYPE_KEY));

    nextMoveLoaded = true;  
  } //TODO Refactor
  if(nextMoveLoaded) sendMessage(RESUME_KEY, "true");
  else sendMessage(RESUME_KEY, "void");  
}

int main(void) {
  
    APP_LOG(APP_LOG_LEVEL_DEBUG,"C Code Started");
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    app_message_register_inbox_received((AppMessageInboxReceived) in_received_handler);
    // Subscribe to Wakeup API
    wakeup_service_subscribe(wakeup_handler);

    if (!persist_exists(0)) { 
          APP_LOG(APP_LOG_LEVEL_DEBUG,"First time using app");
          persist_write_string(0, "0"); // Set workouts to 0
    }

  //Menu data
    int totalworkouts = atoi(readFromStorage(0)); 
    NUM_FIRST_MENU_ITEMS = totalworkouts; 
    for (int i = 0; i<totalworkouts; i++) { //Populate workout_names array
      char * temp = readFromStorage(i+1); 
      //workout_names[i] =   temp;   // This does not work because of issues with pointers. (Every element becomes the last)
      workout_names[i]= malloc(sizeof(char)*(strlen(temp)+1)); // Save workout titles
      strcpy(workout_names[i],  temp);
    }  

        window = window_create(); 
      window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
      });
  
      loading_window = window_create(); 
      window_set_window_handlers(loading_window, (WindowHandlers) {
        .load = loading_window_load,
        .unload = loading_window_unload,
      }); 
  
     timer_window_init(); 

    //checkScheduledWakeup();
   if (totalworkouts == 0) { 
        showInstructions(); 
        instructions = true; 
   }
  
        // Was this a wakeup?
   else if(launch_reason() == APP_LAUNCH_WAKEUP) {
      // The app was started by a wakeup
      WakeupId id = 0;
      int32_t reason = 0;
  
      // Get details and handle the wakeup
      wakeup_get_launch_event(&id, &reason);
      wakeup_handler(id, reason);
   }
  
   else if (launch_reason() == APP_LAUNCH_USER ){
      // else if (persist_exists(PERSIST_KEY_WAKEUP_ID) && persist_read_int(PERSIST_KEY_WAKEUP_ID) > 0 ){ 
      APP_LOG(APP_LOG_LEVEL_DEBUG, "======LOOKS LIKE I NEED TO RESTORE ======");
  
      needRestore = true;
      // wakeup_cancel_all(); 
  
      if (persist_exists(PERSIST_NEXT_MOVE_KEY)){  
        APP_LOG(APP_LOG_LEVEL_DEBUG, "There was a move preloaded!");
        char * moveName = readFromStorage(PERSIST_NEXT_MOVE_KEY);
        strcpy(nextMoveName, moveName );
        strcpy(nextMoveTime, readFromStorage(PERSIST_NEXT_MOVE_TIME_KEY));
        strcpy(nextMoveType, readFromStorage(PERSIST_NEXT_MOVE_TYPE_KEY));

        nextMoveLoaded = true;  
      }
      
      char name[100];
      char type[6]; 
      char value[5];

         strcpy(name, readFromStorage(PERSIST_KEY_WAKEUP_NAME));
         strcpy(type, readFromStorage(PERSIST_KEY_WAKEUP_TYPE));

        APP_LOG(APP_LOG_LEVEL_DEBUG, "Move Name: %s", name);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Move Type: %s", type);


    
      if (persist_exists(PERSIST_PAUSE_KEY) && strcmp(type, "time") == 0){ 
        int time_left =persist_read_int(PERSIST_PAUSE_KEY); 
        snprintf(time_str, 10, "%d", time_left);
        advanceToMove(name, time_str, type, false); 
        pause_click_handler(NULL, NULL); 
      }
    
      else{ 
        s_wakeup_id = persist_read_int(PERSIST_KEY_WAKEUP_ID); 
        //There is a wakeup scheduled 
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Wakup already scheduled");
        
        time_t timestamp = 0;

        if (strcmp(type, "reps") == 0 ){ //Rep move was on
            strcpy(value, readFromStorage(PERSIST_KEY_WAKEUP_VALUE));
            advanceToMove(name,value,type,false);
        }
        else if (wakeup_query(s_wakeup_id, &timestamp) ) { //Time move was on 
           int seconds_remaining = timestamp - time(NULL);
           snprintf(time_str, 10, "%d", seconds_remaining);
           APP_LOG(APP_LOG_LEVEL_DEBUG, "Seconds remaining %i", seconds_remaining);
           APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", time_str);
           advanceToMove(name, time_str, type, false); 
        }
        else { 
          APP_LOG(APP_LOG_LEVEL_DEBUG, "SOMETHING WRONG WITH THE SAVED WAKEUP ");
          //finishWorkout(); //Should never get here, but I'll check anyways
          window_stack_push(window, true);
        }      
    }
  }
  
    else {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "NORMAL BOOT");
      window_stack_push(window, true);
    }
  
     //persist_delete(PERSIST_KEY_WAKEUP_ID);
    app_event_loop();
    text_layer_destroy(text_layer);
    window_destroy(window);
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Exiting..."); 

}

/**
* Finishes the current workout
*/
void finishWorkout(){ 
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Workout Finished");

    window_stack_remove(loading_window, false);
    window_stack_pop_all(false);
    window_stack_push(window, false); 

    showEndCard();      
    vibes_double_pulse(); 
}

/*
* Shows the instructions window to the user. 
*/
void showInstructions(){    
    static TextLayer *ins_text;
    ins_window = window_create(); 
    window_stack_push(ins_window, true);
    Layer *ins_window_layer = window_get_root_layer(ins_window);
    GRect bounds = layer_get_frame(ins_window_layer); 
    ins_text = text_layer_create(GRect(0, 0, bounds.size.w /* width */, 150 /* height */));
    text_layer_set_overflow_mode(ins_text, GTextOverflowModeWordWrap ); 
    text_layer_set_text(ins_text, "Use your phone to add workouts. On the pebble app, find this timer app and click on settings icon. ");
    text_layer_set_font(ins_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(ins_text, GTextAlignmentLeft);
    layer_add_child(ins_window_layer, text_layer_get_layer(ins_text)); 
}

/*
* Shows the end workout window to the user
*/
void showEndCard(){
    static Window *end_window; 
    static TextLayer *end_text;
    static TextLayer *end2_text;
    end_window = window_create(); 
    window_stack_push(end_window, true);
    Layer *end_window_layer = window_get_root_layer(end_window);
    GRect bounds = layer_get_frame(end_window_layer);

    end_text = text_layer_create(GRect(0, 10, bounds.size.w /* width */, 28 /* height */));
    text_layer_set_text(end_text, "Congratulations!");
    text_layer_set_font(end_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(end_text, GTextAlignmentCenter);
    layer_add_child(end_window_layer, text_layer_get_layer(end_text));

    end2_text = text_layer_create(GRect(0, 60, bounds.size.w /* width */, 30 /* height */));
    text_layer_set_text(end2_text, "Workout Finished");
    text_layer_set_font(end2_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(end2_text, GTextAlignmentCenter);
    layer_add_child(end_window_layer, text_layer_get_layer(end2_text));
}

//Minified Strtok (ignore) Thanks to Steve Caldwell for trick. 
char*strtok(s,delim)
register char*s;register const char*delim;{register char*spanp;register int c,sc;char*tok;static char*last;if(s==NULL&&(s=last)==NULL)
return(NULL);cont:c=*s++;for(spanp=(char*)delim;(sc=*spanp++)!=0;){if(c==sc)
goto cont;}
if(c==0){last=NULL;return(NULL);}
tok=s-1;for(;;){c=*s++;spanp=(char*)delim;do{if((sc=*spanp++)==c){if(c==0)
s=NULL;else
s[-1]=0;last=s;return(tok);}}while(sc!=0);}}


/*
* Message Protocol:
*   The phone sends the following messages to the Pebble
*      Type: "workouts" Data: str(move1,move2,move3...)  - Gives the Pebble the list of workout names that it should store
*      Type: "move" Data:(move, str(time), type) - Tells the Pebble the move name and duration for the next move as well as the type (time or reps)
*      Type: "end" - Tells the Pebble that there are no more moves in the workout. 
*      Type: "ready" - Tells the Pebble that the phone is ready to receive messages. 
*  The phone does not expect an imediate reply in any of these cases. It does expect a "done" message from Pebble after sending a (move, time) message. 
*/