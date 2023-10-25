#ifndef _FILEDIR_H
#define _FILEDIR_H

#include "types.h"

extern int32_t dir_open(const uint8_t* fname);
extern int32_t dir_close(int32_t fd);
extern int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);

#endif
