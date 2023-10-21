#  systemcall_link
#    Inputs: none
#    Return Value: none
#    Function: assembly wrapper for systemcall_handler. sets up the stack, calls systemcall_handler, then does iret

.GLOBL systemcall_link
systemcall_link:
            pushal 
            pushfl 
            call systemcall_handler 
            popfl       
            popal  

            iret  




  
            