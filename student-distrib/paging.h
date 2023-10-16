/* paging.h - Defines used to enable paging
 */

#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

#define PAGE_ENTRIES 1024
#define KERNEL_START 0x400000

/* Page table struct */
typedef union page_table_entry_t {
    
    uint32_t val;
    struct {
        unsigned int present:1;             // Bit 0
        unsigned int read_write:1;          // Bit 1
        unsigned int user_supervisor:1;     // Bit 2
        unsigned int page_write_through:1;  // Bit 3
        unsigned int page_cache_disable:1;  // Bit 4
        unsigned int accessed:1;            // Bit 5
        unsigned int dirty:1;               // Bit 6
        unsigned int PAT:1;                 // Bit 7
        unsigned int global:1;              // Bit 8
        unsigned int avail:3;               // Bit 9-11
        unsigned int page_base_address:20;  // Bit 12-31
    } __attribute__ ((packed));
} page_table_entry_t;

/* Page Directory Struct */
typedef union page_dir_entry_t {
    
    uint32_t val;
    struct {
        unsigned int present:1;             // Bit 0
        unsigned int read_write:1;          // Bit 1
        unsigned int user_supervisor:1;     // Bit 2
        unsigned int page_write_through:1;  // Bit 3
        unsigned int page_cache_disable:1;  // Bit 4
        unsigned int accessed:1;            // Bit 5
        unsigned int ignore:1;              // Bit 6 (always 0)
        unsigned int page_size:1;           // Bit 7
        unsigned int global:1;              // Bit 8
        unsigned int avail:3;              // Bit 9-11
        unsigned int page_table_base_address:20; // Bit 12-31
    } __attribute__ ((packed))page_dir_entry_4kb_t;


    struct {
        unsigned int present:1;             // Bit 0
        unsigned int read_write:1;          // Bit 1
        unsigned int user_supervisor:1;     // Bit 2
        unsigned int page_write_through:1;  // Bit 3
        unsigned int page_cache_disable:1;  // Bit 4
        unsigned int accessed:1;            // Bit 5
        unsigned int dirty:1;              // Bit 6 (always 0)
        unsigned int page_size:1;           // Bit 7
        unsigned int global:1;              // Bit 8
        unsigned int avail:3;              // Bit 9-11
        unsigned int PAT:1;                 // Bit 12
        unsigned int reserved:9;              // Bit 13-21
        unsigned int page_base_address:10; // Bit 22-31
    } __attribute__ ((packed))page_dir_entry_4mb_t;
} page_dir_entry_t;




extern page_dir_entry_t page_dir[PAGE_ENTRIES] __attribute__((aligned(4096)));
extern page_table_entry_t page_table[PAGE_ENTRIES] __attribute__((aligned(4096))); // new page table


extern void page_init();

#endif






