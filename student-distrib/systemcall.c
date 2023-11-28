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
#include "excepts.h"
#include "scheduler.h"

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
int32_t systemcall_handler(int32_t syscall, int32_t arg1, int32_t arg2, int32_t arg3){

    switch(syscall){
        case SYS_HALT:
            return halt((uint8_t)arg1);
            break;
        case SYS_EXECUTE:
            return execute((const uint8_t*) arg1);
            break; 
        case SYS_READ:
            return read(arg1, (void*)arg2, arg3);
            break;
        case SYS_WRITE:
            return write(arg1, (const void*)arg2, arg3);
            break;
        case SYS_OPEN:
            return open((const uint8_t*)arg1);
            break;
        case SYS_CLOSE:
            return close(arg1);
            break;
        case SYS_GETARGS:
            return getargs((uint8_t*)arg1, arg2);
            break; 
        case SYS_VIDMAP:    
            return vidmap((uint8_t**)arg1);
            break; 
        default:
            return -1; //not a valid syscall
    }

}

    
/* halt
    *   Function: Terminates process, return to parent process. 
    *   Inputs: status - 8-bit value to be returned to parent process
    *   Return Value: -1 on failure, 0 on success
 */

int cur_pid = -1; // -1 before any processes are open

/* Need to restore parent data, restore parent paging, close relevant FDs, jump to execute return */
int32_t halt(uint8_t status) {
    uint32_t flags;
    cli_and_save(flags);
    
    int i;
    
    /* Get the current and parent pcb */
    pcb_entry_t* cur_pcb = (pcb_entry_t*) (EIGHT_MB - (cur_pid+1)*EIGHT_KB);

    /* Close relevant FDs */
    for(i=0; i<8; i++)
        cur_pcb->fd_array[i].in_use = 0; 

    /* mark process as not current */
    cur_pcb->current = 0;

    /* If current PID is base shell, then relaunch shell */
    if (cur_pid >= 0 && cur_pid <=2) { 
        // CHANGE
        cur_pid --; // cur_pid will be incremented in execute
        execute((const uint8_t*)"shell"); 
    }
    
    /* Update cur pid to parent, mark parent as current*/
    cur_pid = cur_pcb->parent_pid;
    cur_pcb->current = 1; 
    //cur_pcb->pid = cur_pid;

    /* restore PID page */
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
    page_dir[32].page_dir_entry_4mb_t.page_base_address = ((EIGHT_MB + (cur_pid*FOUR_MB)) >> 22); // align the page_table address to 4MB boundary

    /* mark vidmem page as not present*/
    video_page_table[0].present = 0;

    /* Flush TLB */
    flush_tlb();

    /* Restore TSS */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHT_MB - (cur_pid)*EIGHT_KB);

    /* restore stack */
    register uint32_t s_esp = cur_pcb->esp_exec;
    register uint32_t s_ebp = cur_pcb->ebp_exec; 

    restore_flags(flags);
    /* Context Switch */
    asm volatile(
        "movl %0, %%esp; \n"                 // restore esp
        "movl %1, %%ebp; \n"                 // restore ebp
        "movsx %%bl, %%ebx; \n"
        "movl %%ebx, %%eax; \n"              //expand 8bit arg from BL into 32-bit return val
        "jmp return_label; \n"
    
        :                                    // no outputs
        : "r"(s_esp), "r"(s_ebp)                // inputs
     );

    return 0;   // to silence compiler warning
}



/* Load and execute a program. Hands off processor to new program until termination. 
    *   Inputs: command - space-separated sequence of words, first word is filename of program to be executed. 
                            rest of the command (stripped of leading spaces) should be provded to new program on request via getargs system call
    *   Return Value: -1 on failure, 256 if program dies by exception, 0-255 if program executes halt system call (return value of halt)

 */

uint8_t ELF[] = {0177, 'E', 'L', 'F'};

