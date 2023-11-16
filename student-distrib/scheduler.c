#include "scheduler.h"
#include "pcb.h"
#include "systemcall.h"


int32_t active_pid; 
uint32_t find_next_pid(int32_t active_pid);

/* switch_process
 *   Inputs: none
 *   Return Value: none
 *   Function: switches to next process in round-robin fashion when pit interrrupt occurs */
void switch_process(){

    uint32_t new_pid = find_next_pid(active_pid);
}


/* find_next_pid
 *   Inputs: pid of currently active process
 *   Return Value: pid scheduler should switch to
 *   Function: helper function that starts at cur_pid, walks through pcb structs to find next active process to run */
uint32_t find_next_pid(int32_t active_pid){

    
    uint32_t pid = (active_pid + 1)%6; 
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

    return -1; 
}

