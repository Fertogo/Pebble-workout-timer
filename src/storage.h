#pragma once

#include "linked-list.h"


void storage_store_workouts(char* header, LinkedRoot* data); 
char* storage_get(int key); 
char* storage_get_workout(int index); 
void storage_set(int key, char* data); 