#ifndef _PAGE_H
#define _PAGE_H

#define NUM_PAGES 1024

#include "types.h"
#include "paging.h"

extern void loadPageDirectory(page_dir_entry_t* p_d);
extern void enablePaging();

#endif

