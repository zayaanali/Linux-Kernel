#include "systemcall.h"
#include "x86_desc.h"
#include "lib.h"

extern void systemcall_link(); 

void init_syscall_idt(){

    // set reserved0|D|reserved1|reserved2|reserved3|reserved4[8] to 0 1111 0000 0000 for 32-bit trap gate
    idt[128].present = 1;             // segment is present
    idt[128].dpl = 0;                 // DPL = 00 (highest priority)
    idt[128].reserved0 = 0;           
    idt[128].size = 1;                // size (D) = 1 (32 bit gate)
    idt[128].reserved1 = 1;
    idt[128].reserved2 = 1;
    idt[128].reserved3 = 1;
    idt[128].reserved4 = 0;
    idt[128].seg_selector = KERNEL_CS;

    SET_IDT_ENTRY(idt[128], systemcall_link);
}

void systemcall_handler(){
    printf("A system call was called. \n");
}

    
