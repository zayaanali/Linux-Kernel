#include "file.h"
#include "filesystem.h"
#include "pcb.h"
#include "systemcall.h"
#include "scheduler.h"


/* file_open
 *   Inputs: fname  : name of file to open
 *   Return Value: 0 upon success
 *   Function: Initialize any structs/variables needed for file handling 
 */
int32_t file_open(const uint8_t* fname){
    // initialize file position to start reading from to beginning of file
    int32_t i;      // return value
    dentry_t dentry[1];

    i=read_dentry_by_name(fname, dentry);

    if(i==-1){
        return i;       // dentry not found
    }
    

    uint32_t inode_id = dentry->inode_id;
    //file_pos[inode_id] = 0; 
    
    return insert_into_file_array(&file_funcs, inode_id);
}


/* file_close
 *   Inputs: fname  : name of file to close
 *   Return Value: 0 upon success
 *   Function: Undo anything done with file_open if necessary
 */
int32_t file_close(int32_t fd){

    return remove_from_file_array(fd); 
}

/* file_write
 *   Inputs: none
 *   Return Value: -1 for failure, this always fails
 *   Function: signals failure to write to read-only files
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes){

    //cannot write to read-only files
    return -1; 
}


/* file_read
 *   Inputs: fname    : name of file to read data from
 *           buf      : buffer to store data read
 *           nbytes   : number of bytes to read from the file
 *   Return Value: number of bytes read
 *   Function: reads nbytes bytes from file fname into buffer buf
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes){

    int32_t bytes_read;   

    cli();
    bytes_read = read_data(pcb_ptr[active_pid]->fd_array[fd].inode, pcb_ptr[active_pid]->fd_array[fd].file_pos, buf, nbytes);       // read data into buf, return number of bytes read

    // update file position
    pcb_ptr[active_pid]->fd_array[fd].file_pos +=bytes_read; 
    sti(); 
    
    return bytes_read; 
}
