#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "lib.h"

extern int32_t active_pid;
extern int32_t active_tid;

extern int32_t switch_process();
extern int32_t initialize_shells();

#endif
