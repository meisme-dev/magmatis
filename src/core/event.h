#pragma once
#include "magmatis.h"

int magmatis_event_loop_run(Magmatis *program);

#ifndef USE_LONG_NAMES
#define event_loop_run magmatis_event_loop_run
#endif