#include <pebble.h>
#include "common.h"

#include "storage.h"
#include "windows/win_main.h"

#define PERSIST_KEY_WORKOUTS_ROOT 200


char* storage_get(int key);
void storage_set(int key, char* data);
static void clear_storage();

//Called when OWM returns to Pebble
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

void storage_set(int key, char* data) {
  LOG("Saving to storage: Key: %i Data: %s", key, data);
  persist_write_string(key, data);
}
