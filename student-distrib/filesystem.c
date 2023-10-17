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
    for(i=0; i<boot_block.num_dir_entries; i++){
        if(strncmp(boot_block.dir_entries[i].file_name,fname,32)==0){
            strcpy(dentry->file_name, boot_block.dir_entries[i].file_name);
            dentry->file_type = boot_block.dir_entries[i].file_type;
            dentry->inode_id = boot_block.dir_entries[i].inode_id;
            return 0; 
        }
    }

    // directory entry not found
    return -1; 
}


int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){

    // check validity of index
    if(index > (boot_block.num_dir_entries-1)){
        return -1; 
    }

    strcpy(dentry->file_name, boot_block.dir_entries[index].file_name);
    dentry->file_type = boot_block.dir_entries[index].file_type;
    dentry->inode_id = boot_block.dir_entries[index].inode_id;
    return 0; 
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    return 0; 
}

