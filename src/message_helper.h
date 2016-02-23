#pragma once
#include "workout.h"

void message_helper_init(); 
char* strtok(); 

void message_helper_request_workout(char* workout_title); 
void message_helper_finish_workout(Workout* workout); 
