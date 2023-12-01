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
int32_t term_cur_pid[3] = {0,1,2};

int32_t active_pid = -1; 
int32_t active_tid = -1;

int32_t find_next_pid(int32_t p);
//static uint8_t base_shells_opened = 0;
uint8_t base_shells_opened = 0;




/* switch_process
 *   Inputs: none
 *   Return Value: none
 *   Function: switches to next process in round-robin fashion when pit interrrupt occurs */
int32_t switch_process() {

    // save esp and ebp of current process
    register uint32_t s_esp asm("%esp");
    register uint32_t s_ebp asm("%ebp");
    
    if (base_shells_opened == 0) { // Opening the first base shell. The current process context does not need to be stored (switching from kernel)
        active_pid = 0;
        active_tid = 0;
        pcb_ptr[active_pid]->pid_in_use=1;
    } else if (base_shells_opened > 0 && base_shells_opened < 3) { // Opening second and third base shells. Current PID context needs to be stored
        pcb_ptr[active_pid]->esp = (uint32_t)s_esp;
        pcb_ptr[active_pid]->ebp = (uint32_t)s_ebp; 

        active_pid++;
        active_tid++;

        pcb_ptr[active_pid]->pid_in_use=1;
    } else { // all base shells open. Current PID context needs to 
        //return 0;
        pcb_ptr[active_pid]->esp = (uint32_t)s_esp;
        pcb_ptr[active_pid]->ebp = (uint32_t)s_ebp; 
        save_cursors(active_tid);
        // update active_pid and active_tid
        active_pid =  find_next_pid(active_pid);
        active_tid = pcb_ptr[active_pid]->t_id;
        restore_cursors(active_tid);
    }

    
   /* Remap vidmem to currently serviced */
   remap_vidmem_service();


    // change page base address and flush tlb
    page_dir[32].page_dir_entry_4mb_t.page_base_address = ((EIGHT_MB + (active_pid*FOUR_MB)) >> 22); // align the page_table address to 4MB boundary
    flush_tlb();

    
    /* Set TSS entries */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHT_MB - (active_pid)*EIGHT_KB);

    // get esp, ebp, and eip of next process
    register uint32_t new_esp = pcb_ptr[active_pid]->esp;
    register uint32_t new_ebp = pcb_ptr[active_pid]->ebp;


    if (base_shells_opened <3) {        
       // base_shells_opened++;    
        
        terminal_switch(2-active_pid); // open 2, 1, then 0 (starts on terminal 0)
        send_eoi(0);
        printf("Starting shell %d\n", 2-active_pid);
        execute((const uint8_t*)"shell");
        return 0;
    }

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

/* find_next_pid
 *   Inputs: pid of currently active process
 *   Return Value: pid scheduler should switch to
 *   Function: helper function that starts at cur_pid, walks through pcb structs to find next active process to run */
int32_t find_next_pid(int32_t p){

    uint32_t pid = (p+1)%6; 
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

