
.GLOBL systemcall_link
systemcall_link:
            pushal 
            pushfl 
            call systemcall_handler 
            popfl       
            popal  

            ret  