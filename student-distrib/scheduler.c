#include "scheduler.h"
#include "pcb.h"
#include "systemcall.h"
#include "terminal.h"
#include "paging.h"
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

    // get esp, ebp, and eip of next process
    uint32_t new_esp;
    uint32_t new_ebp;
    uint32_t new_eip;

    // do context switch
    context_switch(new_esp, new_ebp, new_eip);
}

/* find_next_pid
 *   Inputs: pid of currently active process
 *   Return Value: pid scheduler should switch to
 *   Function: helper function that starts at cur_pid, walks through pcb structs to find next active process to run */
int32_t find_next_pid(int32_t active_pid){

    uint32_t pid = (active_pid+1)%6; 
    int i;
    pcb_entry_t* pcb = pcb_ptr[pid];
    uint32_t cur_t_id = pcb_ptr[active_pid]->t_id;

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

void context_switch(uint32_t esp, uint32_t ebp, uint32_t eip) {
    asm volatile(
        "pushl %0; \n"            // push data segment register
        "pushl %1; \n"            // push new esp
        "pushfl; \n"
        "popl %%ecx; \n"
        "orl $0x200, %%ecx; \n"   // Set IF flag to one
        "pushl %%ecx; \n"
        "pushl %2; \n"            // push new eflags
        "pushl %3; \n"            // push new eip
        "iret; \n"

        :                                       
        : "g" (USER_DS), "g" (esp), "g" (USER_CS), "g" (eip)
        : "memory", "cc", "ecx"
    );
    return;
}
