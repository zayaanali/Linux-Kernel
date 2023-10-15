#include "paging.h"
#include "page.h"

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
        page_dir[i].reserved = 0;
        page_dir[i].page_size = 0;
        page_dir[i].ignored = 0;
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
        page_table[i].reserved = 0;
        page_table[i].page_size = 0;
        page_table[i].ignored = 0;
        page_table[i].page_table_base_address = 0;
    }

    /* Set first page dir entry as the page table we created */
    page_dir[0].present = 1;
    page_dir[0].read_write = 1;
    page_dir[0].user_supervisor = 1;
    page_dir[0].page_table_base_address = ((unsigned int) page_table) >> 12;


    loadPageDirectory(page_dir);
    enablePaging();
}
