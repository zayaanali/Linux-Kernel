#define ASM     1
#include "page.h"

.text

.globl page_dir, page_table
   
   
   .align  4096
page_dir:
_page_dir:
    .rept NUM_PAGES
    .word 0
    .endr
page_dir_bottom:

   .align  4096
page_table:
_page_table:
    .rept NUM_PAGES
    .word 0
    .endr
page_table_bottom:


.text
.globl loadPageDirectory
loadPageDirectory:
push %ebp
mov %esp, %ebp
mov 8(%esp), %eax
mov %eax, %cr3
mov %ebp, %esp
pop %ebp
ret

.text
.globl enablePaging
enablePaging:
push %ebp
mov %esp, %ebp
mov %cr0, %eax
or $0x80000000, %eax
mov %eax, %cr0
mov %ebp, %esp
pop %ebp
ret