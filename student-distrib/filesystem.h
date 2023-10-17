#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"

#define BLOCK_SIZE 4096     


/* directory entry struct */
typedef union directory_entry_t {
    
    struct {
        struct name file_name; 
        uint32_t file_type; 
        uint32_t inode_id;
    };
} directory_entry_t;


/* struct for 32B name */
struct name{
    char chars[32];
}

#endif