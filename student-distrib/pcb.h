#ifndef _PCB_H
#define _PCB_H

#include "types.h"
#include "rtc.h"
#include "file.h"
#include "filedir.h"

typedef struct file_arr_entry_t{

    uint32_t file_op_tbl_ptr;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags; 

}file_arr_entry_t;


file_arr_entry_t file_array[8];


//int32_t (*rtc_func_ptrs[])(...) = {rtc_read, rtc_write, rtc_open, rtc_close}

#endif