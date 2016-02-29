#pragma once

#include "linked-list.h"
#include "workout.h"

void storage_store_workouts(char* header, LinkedRoot* data); 
char* storage_get(int key); 
char* storage_get_workout(int index); 
void storage_set(int key, char* data); 

void storage_save_current_move(SavedMove* saved_move); 
SavedMove* storage_get_current_move(); 
void storage_reset_current_move(); 