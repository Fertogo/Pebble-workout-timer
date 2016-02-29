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

Move* move_create(char* name, uint16_t length, MoveType type, uint8_t index) {
  Move* move = malloc(sizeof(Move));
  move->type = type;
  move->length = length;
  move->status = MOVE_STATUS_QUEUED;
  move->index = index; 
  strcpy(move->name, name);
  return move;
}

static void timer_cancel_tick(Move* move) {
  if(!move) return;
  if (move->type != MOVE_TYPE_TIMER) return;
  if (move->timer) {
    LOG("Canceling timer");
    app_timer_cancel(move->timer);
    move->timer = NULL;
  }
}

static void timer_cancel_wakeup(Move* move) {
  if (!move) return;
  if (move->wakeup_id <= 0) return;
  wakeup_cancel(move->wakeup_id);
  move->wakeup_id = -1;
}

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

void move_finish(Move* move) {
  LOG("MOVE FINISH");
  timer_cancel_tick(move);
  move->status = MOVE_STATUS_DONE;
  vibes_short_pulse();
  workout_move_finished(move);
}

void move_stop(Move* move) {
  move->status = MOVE_STATUS_DONE;
  timer_cancel_tick(move);
}

static void timer_schedule_tick(Move* move) {
  LOG("Scheduling timer tick");
  move_print(move);
  timer_cancel_tick(move);
  move->timer = app_timer_register(1000, timer_tick, (void*)move);
}

void move_timer_pause(Move* move) {
  win_move_show_pause();
  if (move->type != MOVE_TYPE_TIMER) return;
  LOG("Pausing timer");
  move->status = MOVE_STATUS_PAUSED;
  timer_cancel_tick(move);
}

void move_timer_resume(Move* move) {
  win_move_hide_pause();
  if (move->type != MOVE_TYPE_TIMER) return;
  move->status = MOVE_STATUS_RUNNING;
  timer_schedule_tick(move);
  //TODO Schedule Wakeup
}

void move_rep_next(Move* move) {
  LOG("Next Rep");
  if (move->type != MOVE_TYPE_REPS) return;
  move->current_value--;
  if (move->current_value <= 0) move_finish(move);
  else update_move_value(move);
}

void move_timer_pause_or_resume(Move* move){
  if (move->status == MOVE_STATUS_PAUSED) move_timer_resume(move);
  else if (move-> status == MOVE_STATUS_RUNNING) move_timer_pause(move);
}

static void update_move_value(Move* move) {
  char* move_value = malloc(30);
  move_value_str(move->current_value, move->type, move_value, 30);
  LOG("Value %s", move_value);

  win_move_set_value(move_value);
  free(move_value);
}

//Initialize a move
Move* move_initialize(Move* move) { 
  INFO("Initializing new move");
  move->status = MOVE_STATUS_RUNNING;
  move->current_value = move->length;
  move_print(move); 
  return move; 
}

void move_start(Move* move){
  win_move_set_next_move_name(workout_get_next_move_name(move));
  LOG("Setting Move Name+Value %s - %i", move->name, move->current_value);
  win_move_set_move(move);
  update_move_value(move);
  if (move->status == MOVE_STATUS_PAUSED) move_timer_pause(move); 
  else if (move->type == MOVE_TYPE_TIMER) timer_schedule_tick(move); 
}

//TODO - Remove
// Temp function
void move_print(Move* move) {
  LOG("Move. Length: %i, Type: %i, Title: %s, Current_value: %i, Index: %i, Status: %i", (int)move->length, move->type, move->name, (int)move->current_value, (int)move->index, move->status);
}
