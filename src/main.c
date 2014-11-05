//Custom Workout Timer for Pebble. v 2.0
//By Fernando Trujano
//    trujano@mit.edu
// 7/29/2014

#include "pebble.h"
#include<stdlib.h>
#include<string.h> 
  
#define WAKEUP_REASON 0
#define PERSIST_KEY_WAKEUP_ID 337
#define PERSIST_KEY_WAKUP_NAME 338
  
static Window *window;
static TextLayer *text_layer;
static AppTimer *timer;
static MenuLayer *menu_layer;


static WakeupId s_wakeup_id;
  
#define NUM_MENU_SECTIONS 1
int NUM_FIRST_MENU_ITEMS = 0 ; //# of workout saved by user (Key 1 on internal storage) // Set on main 
bool instructions = false; 

char * workout_names[20]; //Save up to 20 workouts

char *readFromStorage(int key) { 
  char * total; 
  total = "Error"; 
  if (persist_exists(key)){ 
    persist_read_string(key, total, 254); //Max Size  //Optimize
    char * s = total;  
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Read From Storage: Key: %i , Value: %s", key,s);
    return s; 
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Reading from storage: Key %i not found", key);
  return "Error";  
}

void sendMessage(char* message) { 
  DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
 	dict_write_cstring(iter,1, message);
 	app_message_outbox_send();
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Message \"%s\" sent to phone", message);
}

void clearMemory() { 
    for (int i =0; i<NUM_FIRST_MENU_ITEMS; i++ ) { 
      persist_delete(i); //Delete workout
    }
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Memory Cleared Successfully");
    NUM_FIRST_MENU_ITEMS = 0; 
    persist_write_string(0,"0");
}

void updateMenu(){ 
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Updating Menu...");
  NUM_FIRST_MENU_ITEMS = atoi(readFromStorage(0));
  APP_LOG(APP_LOG_LEVEL_DEBUG,"NUM_FIRST_MENU_ITEMS= %i", NUM_FIRST_MENU_ITEMS);
  //Populate workout_names array
  for (int i = 0; i<NUM_FIRST_MENU_ITEMS; i++) { 
    char * temp = readFromStorage(i+1); 
    //workout_names[i] =   temp;   // This does not work because of issues with pointers. (Every element becomes the last)
    workout_names[i]= malloc(sizeof(char)*(strlen(temp)+1)); // Save workout titles
    strcpy(workout_names[i],  temp);
  }  
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Done with for loop! ");
  menu_layer_reload_data(menu_layer); //Reload the menu 
  vibes_short_pulse(); 
}


static Window *window;

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
            sendMessage(workout); 
            //clearMemory(); 
          }
      }  
      break; 
  } 
}

