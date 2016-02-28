#pragma once
#include <pebble.h>


typedef enum {
  MOVE_TYPE_TIMER = 0, 
  MOVE_TYPE_REPS = 1
} MoveType; 

typedef enum{ 
  MOVE_STATUS_PAUSED = 0, 
  MOVE_STATUS_RUNNING = 1,
  MOVE_STATUS_QUEUED = 2, 
  MOVE_STATUS_DONE = 3
}MoveStatus; 

typedef struct Move { 
  uint8_t id; 
  AppTimer* timer;
  
  uint8_t index; // Index of this timer in the workout as a whole
  MoveType type; 
  MoveStatus status; 
  uint16_t length; 
  uint16_t current_value; 
  WakeupId wakeup_id; 
  char name[40]; 
  
  struct Workout* workout; //Workout struct this move is part of.  
} Move; 


void move_start(Move* move); 
void move_timer_pause(Move* move); 
void move_timer_resume(Move* move); 
void move_rep_next(Move* move); 
void move_timer_pause_or_resume(Move* move); 
void move_finish(Move* move); 
void move_stop(Move* move); 


void move_value_str(uint16_t move_value, MoveType move_type, char* str, int str_len);
Move* move_create(char* name, uint16_t length, MoveType type);



void move_print(Move* timer);
