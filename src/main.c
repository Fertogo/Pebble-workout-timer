#include <pebble.h>

#include "move.h"
#include "windows/win_move.h"
#include "workout.h"
#include "windows/win_main.h"

#include "message_helper.h"


static void init(void); 
static void deinit(void); 

int main(void) { 
  init(); 
  app_event_loop(); 
  deinit(); 
}


static void init(void) { 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "C Code - Init");
  message_helper_init(); 
  win_main_init(); 
}

static void deinit(void) { 
  //TODO Save currently running move
}