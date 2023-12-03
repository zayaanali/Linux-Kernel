#include "scheduler.h"
#include "pcb.h"
#include "systemcall.h"
#include "terminal.h"
#include "paging.h"
#include "page.h"
#include "lib.h"
#include "x86_desc.h"
#include "i8259.h"

/* Current PID for each terminal (what is the highest process for each terminal) */
int32_t term_cur_pid[3] = {-1,-1,-1};

volatile int32_t active_pid = -1; 
volatile int32_t active_tid = -1;
uint8_t base_shells_opened = 0;




/* switch_process
 *   Inputs: none
 *   Return Value: none. "return" used to switch into next process
 *   Function: switches to next process in round-robin fashion when pit interrrupt occurs */
int32_t switch_process() {
    /* Get next terminal */
    int next_term = (active_tid+1)%NUM_TERMINALS;
    remap_vidmem(next_term);
    
    /* Save EBP/ESP of the current process */
    register uint32_t s_esp asm("%esp");
    register uint32_t s_ebp asm("%ebp");

    /* Set up new process */
    if (term_cur_pid[next_term] == -1) { // No process currently running on this terminal
        // First process context does not need to be stored (switching from kernel)
        if (next_term != 0) {
            // Store current process context (so that can return later)
            pcb_ptr[active_pid]->esp = (uint32_t)s_esp;
            pcb_ptr[active_pid]->ebp = (uint32_t)s_ebp;
        }
        
        // set next active tid/pid and set in use flag
        active_tid = next_term;
        active_pid++;

        // Start new process
        cur_terminal = next_term;
        printf("Starting shell %d\n", active_pid);
        execute((const uint8_t*)"shell");
        return 0;
    } else {
        // Store current process context (so that can return later)
        pcb_ptr[active_pid]->esp = (uint32_t)s_esp;
        pcb_ptr[active_pid]->ebp = (uint32_t)s_ebp;
        
        // set next active tid/pid and set in use flag
        active_tid = next_term;
        active_pid = term_cur_pid[active_tid];
        
    }

    // change PID page base address
    page_dir[32].page_dir_entry_4mb_t.page_base_address = ((EIGHT_MB + (active_pid*FOUR_MB)) >> 22); // align the page_table address to 4MB boundary
    flush_tlb();

    
    /* Set TSS entries */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHT_MB - (active_pid)*EIGHT_KB);

    // get esp, ebp, and eip of next process
    register uint32_t new_esp = pcb_ptr[active_pid]->esp;
    register uint32_t new_ebp = pcb_ptr[active_pid]->ebp;

    // do context switch
    asm volatile(
        "movl %0, %%esp; \n"            // switch to new esp
        "movl %1, %%ebp; \n"            // switch to new ebp

        :                                       
        : "g" (new_esp), "g" (new_ebp)
        : "memory", "cc", "ecx"
    );
    send_eoi(0);
    return 0;
}
