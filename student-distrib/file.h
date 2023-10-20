#ifndef _FILE_H
#define _FILE_H

#include "types.h"

extern int file_open(const uint8_t* fname);

extern int file_close(int32_t fd);

extern int file_write();

extern int file_read(const uint8_t* fname, uint8_t* buf, uint32_t nbytes);

#endif