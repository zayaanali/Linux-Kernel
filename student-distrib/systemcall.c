/* This code creates the IDT entry for systemcalls and defines the c handler for a systemcall. */

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

/* This link function is defined externally, in system_s.S. This function will call the defined .c systemcall_handler below */
extern void systemcall_link(); 


/* init_syscall_idt
 *   Inputs: none
 *   Return Value: none
 *   Function: Initialize IDT entry corresponding to systemcall vector (vector 0x80) */
void init_syscall_idt(){

    // systemcall should be in IDT entry 0x80 = 128
    // set reserved0|size|reserved1|reserved2|reserved3|reserved4[8] to 0 1111 0000 0000 for 32-bit trap gate
    idt[128].present = 1;             // segment is present
    idt[128].dpl = 0;                 // DPL = 00 (highest priority)
    idt[128].reserved0 = 0;           
    idt[128].size = 1;                // size (D) = 1 (32 bit gate)
    idt[128].reserved1 = 1;
    idt[128].reserved2 = 1;
    idt[128].reserved3 = 1;
    idt[128].reserved4 = 0;
    idt[128].seg_selector = KERNEL_CS;

    // set offset fields so that this gate points to assembly systemcall handler for the handler function 
    SET_IDT_ENTRY(idt[128], systemcall_link);

    
}


/* systemcall_handler
 *   Inputs: none
 *   Return Value: none
 *   Function: Handler for any systemcall */
int32_t systemcall_handler(uint8_t syscall, int32_t arg1, int32_t arg2, int32_t arg3){
    printf("A system call was called. \n");

    switch(syscall){
        case SYS_HALT:
            break;
        case SYS_EXECUTE:
            // 
            break; 
        case SYS_READ:
            read(arg1, (void*)arg2, arg3);
            break;
        case SYS_WRITE:
            write(arg1, (const void*)arg2, arg3);
            break;
        case SYS_OPEN:
            open((const uint8_t*)arg1);
            break;
        case SYS_CLOSE:
            close(arg1);
            break;
        case SYS_GETARGS:
            getargs((uint8_t*)arg1, arg2);
            break; 
        case SYS_VIDMAP:    
            vidmap((uint8_t**)arg1);
            break; 
        case SYS_SET_HANDLER:
            break;
        case SYS_SIGRETURN:
            break;
        default:
            return -1; //not a valid syscall
    }
}

    
/* Terminates process, return to parent process. 
    *   Inputs: status - 8-bit value to be returned to parent process
    *   Return Value: -1 on failure, 0 on success
 */

int cur_pid = -1; // -1 before any processes are open 

/* Need to restore parent data, restore parent paging, close relevant FDs, jump to execute return */
int32_t halt(uint8_t status) {
    int i;
    
    /* Get the current and parent pcb */
    pcb_entry_t* cur_pcb = (pcb_entry_t*) (EIGHT_MB - (cur_pid+1)*EIGHT_KB);
    pcb_entry_t* parent_pcb = (pcb_entry_t*) (EIGHT_MB - (parent_pid+1)*EIGHT_KB);
    int parent_pid = cur_pcb->parent_pid;
    pcb_entry_t* parent_pcb = (pcb_entry_t*) (EIGHT_MB - (parent_pid+1)*EIGHT_KB);

    
    /* If current PID is base shell, then relaunch shell */
    if (cur_pid == 0) {
        cur_pcb->parent_pid = 0;
        execute((uint8_t*)"shell");
    }

    /* Restore parent data */
    cur_pid = parent_pid;

    /* Restore paging (DOUBLE CHECK) */
    add_pid_page(cur_pid); // for the 

    /* Flush TLB */
    flush_tlb();

    /* Close relevant FDs (DOUBLE CHECK) */
    for (i=0; i<MAX_FD_ENTRIES; i++) {
        cur_pcb->fd_array->flags.in_use = 0;
    }

    cur_pcb->pid = cur_pid;

    /* Restore TSS */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHT_MB - (parent_pid)*EIGHT_KB);

    


    /* Somehow return? */
}



/* Load and execute a program. Hands off processor to new program until termination. 
    *   Inputs: command - space-separated sequence of words, first word is filename of program to be executed. 
                            rest of the command (stripped of leading spaces) should be provded to new program on request via getargs system call
    *   Return Value: -1 on failure, 256 if program dies by exception, 0-255 if program executes halt system call (return value of halt)

 */
uint8_t args[128];
uint8_t ELF[] = {0177, 'E', 'L', 'F'};

