#ifndef _PCB_H
#define _PCB_H

#include "types.h"
#include "rtc.h"
#include "file.h"
#include "filedir.h"



//typedef int32_t (*open_func_ptr)(const uint8_t* filename);
typedef int32_t (*close_func_ptr)(int32_t fd);
typedef int32_t (*read_func_ptr)(int32_t fd, void* buf, int32_t nbytes);
typedef int32_t (*write_func_ptr)(int32_t fd, const void* buf, int32_t nbytes);

typedef struct file_op_func_t{

    //open_func_ptr open_func;
    close_func_ptr close_func;
    read_func_ptr read_func;
    write_func_ptr write_func; 

}file_op_func_t;

extern file_op_func_t* rtc_funcs; 
extern file_op_func_t* file_funcs; 
extern file_op_func_t* dir_funcs; 
extern file_op_func_t* term_funcs; 


typedef struct file_arr_entry_t{

    file_op_func_t* file_op_tbl_ptr;
    uint32_t inode;
    uint32_t file_pos;
    union{
        uint32_t flags; 
        struct{
            uint32_t in_use : 1;
            uint32_t reserved : 31;
        } __attribute__((packed));
    }; 
    
}file_arr_entry_t;


extern file_arr_entry_t file_array[8];

#endif
