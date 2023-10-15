#include "paging.h"

void paging_init(){
    PageTableEntry pageTable[1024] __attribute__((aligned(4096)));
    PageDirectoryEntry pageDirectory[1024] __attribute__((aligned(4096)));
    for (int i = 0; i < 1024; i++) {
        pageTable[i].present = 1;
        pageTable[i].read_write = 1;
        pageTable[i].user_supervisor = 0;
        pageTable[i].page_base_address = i * 0x1000; // 4 KiB per page
    }
    for (int i = 0; i < 1024; i++) {
        pageDirectory[i].present = 1;
        pageDirectory[i].read_write = 1;
        pageDirectory[i].user_supervisor = 0;
        pageDirectory[i].page_table_base_address = ((uint32_t)pageTable) >> 12;
    }

    asm volatile ("mov %0, %%cr3":: "r"(pageDirectory));

    uint32_t cr0;
    asm volatile ("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging
    asm volatile ("mov %0, %%cr0":: "r"(cr0));
    
}
