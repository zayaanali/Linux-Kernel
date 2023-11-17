#include "scheduler.h"
#include "pcb.h"
#include "systemcall.h"
#include "terminal.h"
#include "paging.h"

int32_t active_pid = -1; 
int32_t find_next_pid(int32_t active_pid);
uint8_t base_shells_opened = 0;

/* switch_process
 *   Inputs: none
 *   Return Value: none
 *   Function: switches to next process in round-robin fashion when pit interrrupt occurs */
int32_t switch_process(){

    if(base_shells_opened<3){
        active_pid++;
        base_shells_opened++;
        terminal_switch(active_pid);
        execute((const uint8_t*)"shell");
    }

    if(base_shells_opened==3){
        terminal_switch(0);
        base_shells_opened++;
    }

    int32_t new_pid = find_next_pid(active_pid);

    // update active_pid
    active_pid = new_pid;

    // remap vidmem
   // if active_pid is on cur_terminal, it's being viewed, map vid mem to b8000
    if(pcb_ptr[active_pid]->t_id == cur_terminal){
        page_table[184].page_base_address = 184;
    }else{
        page_table[184].page_base_address = 184 + 1 + pcb_ptr[active_pid]->t_id;
    }

    // do context switch
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

