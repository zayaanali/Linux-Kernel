#include "paging.h"
#include "page.h"

page_dir_entry_t page_dir[PAGE_ENTRIES] __attribute__((aligned(4096)));
page_table_entry_t page_table[PAGE_ENTRIES] __attribute__((aligned(4096))); // new page table


void page_init() {
    int i; 

    /* Initalize page directory */
    for (i = 0; i < NUM_PAGES; i++) {
        page_dir[i].present = 0;
        page_dir[i].read_write = 1;
        page_dir[i].user_supervisor = 0;
        page_dir[i].page_write_through = 0;
        page_dir[i].page_cache_disable = 0;
        page_dir[i].accessed = 0;
        page_dir[i].ignore = 0;
        page_dir[i].page_size = 0;
        page_dir[i].global = 0;
        page_dir[i].avail = 0;
        page_dir[i].page_table_base_address = 0;
    }
    
    /* Initalize page directory */
    for (i = 0; i < NUM_PAGES; i++) {
        page_table[i].present = 1;
        page_table[i].read_write = 1;
        page_table[i].user_supervisor = 1;
        page_table[i].page_write_through = 0;
        page_table[i].page_cache_disable = 0;
        page_table[i].accessed = 0;
        page_table[i].dirty = 0;
        page_table[i].PAT = 0;
        page_table[i].global = 0;
        page_table[i].avail = 0;
        page_table[i].page_base_address = (i * 0x1000) ; // 4KB page size - 0x1000 = 4096

    }
    
    /* Set virtual memory 0-4MB (broken down into 4KB pages) */
    page_dir[0].present = 1;
    page_dir[0].read_write = 1;
    page_dir[0].user_supervisor = 1;
    page_dir[0].page_size = 0; // 4KB page size
    page_dir[0].page_table_base_address = ((unsigned int) page_table) >> 12; // align the page_table address to 4KB boundary

    /* Set Kernel Memory 4-8MB (single 4MB page) */
    page_dir[1].present = 1;
    page_dir[1].read_write = 1;
    page_dir[1].user_supervisor = 1;
    page_dir[1].page_size = 1; // 4MB page
    page_dir[1].page_table_base_address = (KERNEL_START >> 12); // align the page_table address to 4KB boundary


    /* load directory and enable */
    loadPageDirectory(page_dir);
    enablePaging();
}
