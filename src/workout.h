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

void workout_parse_message(char* header, LinkedRoot* data);

void workout_request_workout(char* workout_title); 

Workout* workout_create(char* name);

