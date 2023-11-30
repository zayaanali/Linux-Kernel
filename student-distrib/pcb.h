#ifndef _PCB_H
#define _PCB_H

#include "types.h"
#include "rtc.h"
#include "file.h"
#include "filedir.h"
#include "keyboard.h"

#define MAX_FD_ENTRIES 8
#define NUM_REGS 10
#define MAX_PROCESSES 6

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

extern file_op_func_t rtc_funcs; 
extern file_op_func_t file_funcs; 
extern file_op_func_t dir_funcs; 
extern file_op_func_t term_funcs; 


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

typedef struct pcb_entry{
    /* current regs*/
    uint32_t esp;
    uint32_t ebp;

    uint32_t esp_exec;
    uint32_t ebp_exec;

    /* Current Task Info */
    file_arr_entry_t fd_array[MAX_FD_ENTRIES];


    /* current args */
    unsigned char args[MAX_BUFFER_SIZE];

    /* Parent Data */
    int32_t parent_pid;
    uint32_t parent_esp0;

    // thread id. matches 0 indexed terminal this process is on
    uint8_t t_id; 

    // flag to track if this process is current process of it's thread
    uint8_t current;

    // flag to track if this pcb currently being used
    uint8_t pid_in_use;


}pcb_entry_t;

extern pcb_entry_t* pcb_ptr[MAX_PROCESSES]; 

extern uint32_t insert_into_file_array(file_op_func_t* file_funcs_ptr, uint32_t inode);
extern uint32_t remove_from_file_array(int32_t fd);

extern void pcb_init();

#endif
