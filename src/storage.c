#include <pebble.h>
#include "storage.h"
#include "win_main.h"

#define PERSIST_KEY_WORKOUTS_ROOT 200


char* storage_get(int key); 
void storage_set(int key, char* data); 
static void clear_storage(); 

//Called when OWM returns to Pebble
void storage_store_workouts(char* header, LinkedRoot* data){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Storing Workouts"); 
  clear_storage(); 

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Total Number: %i", linked_list_count(data)); 


  int count = 0; 
  char* workout_info = linked_list_get(data, count); 
  while (workout_info != NULL) { 
  
    //Save workout to persistant memory
    storage_set(PERSIST_KEY_WORKOUTS_ROOT + count, workout_info); 
    
    count++; 
    workout_info = linked_list_get(data, count); 
    
  }
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done Storing Workout"); 
  win_main_refresh(); 
  
}

char* storage_get_workout(int index) { 
  return storage_get(PERSIST_KEY_WORKOUTS_ROOT + index); 
}

//Clear all of the Pebble's storage. 
void clear_storage() { 

  //Clear workout data
  int i = 0; 
  while(persist_exists(PERSIST_KEY_WORKOUTS_ROOT + i)) { 
    persist_delete(PERSIST_KEY_WORKOUTS_ROOT + i);
    i++; 
  }
    
}

char* storage_get(int key) { 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Storage Get %i", key); 

  char * total = "error"; 
  if (persist_exists(key)){ 
    persist_read_string(key, total, persist_get_size(key));
    char * s = total;  
    APP_LOG(APP_LOG_LEVEL_DEBUG,"Read From Storage: Key: %i , Value: %s", key,s);
    return s; 
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Reading from storage: Key %i not found", key);
  return NULL;  
}

void storage_set(int key, char* data) { 
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Saving to storage: Key: %i Data: %s", key, data);
  persist_write_string(key, data); 
}