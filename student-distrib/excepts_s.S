
#include "excepts.h"
.text

.global divide_error_link


divide_error_link: 

    pushl %ebp
    movl %esp, %ebp

    pushl %eax
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi
    pushf
    
    call divide_error
    
    popf
    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %ebx
    popl %eax
    
    leave
    iret 



# MAKE_LINKAGE(divide_error_link, divide_error)