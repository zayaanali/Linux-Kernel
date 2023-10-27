#define ASM 1

save_parent_regs_to_pcb:
    pushl %ebp
    movl %esp, %ebp

    movl cur_pid, %edx
    addl $1, %edx
    
    movl scalar, %ecx
    imull %ecx, %edx            # edx <- (cur_pid+1)*(-8kb)
    addl base_pcb_addr, %edx    # edx <- (cur_pid+1)*(-8kb) + 8MB

    call get_eip
raddr:
    subl raddr, %eax
    movl %eax, (%edx)       # put eip
    movl %esp, 4(%edx)
    movl %ebp, 8(%edx)


    leave
    ret

base_pcb_addr:
    .long 0x800000

scalar:
    .long -0x2000      # 8kb


get_eip:
    movl (%esp), %eax
    ret
