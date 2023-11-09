#ifndef _SYSTEMCALL_H
#define _SYSTEMCALL_H

#include "types.h"


#define SYS_HALT    1
#define SYS_EXECUTE 2
#define SYS_READ    3
#define SYS_WRITE   4
#define SYS_OPEN    5
#define SYS_CLOSE   6
#define SYS_GETARGS 7
#define SYS_VIDMAP  8
#define SYS_SET_HANDLER  9
#define SYS_SIGRETURN  10

#define ELF_SIZE 4
#define EIGHT_MB 0x800000
#define EIGHT_KB 0x2000
#define KERNEL_BASE 0x08000000
#define FOUR_MB 0x400000
#define ONETHIRTYTWO_MB 0x08400000
#define PROGIMG_OFF 0x48000
#define VIDEO       0xB8000


extern int32_t systemcall_handler(int32_t syscall, int32_t arg1, int32_t arg2, int32_t arg_3);
extern void init_syscall_idt();
extern int cur_pid; 
extern int parent_pid; 

int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t getargs(uint8_t* buf, int32_t nbytes);
int32_t vidmap(uint8_t** screen_start);
//int32_t set_handler(int32_t signum, void* handler_address);
//int32_t sigreturn(void);

#endif
