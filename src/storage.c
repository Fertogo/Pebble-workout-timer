/* 
Storage | Custom Workout Timer. v4.0
Handles interactions with Pebble's persistent storage
Copyright © 2016 Fernando Trujano
                 trujano@mit.edu
*/

#include <pebble.h>
#include "common.h"

#include "storage.h"
#include "windows/win_main.h"

#define PERSIST_KEY_WORKOUTS_ROOT 200
#define PERSIST_SAVED_MOVE 100
#define PERSIST_STORAGE_VERSION 1 


char* storage_get(int key);
void storage_set(int key, char* data);
static void clear_storage();
static void clear_v3_storage(); 
void storage_migrate_v_3(); 

/**
* Processes workout messages, stores workouts on Pebble and updates UI
*      Called when OWM returns to Pebble
* @param char* heade: Header of the received WORKOUTS message
* @param LinkedRoot* data: LinkedList containing message contents
*/
void storage_store_workouts(char* header, LinkedRoot* data){
  LOG("Storing Workouts");
  clear_storage();

  LOG("Total Number: %i", linked_list_count(data));


  int count = 0;
  char* workout_info = linked_list_get(data, count);
  while (workout_info != NULL) {

    //Save workout to persistant memory
    storage_set(PERSIST_KEY_WORKOUTS_ROOT + count, workout_info);

    count++;
    workout_info = linked_list_get(data, count);

  }

  LOG("Done Storing Workout");
  win_main_refresh();

}

/**
* Gets requested workout info given index
* @returns char* workoutInfo, or NULL if workout with given index not found. 
*/
char* storage_get_workout(int index) {
  return storage_get(PERSIST_KEY_WORKOUTS_ROOT + index);
}

/**
* Clear all data Pebble is currently storing
*/
void clear_storage() {
  int i = 0;
  while(persist_exists(PERSIST_KEY_WORKOUTS_ROOT + i)) {
    persist_delete(PERSIST_KEY_WORKOUTS_ROOT + i);
    i++;
  }
  storage_reset_current_move(); 
}

/**
* Gets an item from persistent storage given a key
* @param int key: Key where requested char* lives on persistent storage
* @returns char* with read data, or NULL if key not found
*/ 
char* storage_get(int key) {
  LOG("Storage Get %i", key);

  char * total = "error";
  if (persist_exists(key)){
    persist_read_string(key, total, persist_get_size(key));
    char * s = total;
    LOG("Read From Storage: Key: %i , Value: %s", key,s);
    return s;
  }
  LOG("Reading from storage: Key %i not found", key);
  return NULL;
}

/**
* Saves new data to persistent storage
* @param int key: Key to save the data to
* @param char* data: Data to save
*/
void storage_set(int key, char* data) {
  LOG("Saving to storage: Key: %i Data: %s", key, data);
  persist_write_string(key, data);
}

/**
* Saves given SavedMove struct to persistent storage
* @param SavedMove* saved_move: SaveMove struct to save
*/ 
void storage_save_current_move(SavedMove* saved_move) { 
  persist_write_data(PERSIST_SAVED_MOVE, saved_move, sizeof(SavedMove));
}

/**
* Retrieves SavedMove from persistent storage
* @returns SavedMoveL saved_moved, or NULL if no move is saved
*/
SavedMove* storage_get_current_move() { 
  if (persist_exists(PERSIST_SAVED_MOVE)){ 
    SavedMove* saved_moved = malloc(sizeof(SavedMove));
    persist_read_data(PERSIST_SAVED_MOVE, saved_moved, sizeof(SavedMove));
    return saved_moved; 
  }
  return NULL; 
}

/**
* Reset the stored savedMove
*/
void storage_reset_current_move() { 
  if (persist_exists(PERSIST_SAVED_MOVE)) { 
    persist_delete(PERSIST_SAVED_MOVE); 
  }
}

/**
* Initializes persistant storage
*/
void storage_init() { 
  LOG("Initializing Storage"); 
  storage_migrate_v_3(); 
  storage_set(PERSIST_STORAGE_VERSION,"4"); 
}

/**
* Migrates storage between v3 to v4
*/
void storage_migrate_v_3(){ 
  if (storage_get(0) != NULL) { 
    LOG("There is v3 data"); 
    clear_v3_storage();
  }
}

/**
* Clears all storage that v3.x used 
*/
void clear_v3_storage() { 
  //Should delete all keys in range [0,21], [337,344]
  for (int i=0; i<30; i++) { 
    if (persist_exists(i)) persist_delete(i); 
  }
   for (int i=330; i<350; i++) { 
    if (persist_exists(i)) persist_delete(i); 
  }
}
