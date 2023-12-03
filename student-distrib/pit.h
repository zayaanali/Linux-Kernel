#ifndef _PIT_H
#define _PIT_H

#include "lib.h"
#define PIT_IRQ 0


void pit_init();
void pit_int_handler();
void init_pit_idt();


#endif
