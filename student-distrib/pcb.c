#include "pcb.h"
#include "rtc.h"
#include "file.h"
#include "filedir.h"
#include "terminal.h"


file_op_func_t* rtc_funcs; 
file_op_func_t* file_funcs; 
file_op_func_t* dir_funcs; 
file_op_func_t* term_funcs; 

file_arr_entry_t file_array[8];

void pcb_init(){
    //rtc_funcs->open_func = rtc_open;
    rtc_funcs->close_func = rtc_close;
    rtc_funcs->read_func = rtc_read;
    rtc_funcs->write_func = rtc_write;

   // file_funcs->open_func = file_open;
    file_funcs->close_func = file_close;
    file_funcs->read_func = file_read;
    file_funcs->write_func = file_write;

    //dir_funcs->open_func = dir_open;
    dir_funcs->close_func = dir_close;
    dir_funcs->read_func = dir_read;
    dir_funcs->write_func = dir_write;


    term_funcs->close_func = terminal_close;
    term_funcs->read_func = terminal_read;
    term_funcs->write_func = terminal_write;

    // set up stdin entry of file array
    // set up stdout entry of file array
}