//When user long clicks on a menu Item - aka deletes item
void menu_long_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  switch (cell_index->section) {
    case 0:
      for (int i=0; i<NUM_FIRST_MENU_ITEMS; i++){ 
          if (cell_index->row == i ){  
            //deleteFromStorage(i+1); //Disabled until deleteFromStorage is updated to delete from server too. 
            //updateMenu(); 
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
    .select_long_click = menu_long_select_callback, 
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}
// End menu stuff

int timer_time = 0; 
static TextLayer *timer_text; 
static TextLayer *paused_text; 
char * time_str = "";

static void time_window_disappear(Window *window){ 
  // Cancel the timer
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Timer Window Disappeared"); 
  app_timer_cancel(timer); 
  //Save Wakeup Id
 // persist_write_string(0, "0"); // Set workouts to 0

}
//Called every one second
static void timer_callback(void *data) {
      if (timer_time==0) { 
        persist_delete(PERSIST_KEY_WAKEUP_ID);
        wakeup_cancel(s_wakeup_id); 
        vibes_long_pulse(); //Vibrate Pebble 
        window_stack_pop(false); 
        sendMessage("done"); //Go to next workout, if possible
        
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
    text_layer_set_text(paused_text, "\0"); //Empty String
    timer = app_timer_register(1000 /* milliseconds */, timer_callback, NULL);
    paused = false; 
  } 
  else { 
    app_timer_cancel(timer);
    text_layer_set_text(paused_text, "Paused");
    paused = true; 
  }
}

void timerwindow_config_provider(Window * window){ 
    window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, pause_click_handler);
}
//Create Timer Function 
static Window *timer_window; 
static TextLayer *title_text; 



void createTimer(char* name, char* time) {  //Creates Timer window
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Creating Timer with name: %s and time: %s",name, time); 
    
    timer_window = window_create(); 
      window_set_window_handlers(timer_window, (WindowHandlers) {
        .disappear = time_window_disappear,
      });
  
    window_set_click_config_provider(timer_window, (ClickConfigProvider) timerwindow_config_provider);
    window_stack_push(timer_window, true);
    Layer *timer_window_layer = window_get_root_layer(timer_window);
    GRect bounds = layer_get_frame(timer_window_layer);

    title_text = text_layer_create(GRect(0, 10, bounds.size.w /* width */, 28 /* height */));
    text_layer_set_text(title_text, name);
    text_layer_set_font(title_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(title_text, GTextAlignmentCenter);
    layer_add_child(timer_window_layer, text_layer_get_layer(title_text));
  
    timer_text = text_layer_create(GRect(0, 60, bounds.size.w /* width */, 30 /* height */));
    text_layer_set_text(timer_text, "");
    text_layer_set_font(timer_text, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
    text_layer_set_text_alignment(timer_text, GTextAlignmentCenter);
    layer_add_child(timer_window_layer, text_layer_get_layer(timer_text));
  
    paused_text = text_layer_create(GRect(0, 120, bounds.size.w /* width */, 30 /* height */));
    text_layer_set_text(paused_text, "\0"); //Empty String
    text_layer_set_font(paused_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(paused_text, GTextAlignmentCenter);
    layer_add_child(timer_window_layer, text_layer_get_layer(paused_text));
  

    timer_time = atoi(time);
  

    timer = app_timer_register(1 /* milliseconds */, timer_callback, NULL);  


  

  
}

void window_unload(Window *window) {
  if (timer != NULL) app_timer_cancel(timer);
  // Destroy the menu layer
  menu_layer_destroy(menu_layer); 
} 

enum {
      AKEY_NUMBER,
      AKEY_TEXT,
   };

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

//Minified Strtok (ignore) Thanks to Steve Caldwell for trick. 
char*strtok(s,delim)
register char*s;register const char*delim;{register char*spanp;register int c,sc;char*tok;static char*last;if(s==NULL&&(s=last)==NULL)
return(NULL);cont:c=*s++;for(spanp=(char*)delim;(sc=*spanp++)!=0;){if(c==sc)
goto cont;}
if(c==0){last=NULL;return(NULL);}
tok=s-1;for(;;){c=*s++;spanp=(char*)delim;do{if((sc=*spanp++)==c){if(c==0)
s=NULL;else
s[-1]=0;last=s;return(tok);}}while(sc!=0);}}


//Recieve message from js
static void in_received_handler(DictionaryIterator *iter, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "WatchApp Recieved Message!"); 
  
    // Check for type of message
    Tuple *text_tuple2 = dict_find(iter, AKEY_NUMBER);
    char *type; 
    type = "Error";
    if (text_tuple2) {  
      type = text_tuple2->value->cstring;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Type: %s", type );
     }
    // Check for fields you expect to receive
    Tuple *text_tuple = dict_find(iter, AKEY_TEXT);
    // Act on the found fields received
    char *message; 
    message = "Error";
    if (text_tuple) {  
      message = text_tuple->value->cstring;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Text: %s", message );
     }
   
    if (strcmp(type,"workouts") == 0) { 
        clearMemory(); 
    
        APP_LOG(APP_LOG_LEVEL_DEBUG, "adding workouts!");
        char * workouttitle;
  
        workouttitle = strtok (message,",");
        while (workouttitle != NULL)
        {
          APP_LOG(APP_LOG_LEVEL_DEBUG, workouttitle);
          addWorkout(workouttitle); 
          workouttitle = strtok (NULL, ",");
        }  

      updateMenu(); 
    }
      
    else if (strcmp(type,"end") == 0) { 
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Workout Finished");
        //Show end Card
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
      
        vibes_double_pulse(); 
    }
  
    else { //It is a Timer. Type = Title, message = duration
        APP_LOG(APP_LOG_LEVEL_DEBUG,"Timer message recieved with value %s", message); 
      
        time_t future_time = time(NULL) + atoi(message);
        s_wakeup_id = wakeup_schedule(future_time, 0, true); //Create Wakeup Timer
        persist_write_int(PERSIST_KEY_WAKEUP_ID, s_wakeup_id); // Save wakeup id! 
        persist_write_string( PERSIST_KEY_WAKUP_NAME, type);  //Save workout name
        createTimer(type,message);  
    } 
 }

static void wakeup_handler(WakeupId id, int32_t reason) {
  // The app has woken!
  APP_LOG(APP_LOG_LEVEL_DEBUG,"WAkey wakey, kate is a loser %i", (int)reason);
  persist_delete(PERSIST_KEY_WAKEUP_ID);
  //persist_delete(PERSIST_KEY_WAKEUP_NAME);
  vibes_long_pulse(); //Vibrate Pebble 
  sendMessage("done"); //Go to next workout, if possible
}

static void checkScheduledWakeup(void){ 

  if (persist_exists(PERSIST_KEY_WAKEUP_ID) && persist_read_int(PERSIST_KEY_WAKEUP_ID) > 0){ 
      s_wakeup_id = persist_read_int(PERSIST_KEY_WAKEUP_ID); 
    //There is a wakeup scheduled 
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Wakup already scheduled");
    time_t timestamp = 0;
    wakeup_query(s_wakeup_id, &timestamp);
    int seconds_remaining = timestamp - time(NULL);
    snprintf(time_str, 10, "%d", seconds_remaining);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Seconds remaining %i", seconds_remaining);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", time_str);
    char* name = readFromStorage(PERSIST_KEY_WAKUP_NAME); 
    createTimer(name, time_str); 
  }
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

  
   
  /* //Print Memory for debugging 
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Printing Memory");   
  int i = 0; 
  while (persist_exists(i)){ 
         APP_LOG(APP_LOG_LEVEL_DEBUG,"Memory: %i, Value: %s ", i,readFromStorage(i));   
        i++; 
  } 
  */
  
  if (totalworkouts == 0) { 
        //Show Instructions
        static Window *ins_window; 
        static TextLayer *ins_text;
        ins_window = window_create(); 
        window_stack_push(ins_window, true);
        Layer *ins_window_layer = window_get_root_layer(ins_window);
        GRect bounds = layer_get_frame(ins_window_layer); 
        ins_text = text_layer_create(GRect(0, 0, bounds.size.w /* width */, 150 /* height */));
        text_layer_set_overflow_mode(ins_text, GTextOverflowModeWordWrap ); 
        text_layer_set_text(ins_text, "Use your phone to add workouts. On the pebble app, find this timer app and click osettings icon. ");
        text_layer_set_font(ins_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
        text_layer_set_text_alignment(ins_text, GTextAlignmentLeft);
        layer_add_child(ins_window_layer, text_layer_get_layer(ins_text)); 
        instructions = true; 
  }
  
  else{
      window = window_create(); 
      window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
      });
      window_stack_push(window, true /* Animated */);
  }
    
    
    // Was this a wakeup?
  if(launch_reason() == APP_LAUNCH_WAKEUP) {
    // The app was started by a wakeup
    WakeupId id = 0;
    int32_t reason = 0;

    // Get details and handle the wakeup
    wakeup_get_launch_event(&id, &reason);
    wakeup_handler(id, reason);
  }
          //persist_delete(PERSIST_KEY_WAKEUP_ID);
  checkScheduledWakeup();
  app_event_loop();
  text_layer_destroy(text_layer);
  window_destroy(window);
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Exiting..."); 

}