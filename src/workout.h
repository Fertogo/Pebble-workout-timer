#pragma once
#include "linked-list.h"
#include "move.h"

#define MAX_MOVES 30
#define MAX_TITLE_SIZE 30

typedef struct Workout { 
  char name[MAX_TITLE_SIZE]; 
  LinkedRoot* moves;
  int current_move_index; 
  int id; 
 } Workout; 

void workout_add_move(Workout* workout, Move* move); 
void workout_start(Workout* workout); 

void workout_move_finished(Move* move); 
void workout_stop(Workout* workout); 

void workout_parse_message(char* header, LinkedRoot* data);

void workout_request_workout(char* workout_title); 


Workout* workout_create(char* name);

char* workout_get_next_move_name(Move* move); 

typedef struct SavedMove { 
  WakeupId wakeup_id; 
  char workout_name[MAX_TITLE_SIZE]; 
  uint8_t index;
  uint16_t value; 
  MoveStatus status; 
  MoveType type; 
} SavedMove;
  
void workout_save_current_move(Move* move);
void workout_restore();

void workout_print(Workout* workout); 
