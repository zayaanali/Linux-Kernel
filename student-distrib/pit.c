#include "pit.h"
#include "systemcall.h"
#include "lib.h"
#include "x86_desc.h"
#include "system_s.h"
#include "filesystem.h"
#include "rtc.h"
#include "file.h"
#include "filedir.h"
#include "pcb.h"
#include "paging.h"
#include "page.h"
#include "terminal.h"
#include "x86_desc.h"
#include "excepts.h"
#include "excepts_s.h"
#include "i8259.h"
#include "scheduler.h"


#define FREQ 10
#define LOWER_BYTE_MASK 0xFF
#define UPPER_BYTE_SHIFT 8
#define PIT_COMMAND_PORT 0x43
#define PIT_DATA_PORT 0x40

void pit_init(){
    uint32_t divisor = 1193180 / FREQ;
    outb(0x36, PIT_COMMAND_PORT);  /* Set our command byte 0x36 */
    outb(divisor & LOWER_BYTE_MASK, PIT_DATA_PORT);  /* Set low byte of divisor */
    outb((divisor >> UPPER_BYTE_SHIFT) && LOWER_BYTE_MASK, PIT_DATA_PORT);  /* Set high byte of divisor */
    init_pit_idt();
    
}
void pit_int_handler(){
    send_eoi(0);
    cli();
    
    switch_process();
    
    sti();
}
void init_pit_idt(){
    
    // pit should be at entry 0x20 of idt
    // set reserved0|size|reserved1|reserved2|reserved3|reserved4[8] to 0 1110 0000 0000 for 32-bit interrupt gate
    idt[32].present = 1;             // segment is present
    idt[32].dpl = 0;                 // DPL = 00 (highest priority)
    idt[32].reserved0 = 0;           
    idt[32].size = 1;                // size (D) = 1 (32 bit gate)
    idt[32].reserved1 = 1;
    idt[32].reserved2 = 1;
    idt[32].reserved3 = 0;
    idt[32].reserved4 = 0;
    idt[32].seg_selector = KERNEL_CS;

    // set offset fields so that this gate points to assembly systemcall handler for the handler function 
    SET_IDT_ENTRY(idt[32], pit_int_link);

    
}
