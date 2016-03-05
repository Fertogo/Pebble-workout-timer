#include <pebble.h>
#include "common.h"

#include "linked-list.h"
#include "workout.h"
#include "windows/win_move.h"
#include "windows/win_workout_finished.h"
#include "windows/win_main.h"
#include "message_helper.h"
#include "windows/win_loading.h"
#include "storage.h"
#include "windows/win_loading.h"

#define TIMER_WAKEUP 0


static void workout_finished(Workout* workout);
void parse_moves_message(Workout* workoout, LinkedRoot* data);
const char MESSAGE_DELIMITER[3] = "\t";
SavedMove* saved_move = NULL; 
Move* restore_move(Workout* workout); 
Workout* temp_workout = NULL; 

/**
* Add given move to the end of given workout
* @param Wokout* workout: Workout to add move to
* @param Move* move: Move to add to Workout
*/
void workout_add_move(Workout* workout, Move* move) {
  LOG("Adding move to workout");
  move->workout = workout;
  linked_list_append(workout->moves, move);
}

/**
* Start given workout at it's current location
*             workout location will be restored if saved_moved exists
* @param Workout* workout: workout to start
*/
void workout_start(Workout* workout){
  LOG("Starting workout");

  Move* first_move = NULL; 
  if (saved_move != NULL) first_move = restore_move(workout); 
  else first_move = move_initialize(linked_list_get(workout->moves, 0));
  
  if (first_move == NULL) return; //Handle edge case when app wakeup on last move
   
  LOG("got first move"); 
  move_print(first_move);

  show_win_move();
  DEBUG("Showing win move");
  move_start(first_move); 
  DEBUG("Started First move");
  
  saved_move = NULL; //Reset saved move; 
}

/**
* Restores first move from given Workout
* @parma Workout* workout: workout to restore move from
*/ 
Move* restore_move(Workout* workout) {
  // Saved move is done! 
  if (launch_reason() == APP_LAUNCH_WAKEUP){ 
    INFO("App started by wakeup"); 
    if (saved_move->index >= linked_list_count(workout->moves)-1){ 
      workout_finished(workout);
      return NULL; 
    }
    else return move_initialize(linked_list_get(workout->moves, saved_move->index + 1)); 
  }
  
  Move* first_move = NULL; 
  LOG("Start Workout - Saved move");
  LOG("Index %i", saved_move->index);
  first_move = linked_list_get(workout->moves, saved_move->index); 
  move_print(first_move);
  first_move->status = saved_move->status; 
  LOG("First Move status: %i", first_move->status); 
  if (saved_move->type == MOVE_TYPE_REPS) { 
    LOG("Saved rep move move value"); 
    first_move->current_value = saved_move->value; 
  }
  else if (saved_move->type == MOVE_TYPE_TIMER) { 
    //TODO Cancel wakeup aft
    if (saved_move->status == MOVE_STATUS_RUNNING) { 
      // Is the wakeup still scheduled?
      time_t wakeup_timestamp = 0;
      if (wakeup_query(saved_move->wakeup_id, &wakeup_timestamp)) {
        LOG("Saved move has a saved wakeup");
        // Get the time remaining
        int seconds_remaining = wakeup_timestamp - time(NULL);
        first_move->current_value = seconds_remaining; 
       } else{ 
        ERROR("Expected move to have wakeup :("); 
        first_move->current_value = 0; 
      } 
    }
    else if (saved_move->status == MOVE_STATUS_PAUSED) {
      LOG("Saved move was paused. ");
      first_move->current_value = saved_move->value; 
    }
  }
  return first_move; 
}


/**
* Handles event when a move is finished. 
*     Either advances to next move or finishes workout
* @param Move* move: move that recently finished
*/
void workout_move_finished(Move* move) {

  LOG("Move just finished");

  uint8_t index = move->index;

  Workout* workout = move->workout;

  if (index >= linked_list_count(workout->moves)-1) {
    workout_finished(workout);
  }
  else {
    //Advance to next move
    Move* next_move = move_initialize(linked_list_get(workout->moves, index+1));
    LOG("Starting next move");
    move_print(next_move);

    move_start(next_move);
  }
}



/**
* Handles when a workout is finished
* @param Workout* workout: Workout that recently finished
*/
static void workout_finished(Workout* workout){
  LOG("Workout Finished");
  message_helper_finish_workout(workout);
  saved_move = NULL; //Reset saved move; 
  show_win_workout_finished();
  hide_win_move();
  free(workout);
}

/**
* Completely stops the given workout
* @param Workout* workout: Workout to stop
*/
void workout_stop(Workout* workout) { 
  saved_move = NULL; 
  free(workout); 
}

