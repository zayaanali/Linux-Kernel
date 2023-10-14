#ifndef _PAGING_H
#define _PAGING_H

typedef struct PageDirectoryEntry {
    unsigned int present:1;           // Bit 0
    unsigned int read_write:1;        // Bit 1
    unsigned int user_supervisor:1;   // Bit 2
    unsigned int page_write_through:1;// Bit 3
    unsigned int page_cache_disable:1;// Bit 4
    unsigned int accessed:1;          // Bit 5
    unsigned int reserved:1;          // Bit 6
    unsigned int page_size:1;         // Bit 7
    unsigned int ignored1:4;          // Bit 8-11
    unsigned int page_table_base_address:20;// Bit 12-31
} __attribute__((packed)) PageDirectoryEntry;


typedef struct PageTableEntry {
    unsigned int present:1;           // Bit 0
    unsigned int read_write:1;        // Bit 1
    unsigned int user_supervisor:1;   // Bit 2
    unsigned int page_write_through:1;// Bit 3
    unsigned int page_cache_disable:1;// Bit 4
    unsigned int accessed:1;          // Bit 5
    unsigned int dirty:1;             // Bit 6
    unsigned int reserved:1;          // Bit 7
    unsigned int global:1;            // Bit 8
    unsigned int ignored1:3;          // Bit 9-11
    unsigned int page_base_address:20;// Bit 12-31
} __attribute__((packed)) PageTableEntry;

extern void paging_init();

#endif