int32_t execute(const uint8_t* command) {
    uint8_t args[128];
    
    uint32_t flags;
    cli_and_save(flags);
   
    uint8_t filename[32] = "";
    int space_found=0;
    int i, j;
    uint8_t read_buffer[4];
    uint32_t entry_point;
    dentry_t new_dentry;
    int32_t caller_pid; 
    int32_t parent_pid;

    /* Init args array to NULL char */
    for (i=0; i<MAX_BUFFER_SIZE; i++) {
        args[i]='\0';
    }
    
    /* Parse filename/arguments */
    j=0;
    for (i = 0; i < strlen((const int8_t*)command); i++) {
        if (command[i] == ' ') // check if space is found
            space_found = 1;
        else if (space_found == 0) // if no space found, add to filename
            filename[i] = command[i];
        else {                      // if space found, add to args
            args[j] = command[i];
            j++;
        }
    }
    

    /* get inode if valid file to use for read data */
    if(read_dentry_by_name(filename, &new_dentry) == -1)
        { printf("execute: File doesn't exist \n"); return -1; }

    /* read first 4 bytes (check for del and ELF const)*/
    read_data(new_dentry.inode_id, 0, read_buffer, 4);
    
    /* Check that file is executable */
    for (i=0; i<ELF_SIZE; i++) {
        if (ELF[i] != read_buffer[i])
            { printf("execute: Not an executable \n"); return -1; }
    }

    /* Check not at max processes */
    if (cur_pid >= 5)
        { printf("execute: Six processes already open \n"); return -1; }
    
    /* Set parent pid, mark parent as not current */  
    caller_pid = active_pid; 
    cur_pid++;  

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
    page_dir[32].page_dir_entry_4mb_t.page_base_address = ((EIGHT_MB + (cur_pid*FOUR_MB)) >> 22); // align the page_table address to 4MB boundary

    /* Flush TLB */
    flush_tlb();

    /* Load Memory with Program Image -- Use virtual address of 128 MB */
    uint8_t* p_img = (uint8_t*)((0x08048000)); // 128MB + offset of 0x48000

    read_data(new_dentry.inode_id, 0, p_img, 36000); // 36000 is well over the max size of file (in bytes)

    /* Read entry point from program file */
    read_data(new_dentry.inode_id, 24, (uint8_t*) &entry_point, 4); // 24 is the offset of the entry point in the file, read 4 bytes

    /* Set up PCB entry */
    pcb_entry_t* pcb_addr = pcb_ptr[cur_pid]; 
    pcb_entry_t pcb;
    pcb.pid = cur_pid;
    pcb.parent_esp0 = tss.esp0;
    pcb.current = 1;

    if (cur_pid >= 3){ // process 3 and above have a parent process
        pcb.parent_pid = caller_pid;
        pcb_ptr[parent_pid]->current = 0; 
        pcb.t_id = pcb_ptr[parent_pid]->t_id;
    } 
    else{ // process 0, 1, 2 (base shells) have no parent process
        pcb.parent_pid = -1;
        pcb.t_id = cur_pid; 
        pcb.current = 1;
    } 

    /* Copy arguments to PCB args value */
    for (i=0; i<MAX_BUFFER_SIZE; i++) {
        pcb.args[i] = args[i];
    }


    //initialize file array entries to not in use
    for(i=0; i<8; i++)
        pcb.fd_array[i].in_use = 0;

    /* Save EBP/ESP and Copy PCB to memory */

    
    memcpy((void*)pcb_addr, (const void*)&pcb, sizeof(pcb_entry_t));

    /* Set up stdin and stdout */
    terminal_open((const uint8_t*)"");


    uint32_t user_esp = KERNEL_BASE + FOUR_MB - 4;
    register uint32_t ret;
    
    register uint32_t s_esp asm("%esp"); 
    register uint32_t s_ebp asm("%ebp"); //reg values volatile?
    pcb_ptr[cur_pid]->esp_exec = s_esp;
    pcb_ptr[cur_pid]->ebp_exec = s_ebp;

    /* Set TSS entries */
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHT_MB - (cur_pid)*EIGHT_KB);
    
    /* Enable interrupts (interrupt switch) */
    restore_flags(flags);

    /* Context Switch */
    asm volatile(
        "pushl %1; \n"             // set data segment register
        "pushl %2; \n"
        "pushfl; \n"
        "popl %%ecx; \n"
        "orl $0x200, %%ecx; \n"          // Set IF flag to one                 
        "pushl %%ecx; \n"
        "pushl %3; \n"                   // push flags
        "pushl %4; \n"                 // push operand 2, eip of program to run 
        "iret; \n"
        "return_label: \n"
        "movl %%eax, %0; \n"     
        
        : "=r" (ret)                                          // no outputs
        : "g" (USER_DS), "g" (user_esp), "g" (USER_CS), "g" (entry_point)        // inputs
        : "memory", "cc", "ecx"
     );

    /* if an exception occurs*/
    if(exception_flag == 1){
        return 256;
    } 
    /* return program call*/
    return ret;
}


/* read
 *   Inputs: fd:    file descriptor of file to read
 *           buf:   buffer to store read data
 *           nbytes:number of bytes to read
 *   Return Value: number of bytes sucessfully read
 *   Function: reads from indicated file
*/
int32_t read(int32_t fd, void* buf, int32_t nbytes){

    if(fd<0 || fd>7){
        return -1; 
    }

    if(fd==1){
        //cannot read from stdout, is write-only
        return -1; 
    }

    if(pcb_ptr[cur_pid]->fd_array[fd].in_use!=1){
        //printf("ERR in read: trying to read from fd %d which is not in use \n", fd);
        return -1; 
    }

    return pcb_ptr[cur_pid]->fd_array[fd].file_op_tbl_ptr->read_func(fd, buf, nbytes);

}


