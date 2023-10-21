#ifndef _TERMINAL_H
#define _TERMINAL_H


#include "types.h"
#include "keyboard.h"
#include "lib.h"


char terminal_buffer[MAX_BUFFER_SIZE];

uint32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
uint32_t terminal_write(int fd, const void* buf, int32_t nbytes);
uint32_t terminal_open(const uint8_t* filename);
uint32_t terminal_close(int32_t fd);



#endif






