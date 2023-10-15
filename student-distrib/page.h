#ifndef _PAGE_H
#define _PAGE_H

#define NUM_PAGES 1024

#include "types.h"

extern void loadPageDirectory(unsigned int*);
extern void enablePaging();

#endif