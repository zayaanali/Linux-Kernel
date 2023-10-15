#define ASM 1

.globl loadPageDirectory, enablePaging

/* NOTE: I think this code is redundant. TA made me make it but pretty sure the c code does the same thing
.text


.globl page_dir, page_table

   .align  4096
page_dir:
_page_dir: # set page directory entry ?
    .rept NUM_PAGES
    .word 0
    .endr
page_dir_bottom:

   .align  4096
page_table: # set each page table entry ?
_page_table:
    .rept NUM_PAGES
    .word 0
    .endr
page_table_bottom:

*/ 
.text

loadPageDirectory:
push %ebp
mov %esp, %ebp
mov 8(%esp), %eax
mov %eax, %cr3
mov %ebp, %esp
pop %ebp
ret

enablePaging:
push %ebp
mov %esp, %ebp
mov %cr0, %eax
or $0x80000000, %eax
mov %eax, %cr0
mov %ebp, %esp
pop %ebp
ret



