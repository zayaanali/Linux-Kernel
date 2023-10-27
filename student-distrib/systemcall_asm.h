#ifndef _SYSTEMCALL_ASM_H
#define _SYSTEMCALL_ASM_H

#include "systemcall.h"
#include "x86_desc.h"

extern void save_parent_regs_to_pcb();

extern void context_switch(uint32_t new_eip, uint32_t new_esp, uint32_t new_ebp);

#endif
