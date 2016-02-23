#include <pebble.h>
#include "linked-list.h"
#include "workout.h"
#include "win_move.h"
#include "win_main.h"
#include "message_helper.h"


static void workout_finished(Workout* workout);
void parse_moves_message(Workout* workoout, LinkedRoot* data);
const char MESSAGE_DELIMITER[3] = "\t"; 


/**
* Add given move to the end of given workout
*
*/
void workout_add_move(Workout* workout, Move* move) { 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Adding move to workout");

  uint8_t id = linked_list_count(workout->moves); 
  move->id = id; 
  move->workout = workout; 
  linked_list_append(workout->moves, move);
  
}

void workout_start(Workout* workout){ 
//   message_helper_request_workout(workout); 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting workout");
  
  Move* first_move = linked_list_get(workout->moves, 0); 
  show_win_move(); 
  move_print(first_move);
  move_start(first_move); 
}

void workout_move_finished(Move* move) {
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Move just finished"); 

  uint8_t index = move->id; 

  Workout* workout = move->workout; 
  
  if (index >= linked_list_count(workout->moves)-1) { 
    workout_finished(workout); 
  }
  else { 
    //Advance to next move
    Move* next_move = linked_list_get(workout->moves, index+1); 
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting next move"); 
    move_print(next_move); 

    move_start(next_move); 
  }  
}

static void workout_finished(Workout* workout){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Workout Finished"); 
  message_helper_finish_workout(workout); 
  
  //TODO show congrats screen
}

//Called with response from phone after requesting a workout
void workout_parse_message(char*header, LinkedRoot* data) { 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Parsing Workout Message"); 

    
  char* workout_name = strtok(header, MESSAGE_DELIMITER); 
  APP_LOG(APP_LOG_LEVEL_INFO, "WorkoutName: %s", workout_name);
  
  Workout* workout = workout_create(workout_name); 

  parse_moves_message(workout, data); 
  workout_start(workout); 
}

void parse_moves_message(Workout* workout, LinkedRoot* data) { 
  APP_LOG(APP_LOG_LEVEL_INFO, "Move data received");
  
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
    Move* move = move_create(name, length, type); 
    workout_add_move(workout, move); 
    
    index++;
    move_data = linked_list_get(data, index); 
    
    
    APP_LOG(APP_LOG_LEVEL_INFO, "Name: %s Type:%i Value:%i", name, type, length);
    
    
  }
  win_main_refresh(); 
}

Workout* workout_create(char* name) { 
  Workout* workout = malloc(sizeof(Workout)); 
  strcpy(workout->name, name);
  
  
  //TODO Cleanup moves?
  workout->moves = linked_list_create_root(); 
  return workout; 
}


