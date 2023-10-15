#include "paging.h"

void page_init() {
    unsigned int i;
    for (i=0; i< PAGE_ENTRIES; i++) {
        page_table[i] = (i * 0x1000) | 3;
    }
}
