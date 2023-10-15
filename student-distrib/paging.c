#include "paging.h"

void page_init() {
    int i; 

    for (i = 0; i < 1024; i++) {
        page_table[i].present = 1;
        page_table[i].read_write = 1;
        page_table[i].user_supervisor = 0;
        page_table[i].page_base_address = i * 0x1000; // 4 KiB per page
    }
    for (i = 0; i < 1024; i++) {
        page_dir[i].present = 1;
        page_dir[i].read_write = 1;
        page_dir[i].user_supervisor = 0;
        page_dir[i].page_table_base_address = ((uint32_t)page_table) >> 12;
    }

    asm volatile ("mov %0, %%cr3":: "r"(page_dir));

    uint32_t cr0;
    asm volatile ("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging
    asm volatile ("mov %0, %%cr0":: "r"(cr0));
    
}