/* write
 *   Inputs: fd:    file descriptor of file to write to
 *           buf:   buffer holding data to write
 *           nbytes:number of bytes to write
 *   Return Value: number of bytes sucessfully written, -1 on failure
 *   Function: writes to inidicated file
*/
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    
    if(fd<0 || fd>7){
        return -1; 
    }

    if(fd==0){
        // cannot write to stdin -- is read-only
        return -1;
    }

    if(pcb_ptr[cur_pid]->fd_array[fd].in_use!=1){
        //printf("ERR in write: trying to write to fd %d which is not in use \n", fd);
        return -1; 
    }

    return pcb_ptr[cur_pid]->fd_array[fd].file_op_tbl_ptr->write_func(fd, buf, nbytes);
}


/* open
 *   Inputs: filename:  name of file to open
 *   Return Value: fd of opened file if successful, -1 on failure
 *   Function: opens indicated file
*/
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


/* close
 *   Inputs: fd:    file descriptor of file to close
 *   Return Value: 0 on success, -1 on failure
 *   Function: closes indicated file
*/
int32_t close(int32_t fd){

    // check that fd is valid
    if(fd<0 || fd >7){
        //printf("ERR in close: invalid fd given in close function \n");
        return -1; 
    }

    // cannot close stdin or stdout
    if(fd==0 || fd==1){
        return -1; 
    }

    if(pcb_ptr[cur_pid]->fd_array[fd].in_use!=1){
       // printf("ERR in close: trying to perform close on fd that's not in use \n");
        return -1; 
    }

    // call close func -- should remove from fd array
    return pcb_ptr[cur_pid]->fd_array[fd].file_op_tbl_ptr->close_func(fd);
}


/* getargs
 *   Inputs: buf: buffer for argument to be parsed into
 *           nbytes: number of bytes to be copied
 *   Return Value: returns the argument
 *   Function: Arguments are parsed from the command buffer in execute

*/
int32_t getargs(uint8_t* buf, int32_t nbytes){
    
    /* Check buffer is valid */
    if (buf==NULL || nbytes<0)
        { printf("Invalid Argument to getargs"); return -1; }
    
    /* Get the current PCB */
    pcb_entry_t * cur_pcb = (pcb_entry_t*) pcb_ptr[cur_pid];

    /*If the arguments and a terminal NULL (0-byte) 
    do not fit in the buffer, simply return -1*/
    if(strlen((const int8_t *)cur_pcb->args) + 1 > nbytes){
        return -1;
    }

    strncpy((void *)buf, (void *)cur_pcb->args, nbytes);
    
    return 0;
}


/* vidmap
 *   Inputs: screen_start: points to start of video memory
 *   Return Value: 132MB address on success (from *screen_start), -1 on failure 
 *   Function: Maps video memory into user space at adress 132MB after prog img page
*/
int32_t vidmap(uint8_t** screen_start){
    cli();
    
    /* check for valid ptr or if screen_start in 4MB user page at 128MB */
    if(screen_start == NULL || (int)screen_start > ONETHIRTYTWO_MB || (int)screen_start < KERNEL_BASE){
        return -1;
    }

    /* Set the physical address */
    uint32_t video_page_addr = 0xB8000;  //page base address for entry (videomem)

    /* add 4kb video page (132MB/4MB = 33 for pd index) */ 
    page_dir[33].page_dir_entry_4kb_t.present = 1;
    page_dir[33].page_dir_entry_4kb_t.read_write = 1;
    page_dir[33].page_dir_entry_4kb_t.user_supervisor = 1;  
    page_dir[33].page_dir_entry_4kb_t.page_size = 0; // 4KB page size
    page_dir[33].page_dir_entry_4kb_t.page_table_base_address =  ((unsigned int)video_page_table) >> 12; // align the page_table address to 4KB boundary

    /* entry into page table */
    video_page_table[0].present = 1;
    video_page_table[0].page_cache_disable = 0;      
    video_page_table[0].read_write = 1;
    video_page_table[0].user_supervisor = 1;
    video_page_table[0].page_base_address = (video_page_addr) >> 12;

    /* Flush TLB */
    flush_tlb();

    /*set screen_start location in mem*/
    *screen_start = (uint8_t*)ONETHIRTYTWO_MB;

    sti();
    return 0;
}

