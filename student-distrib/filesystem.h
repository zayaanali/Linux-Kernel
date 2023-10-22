#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"
#include "multiboot.h"

#define BLOCK_SIZE 4096     
#define MAX_NUM_FILES 64
#define MAX_NUM_DATA_BLOCKS_PER_FILE 1023


/* inode struct */
typedef union inode_t {

    struct {
        uint32_t length; 
        uint32_t dbi [MAX_NUM_DATA_BLOCKS_PER_FILE];        // dbi = data block indices. 0th data block is at data block index of dbi[0]
    };
} inode_t; 



/* directory entry struct */
typedef struct dentry_t {
    
    uint8_t file_name[32];      // file_name field is 32 bytes
    uint32_t file_type; 
    uint32_t inode_id;
    uint8_t reserved[24];       // reserved field is 24 bytes
   
} dentry_t;

/* boot block struct */
typedef struct boot_block_t{

    uint32_t num_dir_entries; 
    uint32_t num_inodes; 
    uint32_t num_dbs;               // number of data blocks
    uint8_t reserved[52];           // reserved field is 52 bytes
    dentry_t dir_entries[63];       // 63 directory entries in boot block
} boot_block_t; 

/* data block struct */
typedef struct data_block_t{

    uint8_t data[4096];

} data_block_t; 


/* functions to interface with filesystem. see descriptions in filesystem.c*/
extern int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
extern void filesys_init(module_t* mod);


/* boot_block, inodes, and data_blocks structures. To be initialized in filesys_init*/
boot_block_t* boot_block; 
inode_t* inodes;
data_block_t* data_blocks;

#endif