int32_t execute(const uint8_t* command) {
    uint8_t filename[32] = "";
    int space_found=0;
    int i;
    uint8_t read_buffer[4];
    uint32_t new_eip; 

    /* Increment the cur pid */
    cur_pid++;

    /* Parse the command */
    for (i = 0; i < strlen((const int8_t*)command); i++) {
        if (command[i] == ' ') // check if space is found
            space_found = 1;
        else if (space_found == 0) // if no space found, add to filename
            filename[i] = command[i];
        else // if space found, add to args
            args[i] = command[i];
    }

    /* get inode if valid file to use for read data */
    dentry_t dentry[1];
    i = read_dentry_by_name(filename, dentry);
    if(i==-1){
        printf("execute: File doesn't exist \n");
        return -1; 
    }

    /* read first 4 bytes (check for del and ELF const)*/
    read_data(dentry->inode_id, 0, read_buffer, 4);
    
    /* Check that file is executable */
    for (i=0; i<ELF_SIZE; i++) {
        if (ELF[i] != read_buffer[i])
            { printf("execute: Not an executable \n"); return -1; }
    }

    /* Check not at max processes */
    if (cur_pid > 5)
        { printf("execute: Six processes already open \n"); return -1; }
    
    /* Add PID page */
    add_pid_page(cur_pid);

    /* Flush TLB */
    flush_tlb();

    /* Load Memory with Program Image -- Use virtual address of 128 MB */
    uint8_t* p_img = (uint8_t*)(0x8000000);
    read_data(dentry->inode_id, 0, p_img, 36000); // 36000 is well over the max size of file (in bytes)

    /* Create PCB entry */
    pcb_entry_t cur_pcb; 
    cur_pcb.pid=cur_pid;

    /* Need to figure out how to fill these out */
    cur_pcb.parent_pid = (cur_pid > 0) ? cur_pid-1 : cur_pid; // what is parent pid     
    cur_pcb.state = 1;      //active
    cur_pcb.priority = 0; 
    //cur_pcb.registers = [];


    //initialize other file array entries to not in use
    for(i=0; i<8; i++){
        cur_pcb.fd_array[i].in_use = 0; 
    }

    /* Copy PCB to memory */
    pcb_entry_t* new_pcb = pcb_ptr[cur_pid];
    memcpy(new_pcb, &cur_pcb, sizeof(pcb_entry_t));
    
    // set up stdin and stdout
    terminal_open((const uint8_t*)"");

    // save old esp0
    new_pcb->parent_esp0 = tss.esp0;
    
    /* Set TSS entries */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHT_MB - (cur_pid)*EIGHT_KB);

    

    /* do after pcb put into kernel since this asm function saves to kernel space*/
    //save_parent_regs_to_pcb();

    memcpy(&new_eip, (p_img+24),4);            // add 24 to uint8_t pointer to get to bytes 24-27 of program image

    //pcb_ptr->p_esp=tss.esp0;
    register uint32_t s_esp asm("%esp"); 
    register uint32_t s_ebp asm("%ebp"); 

    new_pcb->ebp = s_ebp;
    new_pcb->esp = s_esp; 

    uint32_t l_USER_DS = (0|USER_DS)&(0x0ffff);
    uint32_t l_USER_CS = (0|USER_CS)&(0x0ffff); 
    /* Context Switch */
    asm volatile(
        "pushl %0;"                 // push operand 0, USER_DS
        "pushl $0x8400000;"
        "pushfl;"                   // push flags
        "pushl %1;"                 // push operand 1, USER_CS
        "pushl %2;"                 // push operand 2, eip of program to run 
        "iret;"
        "ret;"                      // need "leave" as well?
        :                                           // no outputs
        : "r"(l_USER_DS), "r"(l_USER_CS), "r"(new_eip)       // inputs
     );



}


int32_t read(int32_t fd, void* buf, int32_t nbytes){

    return pcb_ptr[cur_pid]->fd_array[fd].file_op_tbl_ptr->read_func(fd, buf, nbytes);

}

int32_t write(int32_t fd, const void* buf, int32_t nbytes){

    return pcb_ptr[cur_pid]->fd_array[fd].file_op_tbl_ptr->write_func(fd, buf, nbytes);
}

int32_t open(const uint8_t* filename){
    dentry_t dentry[1];

    // create array of function pointers with int32_t type (since all these functions return int32_t). all functions have one arg of type const uint8_t*
    int32_t (*func_open_table[])(const uint8_t*) = {rtc_open, dir_open, file_open};

    // call correct open function depending on file type
    // get dentry to know file type
    int32_t i = read_dentry_by_name(filename, dentry);

    if(i==-1){
        return i;   // invalid filename
    }

    // call correct open function given filetype
    return func_open_table[dentry->file_type](filename);
    
}

int32_t close(int32_t fd){

    // check that fd is valid
    if(fd<0 || fd >7){
        printf("ERR: invalid fd given in close function \n");
        return -1; 
    }

    if(pcb_ptr[cur_pid]->fd_array[fd].in_use!=1){
        printf("ERR: trying to perform close on fd that's not in use \n");
        return -1; 
    }

    // call close func -- should remove from fd array
    return pcb_ptr[cur_pid]->fd_array[fd].file_op_tbl_ptr->close_func(fd);
}

int32_t getargs(uint8_t* buf, int32_t nbytes){

}

int32_t vidmap(uint8_t** screen_start){

}

