#include "utilities.h"
#include <xc.h>

static volatile int current_mode = IDLE;

int get_mode() {
  return current_mode;
}

void set_mode(int mode) {
  current_mode = mode;
}
