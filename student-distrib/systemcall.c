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
    idt[128].dpl = 3;                 // DPL = 00 (highest priority)
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
    uint8_t ignore;
    switch(syscall){
        case SYS_HALT:
            halt(ignore);
            break;
        case SYS_EXECUTE:
            execute((const uint8_t*) arg1);
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

cur_pid = -1; // -1 before any processes are open
parent_pid = -1; // -1 before any processes are open

/* Need to restore parent data, restore parent paging, close relevant FDs, jump to execute return */
int32_t halt(uint8_t status) {
    int i;
    
    /* Get the current and parent pcb */
    pcb_entry_t* cur_pcb = (pcb_entry_t*) (EIGHT_MB - (cur_pid+1)*EIGHT_KB);
    pcb_entry_t* parent_pcb = (pcb_entry_t*) (EIGHT_MB - (parent_pid+1)*EIGHT_KB);

    /* Close relevant FDs */
    for(i=0; i<8; i++)
        cur_pcb->fd_array[i].in_use = 0; 

    /* If current PID is base shell, then relaunch shell */
    if (cur_pid == 0) { 
        parent_pid = -1; cur_pid = -1; // cur_pid will be incremented in execute
        execute((uint8_t*)"shell"); 
    }
    
    /* Update cur/parent pid. Parent_pid should only be negative with base shell (case above) */
    if (parent_pid < 0) printf("ERR: parent_pid is negative \n"); // sanity check
    cur_pid = parent_pid;
    parent_pid--;
    if (parent_pid < 0) printf("ERR: parent_pid is negative \n"); // sanity check

    /* Add PID page */
    page_dir[32].page_dir_entry_4mb_t.present = 1;
    page_dir[32].page_dir_entry_4mb_t.read_write = 1;
    page_dir[32].page_dir_entry_4mb_t.user_supervisor = 1;
    page_dir[32].page_dir_entry_4mb_t.page_write_through = 0;
    page_dir[32].page_dir_entry_4mb_t.page_cache_disable = 0;
    page_dir[32].page_dir_entry_4mb_t.accessed = 0;
    page_dir[32].page_dir_entry_4mb_t.dirty = 0;
    page_dir[32].page_dir_entry_4mb_t.page_size = 1; // 4MB page
    page_dir[32].page_dir_entry_4mb_t.global = 0;
    page_dir[32].page_dir_entry_4mb_t.avail = 0;
    page_dir[32].page_dir_entry_4mb_t.PAT = 0;
    page_dir[32].page_dir_entry_4mb_t.reserved = 0;
    page_dir[32].page_dir_entry_4mb_t.page_base_address = (0x800000 + (cur_pid*0x400000) >> 22); // align the page_table address to 4MB boundary

    /* Flush TLB */
    flush_tlb();


    cur_pcb->pid = cur_pid;
    cur_pcb->parent_pid = parent_pid;

    /* Restore TSS */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHT_MB - (cur_pid)*EIGHT_KB);

    /* restore stack */
    register uint32_t s_esp = cur_pcb->esp;
    register uint32_t s_ebp = cur_pcb->ebp; 

    /* Context Switch */
    asm volatile(
        "movl %0, %%esp; \n"                 // push operand 0, USER_DS
        "movl %1, %%ebp; \n"
        "jmp return_label; \n"
    
        :                                           // no outputs
        : "r"(s_esp), "r"(s_ebp)       // inputs
     );

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
    int i, ret;
    uint8_t read_buffer[4];
    uint32_t entry_point;
    dentry_t *new_dentry;

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
    if(read_dentry_by_name(filename, new_dentry) == -1)
        { printf("execute: File doesn't exist \n"); return -1; }

    /* read first 4 bytes (check for del and ELF const)*/
    read_data(new_dentry->inode_id, 0, read_buffer, 4);
    
    /* Check that file is executable */
    for (i=0; i<ELF_SIZE; i++) {
        if (ELF[i] != read_buffer[i])
            { printf("execute: Not an executable \n"); return -1; }
    }

    /* Check not at max processes */
    if (cur_pid >= 5)
        { printf("execute: Six processes already open \n"); return -1; }
    
    /* Set current/parent pid */    
    cur_pid++;
    if (cur_pid >= 1) // process 1 and above has a parent process
        parent_pid = cur_pid-1;
    else // process 0 (base shell) has no parent process
        parent_pid = -1;
    
    /* Add PID page */
    page_dir[32].page_dir_entry_4mb_t.present = 1;
    page_dir[32].page_dir_entry_4mb_t.read_write = 1;
    page_dir[32].page_dir_entry_4mb_t.user_supervisor = 1;
    page_dir[32].page_dir_entry_4mb_t.page_write_through = 0;
    page_dir[32].page_dir_entry_4mb_t.page_cache_disable = 0;
    page_dir[32].page_dir_entry_4mb_t.accessed = 0;
    page_dir[32].page_dir_entry_4mb_t.dirty = 0;
    page_dir[32].page_dir_entry_4mb_t.page_size = 1; // 4MB page
    page_dir[32].page_dir_entry_4mb_t.global = 0;
    page_dir[32].page_dir_entry_4mb_t.avail = 0;
    page_dir[32].page_dir_entry_4mb_t.PAT = 0;
    page_dir[32].page_dir_entry_4mb_t.reserved = 0;
    page_dir[32].page_dir_entry_4mb_t.page_base_address = (0x800000 + (cur_pid*0x400000) >> 22); // align the page_table address to 4MB boundary

    /* Flush TLB */
    flush_tlb();

    /* Load Memory with Program Image -- Use virtual address of 128 MB */
    uint8_t* p_img = (uint8_t*)((0x08048000));

    read_data(new_dentry->inode_id, 0, p_img, 36000); // 36000 is well over the max size of file (in bytes)

    /* Read entry point from program file */
    read_data(new_dentry->inode_id, 24, (uint8_t*) &entry_point, 4); // 24 is the offset of the entry point in the file

    /* Set up PCB entry */
    pcb_entry_t* pcb_addr = pcb_ptr[cur_pid]; 
    pcb_entry_t pcb;
    pcb.pid = cur_pid;
    pcb.parent_pid = parent_pid;
    pcb.parent_esp0 = tss.esp0;


    //initialize other file array entries to not in use
    for(i=2; i<8; i++)
        pcb.fd_array[i].in_use = 0;

    /* Save EBP/ESP and Copy PCB to memory */
    register uint32_t s_esp asm("%esp"); 
    register uint32_t s_ebp asm("%ebp"); 
    pcb.ebp = s_ebp;
    pcb.esp = s_esp; 
    
    memcpy((void*)pcb_addr, (const void*)&pcb, sizeof(pcb_entry_t));

    /* Set up stdin and stdout */
    terminal_open((const uint8_t*)"");
    
    /* Set TSS entries */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHT_MB - (cur_pid)*EIGHT_KB);

    uint32_t user_ds_ext = (0|USER_DS)&(0x0ffff);
    uint32_t user_cs_ext = (0|USER_CS)&(0x0ffff); 

    uint32_t user_esp = KERNEL_BASE + FOUR_MB - 4;
    
    /* Enable interrupts (interrupt switch) */
    // sti(); 
    
    /* Context Switch */
    asm volatile(
        "pushl %0; \n"             // set data segment register
        "pushl %1; \n"
        "pushfl; \n"
        "popl %%ecx; \n"
        "orl $0x200, %%ecx; \n"          // Set IF flag to one                 
        "pushl %%ecx; \n"
        "pushl %2; \n"                   // push flags
        "pushl %3; \n"                 // push operand 2, eip of program to run 
        "iret; \n"
        "return_label: \n"
        "leave; \n"
        "ret; \n"

        :                                           // no outputs
        : "g" (USER_DS), "g" (user_esp), "g" (USER_CS), "g" (entry_point)        // inputs
        : "memory", "cc", "ecx"
     );

     



    return 0; 
/*        "iret;"
        "execute_ret:"
        "ret;"                      // need "leave" as well?*/
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

