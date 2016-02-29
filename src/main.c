#include <pebble.h>
#include "common.h"

#include "move.h"
#include "windows/win_move.h"
#include "workout.h"
#include "windows/win_main.h"

#include "message_helper.h"


static void init(void);
static void deinit(void);

int main(void) {
  init();
  app_event_loop();
  deinit();
}


static void init(void) {
  LOG("C Code - Init");
  message_helper_init();
  win_main_init();
  workout_restore();
}

static void deinit(void) {
}
