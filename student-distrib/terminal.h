#ifndef _TERMINAL_H
#define _TERMINAL_H


#include "types.h"
#include "keyboard.h"
#include "lib.h"

#define NUM_TERMINALS 3

#define TERM1_VIDMEM 0xB9000
#define TERM2_VIDMEM 0xBA000
#define TERM3_VIDMEM 0xBB000
#define FOUR_KB 0x1000
#define MAX_BUFFER_SIZE 128

extern int32_t TERMINAL_VIDMEM_PTR[3];


int32_t terminal_switch();

typedef struct terminal {
    char keyboard_buffer[MAX_BUFFER_SIZE];
    int buf_ptr;
    volatile int enter_pressed;
    int cursor_x, cursor_y;

    /*for RTC virtualization*/
    volatile int INT_FLAG;
    volatile int INT_COUNT;
    volatile int V_FREQ_NUM;
} terminal_t;


char terminal_buffer[MAX_BUFFER_SIZE];


terminal_t terminals[NUM_TERMINALS];

int cur_terminal;


int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);

int32_t terminal_switch(int new_term_idx);

void remap_vidmem(int new_term);


#endif
