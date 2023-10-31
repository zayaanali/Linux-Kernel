#ifndef _FILE_H
#define _FILE_H

#include "types.h"

extern int32_t file_open(const uint8_t* fname);

extern int32_t file_close(int32_t fd);

extern int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

extern int32_t file_read(int32_t fd, void* buf, int32_t nbytes);

#endif
