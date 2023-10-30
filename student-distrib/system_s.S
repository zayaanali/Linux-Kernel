#  systemcall_link
#    Inputs: none
#    Return Value: none
#    Function: assembly wrapper for systemcall_handler. sets up the stack, calls systemcall_handler, then does iret

.GLOBL systemcall_link
systemcall_link:
            pushal 
            pushfl 
            pushl %EDX
            pushl %ECX
            pushl %EBX
            pushl %EAX
            call systemcall_handler 
            ADDL $16, %ESP
            movl %eax, return
            popfl       
            popal  
            movl return, %eax
            # rn, EAX being overwritten by popal. figure out how to save it
            iret  

.data
return:
    .long 0


  
            