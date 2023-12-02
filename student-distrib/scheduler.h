#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "lib.h"

extern volatile int32_t active_pid;
extern volatile int32_t active_tid;
extern int32_t term_cur_pid[3];

extern int32_t switch_process();

extern uint8_t base_shells_opened;

#endif
