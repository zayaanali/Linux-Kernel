#include "filesystem.h"
#include "types.h"
#include "lib.h"


/* read_dentry_by_name
 *   Inputs: fname  : name of file corresponding to directory entry to read from
 *           dentry : pointer to dentry struct to store read dentry data
 *   Return Value: 0 for success, -1 for failure
 *   Function: read dentry elements from dentry corresponding to file indicated by fname into dentry arg 
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){

    int i;     // loop variable
    uint32_t fname_length = strlen((int8_t*)fname); 
    int8_t fname_ext[33];           // extend fname and fname of current dentry to 33 bytes so that even file names of max 32 bytes can be null-terminated
    int8_t o_fname_ext[33];
    
    for(i=0; i<fname_length; i++){
        fname_ext[i]=fname[i];
    }
    // extend argument fname to always end in at least one null char and be 33 bytes total
    for(i=fname_length; i<33; i++){
        fname_ext[i]='\0'; 
    }

    fname_length = strlen(fname_ext);

    // parameter check. function only works if fname is <=32 bytes and zero-padded when <32
    if(fname_length>32){
        return -1; 
    }

    // loop through directory entries until match is found
    for(i=0; i<boot_block->num_dir_entries; i++){
        strcpy(o_fname_ext, (int8_t*)boot_block->dir_entries[i].file_name);
        o_fname_ext[32]='\0';                                               // null-terminate so strncmp works as expected
        
        if(strncmp(o_fname_ext,(int8_t*)fname,33)==0){
            // found corresponding dentry. copy fields into dentry arg
            strcpy((int8_t*)dentry->file_name, (int8_t*)boot_block->dir_entries[i].file_name);
            dentry->file_type = boot_block->dir_entries[i].file_type;
            dentry->inode_id = boot_block->dir_entries[i].inode_id;
            return 0; 
        }
    }

    // directory entry not found
    return -1; 
}


/* read_dentry_by_index
 *   Inputs: index  : index of directory entry to read from
 *           dentry : pointer to dentry struct to store read dentry data
 *   Return Value: 0 for success, -1 for failure
 *   Function: read dentry elements for dentry at index "index" into dentry arg 
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){

    // check validity of index
    if(index > (boot_block->num_dir_entries-1)){
        return -1; 
    }

    // copy from corresponding dentry into dentry arg
    strcpy((int8_t*)dentry->file_name, (int8_t*)boot_block->dir_entries[index].file_name);
    dentry->file_type = boot_block->dir_entries[index].file_type;
    dentry->inode_id = boot_block->dir_entries[index].inode_id;

    return 0; 
}


/* read_data
 *   Inputs: inode  : inode corresponding to file to read data from
 *           offset : byte-offset to start reading from
 *           buf    : location to store data read
 *           length : length in bytes to read
 *   Return Value: number of bytes successfully read
 *   Function: read "length"  bytes of data from file indicated by inode into buf starting at byte "offset"
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    // Check if inode number is valid
    if (inode >= MAX_NUM_FILES) {
        return -1;
    }

    // Locate the inode structure
    inode_t* inode_ptr = &inodes[inode];

    // Calculate number of data blocks and block size
    uint32_t num_blocks = (inode_ptr->length + BLOCK_SIZE - 1) / BLOCK_SIZE;     //ceiling on number of blocks to read

    // Initialize variables for reading data
    uint32_t read_length = 0;
    uint32_t block_offset = offset % BLOCK_SIZE;
    uint32_t block_index = offset / BLOCK_SIZE;

    if (length > inode_ptr->length)
        length = inode_ptr->length;

    // Read data from data blocks
    while (read_length < length && block_index < num_blocks) {
        // Get block number
        uint32_t block_no = inode_ptr->dbi[block_index];

        // calculate number of bytes to copy into buffer
        uint32_t to_read = BLOCK_SIZE - block_offset;

        // ensure only reading max of "length" bytes
        if (to_read > length - read_length) {
            to_read = length - read_length;
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

/* filesys_init
 *   Inputs: mod  : pointer to module that holds starting address of filesys_img
 *   Return Value: nothing
 *   Function: create mapping between file system struct and filesys_img 
 */
void filesys_init(module_t* mod){
    boot_block = (boot_block_t*)mod->mod_start;
    inodes = (inode_t*)(boot_block + 1);                        // +1 so inodes struct begins after first 4kb boot block
    data_blocks = (data_block_t*)(inodes + MAX_NUM_FILES);      // +MAX_NUM_FILES so data_blocks struct begins after all inodes
}
