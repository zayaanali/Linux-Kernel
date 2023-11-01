#  systemcall_link
#    Inputs: none
#    Return Value: none
#    Function: assembly wrapper for systemcall_handler. sets up the stack, calls systemcall_handler, then does iret

.GLOBL systemcall_link
systemcall_link:
            pushfl
            pushl %EBX
            pushl %esi
            pushl %edi 
            pushl %EDX
            pushl %ECX
            pushl %EBX
            pushl %EAX
            call systemcall_handler 
            ADDL $16, %ESP
            popl %edi
            popl %esi
            popl %ebx
            popfl       
            
            iret  

