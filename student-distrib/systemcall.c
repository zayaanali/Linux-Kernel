/* This code creates the IDT entry for systemcalls and defines the c handler for a systemcall. */

#include "systemcall.h"
#include "x86_desc.h"
#include "lib.h"
#include "system_s.h"
#include "filesystem.h"
#include "rtc.h"
#include "file.h"
#include "filedir.h"
#include "pcb.h"
#include "paging.h"

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

    file_array[2].file_op_tbl_ptr = &file_funcs; 
    file_array[2].inode = 38; 
    
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
int32_t halt(uint8_t status) {

}



/* Load and execute a program. Hands off processor to new program until termination. 
    *   Inputs: command - space-separated sequence of words, first word is filename of program to be executed. 
                            rest of the command (stripped of leading spaces) should be provded to new program on request via getargs system call
    *   Return Value: -1 on failure, 256 if program dies by exception, 0-255 if program executes halt system call (return value of halt)

 */
uint8_t args[128];

int32_t execute(const uint8_t* command) {
    uint8_t filename[256];
    int space_found=0;
    int i; 
    /* Parse the command */
    for (i = 0; i < strlen(command); i++) {
        if (command[i] == ' ') // check if space is found
            space_found = 1;
        else if (space_found == 0) // if no space found, add to filename
            filename[i] = command[i];
        else // if space found, add to args
            args[i] = command[i];
    }

    /* Allocate a single page for each task’s user-level memory */
    add_pid_page(0);
    add_pid_page(1);

    /* Load Memory with Program Image */


    
    


}


int32_t read(int32_t fd, void* buf, int32_t nbytes){

    return file_array[fd].file_op_tbl_ptr->read_func(fd, buf, nbytes);

}

int32_t write(int32_t fd, const void* buf, int32_t nbytes){

    return file_array[fd].file_op_tbl_ptr->write_func(fd, buf, nbytes);
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

    // mark file array entry as not in use

    // perhaps delete this line and just return 0
    return file_array[fd].file_op_tbl_ptr->close_func(fd);
}

int32_t getargs(uint8_t* buf, int32_t nbytes){

}

int32_t vidmap(uint8_t** screen_start){

}
