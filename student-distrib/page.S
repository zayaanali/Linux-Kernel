#define ASM 1

.globl loadPageDirectory, enablePaging, flush_tlb
.text

/*  Load page directory to cr3 */
loadPageDirectory:
pushl %ebp
movl %esp, %ebp
movl %cr3, %eax
movl %eax, %cr3
movl 8(%esp), %eax
movl %eax, %cr3
leave
ret


/* Turn on paging */
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



flush_tlb:
movl %cr3, %eax
movl %eax, %cr3
ret


