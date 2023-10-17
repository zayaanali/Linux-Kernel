#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"

#define BLOCK_SIZE 4096     

extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/* directory entry struct */
typedef union dentry_t {
    
    struct {
        struct name file_name; 
        uint32_t file_type; 
        uint32_t inode_id;
        struct reserved_24 reserved; 
    };
} dentry_t;

/* boot block struct */
typedef union boot_block_t{

    struct {
        uint32_t num_dir_entries; 
        uint32_t num_inodes; 
        uint32_t num_dbs;               // number of data blocks
        struct reserved_52 reserved; 
        dentry_t dir_entries[63];
    };
} boot_block_t; 

/* inode struct */
typedef union inode_t {

    struct {
        uint32_t length; 
        uint32_t dbi [1095];        // dbi = data block indices. 0th data block is at data block index of dbi[0]
    };
} inode_t; 

/* struct for 32B name */
struct name{
    char chars[32];
};

/* struct for 24B reserved */
struct reserved_24{
    char data[32];
};


/* struct for 52B reserved */
struct reserved_52{
    char data[52];
};

#endif