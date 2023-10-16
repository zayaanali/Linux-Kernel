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
pushl %ebp
movl %esp, %ebp
pushal
pushfl
movl 8(%esp), %eax
movl %eax, %cr3
popfl
popal
leave
ret

enablePaging:
pushl %ebp
movl %esp, %ebp
pushal
pushfl

# enable page size extensions to allow 4MB pages
movl %cr4, %eax
orl $0x00000010, %eax
movl %eax, %cr4

# set PG flag in cr0
movl %cr0, %eax
orl $0x80000000, %eax
movl %eax, %cr0

popfl
popal
leave
ret



