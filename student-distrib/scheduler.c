#include "scheduler.h"
#include "pcb.h"
#include "systemcall.h"
#include "terminal.h"
#include "paging.h"
#include "page.h"
#include "lib.h"
#include "x86_desc.h"


int32_t active_pid = -1; 
int32_t active_tid = -1;
int32_t find_next_pid(int32_t active_pid);
uint8_t base_shells_opened = 0;

/* switch_process
 *   Inputs: none
 *   Return Value: none
 *   Function: switches to next process in round-robin fashion when pit interrrupt occurs */
int32_t switch_process(){

    int32_t new_pid;
    int32_t old_pid = active_pid; 

    /* Open base shell if three don't already exist */
    if(base_shells_opened<3){
        active_pid++;
        base_shells_opened++;
        terminal_switch(active_pid);
        execute((const uint8_t*)"shell");
    }
   
    if(base_shells_opened==3){
        // after base shells opened, start by servicing process 0/terminal 0
        terminal_switch(0);
        base_shells_opened++;

        active_pid = 0;
        active_tid = 0; 
        new_pid = 0;
    } else{
        new_pid = find_next_pid(active_pid);

        // update active_pid and active_tid
        active_pid = new_pid;
        active_tid = pcb_ptr[new_pid]->t_id;
    }

    
    // remap vidmem
   // if active_pid is on cur_terminal, it's being viewed, map vid mem to b8000
    if(active_tid == cur_terminal){
        page_table[184].page_base_address = 184;
    }else{
        page_table[184].page_base_address = 184 + 1 + active_tid;
    }


    // change page base address
    page_dir[32].page_dir_entry_4mb_t.page_base_address = ((EIGHT_MB + (new_pid*FOUR_MB)) >> 22); // align the page_table address to 4MB boundary

    /* Flush TLB */
    flush_tlb();

    // save esp and ebp of current process
    register uint32_t s_esp asm("%esp");
    register uint32_t s_ebp asm("%ebp");

    pcb_ptr[old_pid]->esp = s_esp;
    pcb_ptr[old_pid]->ebp = s_ebp; 

    /* Set TSS entries */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHT_MB - (new_pid)*EIGHT_KB);

    // get esp, ebp, and eip of next process
    register uint32_t new_esp = pcb_ptr[new_pid]->esp;
    register uint32_t new_ebp = pcb_ptr[new_pid]->ebp;
    uint32_t new_eip;

    // do context switch
    asm volatile(
        "movl %0, %%esp; \n"            // switch to new esp
        "movl %1, %%ebp; \n"            // switch to new ebp

        :                                       
        : "g" (new_esp), "g" (new_ebp)
        : "memory", "cc", "ecx"
    );
    return 0;
}

/* find_next_pid
 *   Inputs: pid of currently active process
 *   Return Value: pid scheduler should switch to
 *   Function: helper function that starts at cur_pid, walks through pcb structs to find next active process to run */
int32_t find_next_pid(int32_t active_pid){

    uint32_t pid = (active_pid+1)%6; 
    int i;
    pcb_entry_t* pcb = pcb_ptr[pid];

    for(i=0; i<6; i++){
        if(pcb->current){
            return pid;
        }else{
            pid = (pid+1)%6;
            pcb = pcb_ptr[pid];
        }
    }

    // if can't find another active process to switch to, err
    return -1;
}

