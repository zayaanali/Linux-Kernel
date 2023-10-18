#include "filesystem.h"
#include "types.h"
#include "lib.h"



int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){

    int i;     // loop variable
    int fname_length = strlen(fname); 

    // parameter check. function only works if fname is 32 bytes
    if(fname_length!=32){
        return -1; 
    }

    // loop through directory entries until match is found
    for(i=0; i<boot_block->num_dir_entries; i++){
        if(strncmp(boot_block->dir_entries[i].file_name,fname,32)==0){
            strcpy(dentry->file_name, boot_block->dir_entries[i].file_name);
            dentry->file_type = boot_block->dir_entries[i].file_type;
            dentry->inode_id = boot_block->dir_entries[i].inode_id;
            return 0; 
        }
    }

    // directory entry not found
    return -1; 
}


int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){

    // check validity of index
    if(index > (boot_block->num_dir_entries-1)){
        return -1; 
    }

    strcpy(dentry->file_name, boot_block->dir_entries[index].file_name);
    dentry->file_type = boot_block->dir_entries[index].file_type;
    dentry->inode_id = boot_block->dir_entries[index].inode_id;
    return 0; 
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    // Check if inode number is valid
    if (inode >= MAX_NUM_FILES) {
        return -1;
    }

    // Locate the inode structure
    inode_t* inode_ptr = &inodes[inode];

    // Calculate number of data blocks and block size
    uint32_t num_blocks = (inode_ptr->length + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // Initialize variables for reading data
    uint32_t read_length = 0;
    uint32_t block_offset = offset % BLOCK_SIZE;
    uint32_t block_index = offset / BLOCK_SIZE;

    // Read data from data blocks
    while (read_length < length && block_index < num_blocks) {
        // Get block number
        uint32_t block_no = inode_ptr->dbi[block_index];
        if (block_no == 0) {
            return -1; 
        }

        // Calculate how much data to read from this block
        uint32_t to_read = length - read_length;
        if (to_read > BLOCK_SIZE - block_offset) {
            to_read = BLOCK_SIZE - block_offset;
        }

        // Read data from block
        uint8_t* block_ptr = data_blocks[block_no].data;
        memcpy(buf + read_length, block_ptr + block_offset, to_read);

        // Update variables
        read_length += to_read;
        block_index++;
        block_offset = 0; // Reset offset for next blocks
    }

    // Return number of bytes read
    return read_length;
}


void filesys_init(module_t* mod){
    clear();
    int i;
    boot_block = (boot_block_t*)mod->mod_start;
    printf("Number of Dir Entries: %d \n", boot_block->num_dir_entries);
    printf("Number of Inodes: %d \n", boot_block->num_inodes);
    printf("Number of Data Blocks: %d \n", boot_block->num_dbs);

    inodes = (inode_t*)(boot_block + 1);
    for(i = 0; i < 20; i++){
        printf("Length of Inodes %d : %d \n", i, inodes[i].length);
    }

    data_blocks = (data_block_t*)(inodes + MAX_NUM_FILES);
    printf("Data at position 0: %d \n", data_blocks[7].data[7]);


}
