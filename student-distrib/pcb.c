#include "pcb.h"
#include "rtc.h"
#include "file.h"
#include "filedir.h"
#include "terminal.h"

#define FILE_ARRAY_SIZE 8

file_op_func_t rtc_funcs; 
file_op_func_t file_funcs; 
file_op_func_t dir_funcs; 
file_op_func_t term_funcs; 

file_arr_entry_t file_array[8];

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

    // set up stdin entry of file array
    // set up stdout entry of file array
}

uint32_t insert_into_file_array(file_op_func_t* file_funcs_ptr, uint32_t inode){

    uint8_t k; // iteration variable

    // find available file array entry
    for(k=2; k<FILE_ARRAY_SIZE; k++){
        if(file_array[k].in_use !=1){
            // create file array entry here
            file_array[k].in_use = 1;
            file_array[k].file_pos = 0; 
            file_array[k].file_op_tbl_ptr = file_funcs_ptr;
            if(inode>=0 && inode<=63){
                file_array[k].inode = inode; 
            }

            return k;   // return fd
        }
    }

    // no available file array entry
    printf("no room to open this file \n");
    return -1; 
}


uint32_t remove_from_file_array(int32_t fd){

    if(fd<0 || fd>7){
        printf("ERR trying to remove from file array with invalid fd. Given fd: %d \n", fd);
        return -1; 
    }

    if(file_array[fd].in_use!=1){
        printf("ERR cannot remove file array entry at fd: %d .File array entry already removed \n", fd);
        return -1; 
    }

    file_array[fd].in_use = 0;
    return 0;
}
