#include "pcb.h"
#include "rtc.h"
#include "file.h"
#include "filedir.h"
#include "terminal.h"
#include "systemcall.h"
#include "scheduler.h"

#define FILE_ARRAY_SIZE 8

file_op_func_t rtc_funcs; 
file_op_func_t file_funcs; 
file_op_func_t dir_funcs; 
file_op_func_t term_funcs; 

int i = 0x7fe000;
pcb_entry_t* pcb_ptr[MAX_PROCESSES];

/* pcb_init
 *   Inputs: none
 *   Return Value: none
 *   Function: Initializes things for pcb structures. Sets up func operations table for each file type and pcb struct pointers.
*/
void pcb_init(){
    //rtc_funcs->open_func = rtc_open;
    rtc_funcs.close_func = rtc_close;
    rtc_funcs.read_func = rtc_read;
    rtc_funcs.write_func = rtc_write;

   // file_funcs.open_func = file_open;
    file_funcs.close_func = file_close;
    file_funcs.read_func = file_read;
    file_funcs.write_func = file_write;

    //dir_funcs.open_func = dir_open;
    dir_funcs.close_func = dir_close;
    dir_funcs.read_func = dir_read;
    dir_funcs.write_func = dir_write;


    term_funcs.close_func = terminal_close;
    term_funcs.read_func = terminal_read;
    term_funcs.write_func = terminal_write;

    // set up pcb pointers
    int i; 

    for(i=0; i<MAX_PROCESSES; i++){
        // process 0 pcb starts 8kb above 8mb(kernel bottom), each following pcb starts 8kb above the last
        pcb_ptr[i] = (pcb_entry_t*)(EIGHT_MB - (i+1)*EIGHT_KB);
        pcb_ptr[i]->current = 0;
        pcb_ptr[i]->pid_in_use = 0;
    }
}


/* insert_into_file_array
 *   Inputs: file_funcs_ptr:    ptr to func options that should be inserted into fd entry
 *           inode:             if inserting reg file, inode of that file, else is sent as -1 (or some other invalid #) and ignored
 *   Return Value:  fd of new file array entry if successful, -1 on failure
 *   Function: inserts a new entry into current pcb's file array if possible
*/
uint32_t insert_into_file_array(file_op_func_t* file_funcs_ptr, uint32_t inode){

    uint8_t k; // iteration variable

    // find available file array entry
    for(k=2; k<FILE_ARRAY_SIZE; k++){
        if(pcb_ptr[active_pid]->fd_array[k].in_use !=1){
            // create file array entry here
            pcb_ptr[active_pid]->fd_array[k].in_use = 1;
            pcb_ptr[active_pid]->fd_array[k].file_pos = 0; 
            pcb_ptr[active_pid]->fd_array[k].file_op_tbl_ptr = file_funcs_ptr;
            if(inode>=0 && inode<=63){
                pcb_ptr[active_pid]->fd_array[k].inode = inode; 
            }

            return k;   // return fd
        }
    }

    // no available file array entry
    //printf("ERR in insert_into_file_array: no room to open this file \n");
    return -1; 
}

/* remove_from_file_array
 *   Inputs: fd:    fd of file array entry to close
 *   Return Value:  0 if successfully closed, -1 on failure (fd invalid or already closed)
 *   Function: removes file array entry from current pcb's file array if possible
*/
uint32_t remove_from_file_array(int32_t fd){

    if(fd<0 || fd>7){
        printf("ERR trying to remove from file array with invalid fd. Given fd: %d \n", fd);
        return -1; 
    }

    if(pcb_ptr[active_pid]->fd_array[fd].in_use!=1){
        printf("ERR cannot remove file array entry at fd: %d .File array entry already removed \n", fd);
        return -1; 
    }

   pcb_ptr[active_pid]->fd_array[fd].in_use = 0;
    return 0;
}
