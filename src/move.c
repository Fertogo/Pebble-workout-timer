/* 
Move | Custom Workout Timer. v4.0
Manages single move creation, functions and interactions 
Copyright © 2016 Fernando Trujano
                 trujano@mit.edu
*/

#include <pebble.h>
#include "common.h"

#include "move.h"
#include "workout.h"
#include "windows/win_move.h"


static void timer_tick(void* context);
static void timer_schedule_tick(Move* move);
static void timer_cancel_tick(Move* move);
static void timer_cancel_wakeup(Move* move); 
static void update_move_value(Move* move);


/**
* Gets a string representation of the given move value
* @param uint16_t move_value: Value of the move to stringify
* @param MoveType move_type: Type of moved being stringified
* @param char* str: Pointer where the stringified moved will be stored
* @param in str_len: Overestimated length of stringified move
*/
void move_value_str(uint16_t move_value, MoveType move_type, char* str, int str_len){
  int minutes, seconds, reps;
  switch (move_type) {
    case MOVE_TYPE_TIMER:
      minutes = move_value / 60;
      seconds = move_value % 60;
      if (minutes > 0)
        snprintf(str, str_len, "%d:%02d", minutes, seconds);
      else
        snprintf(str, str_len, "%d", seconds);
      break;
    case MOVE_TYPE_REPS:
      reps = move_value;
      snprintf(str, str_len, "%d", reps);
      break;
  }
}

/**
* Creates a Move struct given properties
* @param char* name: Name of the move
* @param uint16_t length: Total length of the move
* @param MoveType type: Type of move
* @param uint8_t index: Index of move in it's parent Workout. 
* @returns Move* representing created move with given properties
*/
Move* move_create(char* name, uint16_t length, MoveType type, uint8_t index) {
  Move* move = malloc(sizeof(Move));
  move->type = type;
  move->length = length;
  move->status = MOVE_STATUS_QUEUED;
  move->index = index; 
  strcpy(move->name, name);
  return move;
}

/**
* Safely attemps to cancel a Move's timer
* @param Move* move: Move to cancel timer from
*/
static void timer_cancel_tick(Move* move) {
  if(!move) return;
  if (move->type != MOVE_TYPE_TIMER) return;
  if (move->timer) {
    LOG("Canceling timer");
    app_timer_cancel(move->timer);
    move->timer = NULL;
  }
}

/**
* Safely attempts to cancel a Move's wakeup
* @param Move* move: Move to cancel wakeup for
*/
static void timer_cancel_wakeup(Move* move) {
  if (!move) return;
  if (move->wakeup_id <= 0) return;
  wakeup_cancel(move->wakeup_id);
  move->wakeup_id = -1;
}

/**
* Ticks the timer of a running timer move
* @param Move* context: Move to tick
*/
static void timer_tick(void* context) {
  Move* move = (Move*)context;
  if (move->type != MOVE_TYPE_TIMER) return;

  move->timer = NULL;
  if (move->current_value <= 0) {
    move_finish(move);
    return;
  }
  move->current_value -= 1;
  LOG("Timer Tick %i", (int)move->current_value);
  move_print(move);
  update_move_value(move);

  if (move->status == MOVE_STATUS_RUNNING) {
    timer_schedule_tick(move); //Only schedule new tick if timer is supposed to be running!
  }

}

/**
* Schedules timer ticks
* @param Move* move: Move to schedule timer for. 
*/ 
static void timer_schedule_tick(Move* move) {
  LOG("Scheduling timer tick");
  move_print(move);
  timer_cancel_tick(move);
  move->timer = app_timer_register(1000, timer_tick, (void*)move);
}

/**
* Finish a move and alert Workout of completion
* @param Move* move: Move to mark as finished
*/
void move_finish(Move* move) {
  LOG("MOVE FINISH");
  timer_cancel_tick(move);
  move->status = MOVE_STATUS_DONE;
  vibes_short_pulse();
  workout_move_finished(move);
}

/** 
* Completely stop a given move
* @param Move* move: Move to stop
*/
void move_stop(Move* move) {
  move->status = MOVE_STATUS_DONE;
  timer_cancel_tick(move);
}

/**
* Pauses a current timer move and updates UI
* @param Move* move: Move to pasue
*/
void move_timer_pause(Move* move) {
  win_move_show_pause();
  if (move->type != MOVE_TYPE_TIMER) return;
  LOG("Pausing timer");
  move->status = MOVE_STATUS_PAUSED;
  timer_cancel_tick(move);
}

/**
* Resumes a paused timer move and updates UI
* @param Move* move: Move to resume
*/
void move_timer_resume(Move* move) {
  win_move_hide_pause();
  if (move->type != MOVE_TYPE_TIMER) return;
  move->status = MOVE_STATUS_RUNNING;
  timer_schedule_tick(move);
  //TODO Schedule Wakeup
}

/**
* Decrements remining reps of rep move by one or advances to next workout if completed
* @param Move* move: Rep move to update
*/
void move_rep_next(Move* move) {
  LOG("Next Rep");
  if (move->type != MOVE_TYPE_REPS) return;
  move->current_value--;
  if (move->current_value <= 0) move_finish(move);
  else update_move_value(move);
}

/**
* Decides wether a move should pause or resume
* @param Move* move: Move to update
*/ 
void move_timer_pause_or_resume(Move* move){
  if (move->status == MOVE_STATUS_PAUSED) move_timer_resume(move);
  else if (move-> status == MOVE_STATUS_RUNNING) move_timer_pause(move);
}

/**
* Update the UI with the current value of given move
* @parm Move* move: Move to update
*/
static void update_move_value(Move* move) {
  char* move_value = malloc(MOVE_NAME_SIZE); 
  move_value_str(move->current_value, move->type, move_value, MOVE_NAME_SIZE);
  LOG("Value %s", move_value);

  win_move_set_value(move_value);
  free(move_value);
}

/**
* Initializes a new move by setting default and starting values
* @param Move* move: Move to initialize
* @returns Move*: Updated Move
*/ 
Move* move_initialize(Move* move) { 
  INFO("Initializing new move");
  move->status = MOVE_STATUS_RUNNING;
  move->current_value = move->length;
  move_print(move); 
  return move; 
}

/**
* Starts a move with its current properties and update UI
*          Note that new moves should be initialized first 
* @param Move* move: Move to start 
*/ 
void move_start(Move* move){
  win_move_set_next_move_name(workout_get_next_move_name(move));
  LOG("Setting Move Name+Value %s - %i", move->name, move->current_value);
  win_move_set_move(move);
  update_move_value(move);
  if (move->status == MOVE_STATUS_PAUSED) move_timer_pause(move); 
  else if (move->type == MOVE_TYPE_TIMER) timer_schedule_tick(move); 
}

/**
* Debugging funciton to help visualize Move's properties
* @param Move* move: Move to print
*/
void move_print(Move* move) {
  LOG("Move. Length: %i, Type: %i, Title: %s, Current_value: %i, Index: %i, Status: %i", (int)move->length, move->type, move->name, (int)move->current_value, (int)move->index, move->status);
}