/**
* Request a workout from the phone to start on the Pebble
* @param: char* workout_title: Title of workouts as is saved on the Pebble and the Phone.
* @param: int start_index: Index of move to start from. 
*/
void workout_request_workout(char* workout_title) { 
  show_win_loading();
  if (!bluetooth_connection_service_peek()){
     win_loading_show_error("Please connect Pebble to Phone then try again.");
  }
  message_helper_request_workout(workout_title);
}

/**
* Get the name of the next move in the workout
* @param: Move* current_move: current move in workout.
* @returns: char* with the name of the next move.
*/
char* workout_get_next_move_name(Move* current_move){
  uint8_t index = current_move->index;
  Workout* workout = current_move->workout;
  if (index >= linked_list_count(workout->moves)-1) return "Last move!";
  Move* next_move = linked_list_get(workout->moves, index+1);
  return next_move->name;
}

//TODO These parse functions do too much and should be split. 
//Parse functions should return what they parse ie Workout* or Move*

/**
* Parses workout messages from phone and starts workout 
* @param char* header: header of message sent from phone
* @param LinkedRoot* data: LinkedList with message data
*/
void workout_parse_message(char*header, LinkedRoot* data) {
  LOG("Parsing Move Message");

  char* workout_name = strtok(header, MESSAGE_DELIMITER);
  uint8_t message_index = atoi(strtok(NULL, MESSAGE_DELIMITER));
  uint8_t message_total = atoi(strtok(NULL, MESSAGE_DELIMITER));
  INFO("WorkoutName: %s", workout_name);
  INFO("Chunk %i / %i", message_index, message_total); 
  if (message_index == 0) { //First message in the batch, create new workout
    temp_workout = workout_create(workout_name); 
  }
  parse_moves_message(temp_workout, data);
  if (message_index == message_total) { //Last message in batch, start workout
    hide_win_loading();
    workout_start(temp_workout);
  }

}

/**
* Parses move section of workout messages from phone and adds move to given Workout. 
* @param char* header: header of message sent from phone
* @param LinkedRoot* data: LinkedList with message data
*/
void parse_moves_message(Workout* workout, LinkedRoot* data) {
  INFO("Move data received");

  int index = 0;
  char* move_data = linked_list_get(data, index);
  while (move_data != NULL) {

    char* name = strtok(move_data, MESSAGE_DELIMITER);
    char* type_str = strtok(NULL, MESSAGE_DELIMITER);
    uint16_t length = atoi(strtok(NULL, MESSAGE_DELIMITER));

    MoveType type = MOVE_TYPE_TIMER;
    if (strcmp(type_str,"time") == 0) type = MOVE_TYPE_TIMER;
    if (strcmp(type_str,"reps") == 0) type = MOVE_TYPE_REPS;

    //Add move to workout
    Move* move = move_create(name, length, type, index);
    workout_add_move(workout, move);

    index++;
    move_data = linked_list_get(data, index);

    INFO("Name: %s Type:%i Value:%i", name, type, length);
  }
  workout_print(workout); 
  win_main_refresh();
}

/**
* Creates and initalizes a new workout with the given name
* @param char* name: Name of workout
* @returns Workout*: Created Workout struct
*/
Workout* workout_create(char* name) {
  Workout* workout = malloc(sizeof(Workout));
  strcpy(workout->name, name);

  //TODO Cleanup moves?
  workout->moves = linked_list_create_root();
  return workout;
}

/**
* Save the state of the given move to persistent memmory and schedule wakeup if necessary. 
* @param Move* move: Move to save state
*/
void workout_save_current_move(Move* move) { 
  SavedMove* saved_move = malloc(sizeof(SavedMove));
  strcpy(saved_move->workout_name, move->workout->name); 
  saved_move->index = move->index; 
  saved_move->status = move->status; 
  saved_move->type   = move->type; 
  
  if (move->type == MOVE_TYPE_TIMER) { 
    //Schedule wakeup and save id 
    if (move->status == MOVE_STATUS_RUNNING) { 
      time_t future_time = time(NULL) + move->current_value; 
      WakeupId wakeup_id = wakeup_schedule(future_time, TIMER_WAKEUP, false);
      saved_move->wakeup_id = wakeup_id;
    }
  }
  
  saved_move->value = move->current_value; 
  
  INFO("Saving move with index: %i", saved_move->index); 
  //Persist SavedMove
  storage_save_current_move(saved_move); 
  free(saved_move); 
}

/**
* Restores workout if needed
*/ 
void workout_restore() { 
  LOG("Restore move?");
  saved_move = storage_get_current_move(); 
  if (saved_move != NULL) { 
    LOG("There is a saved moved"); 
    workout_request_workout(saved_move->workout_name);     
    storage_reset_current_move(); 
  }
}

/**
* Debug function to print workout contents
* @param Workout* workout: Workout to print
*/
void workout_print(Workout* workout) { 
  LOG("Workout. Name: %s, Current_move_index: %i, Total_moves: %i", workout->name, workout->current_move_index, linked_list_count(workout->moves));
}

