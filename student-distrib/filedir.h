#ifndef _FILEDIR_H
#define _FILEDIR_H

#include "types.h"

extern int dir_open(const uint8_t* fname);
extern int dir_close(int32_t fd);
extern int dir_write(int32_t fd, const void* buf, int32_t nbytes);
extern int dir_read(const uint8_t* fname, uint8_t* buf, uint32_t nbytes);

#endif
