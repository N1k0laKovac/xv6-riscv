    .section .text
    .globl timervec
timervec:
    csrr t0, scause         // 读 cause
    // 先处理 timer 中断：机器层，最高位=1，低位=7
    li   t1, 0x80000007
    beq  t0, t1, handle_timer

    // 再处理用户 ecall：同步异常，scause=8
    li   t1, 8
    beq  t0, t1, handle_ecall

    // 其他异常，直接死机
    j    die

handle_timer:
    //  —— 保持之前的 timer 保存/调度逻辑 ——
    // 保存寄存器到栈
    addi sp, sp, -((32+1)*8)
    // 保存 x1..x31
    sd ra,   0*8(sp)
    sd t0,   1*8(sp)
    // …其余寄存器…
    // 保存 mepc（回到用户态的 PC）
    csrr t2, mepc
    sd t2, 32*8(sp)

    // 调度
    mv  a0, sp
    call schedule
    // schedule 返回新上下文指针 in a0
    mv  t3, a0

    // 恢复寄存器
    ld ra,   0*8(t3)
    ld t0,   1*8(t3)
    // …其余寄存器…
    ld t2,   32*8(t3)
    csrw mepc, t2
    addi sp, sp, ((32+1)*8)
    mret

handle_ecall:
    // 保存寄存器到栈（同 timer，但不用保存中断状态）
    addi sp, sp, -((32+1)*8)
    sd ra,   0*8(sp)
    sd t0,   1*8(sp)
    // …其余寄存器…
    csrr t2, mepc
    sd t2, 32*8(sp)

    // 调到 C 的 syscall handler
    mv a0, sp
    call syscall_handler

    // syscall_handler 返回，a0 为上下文指针
    mv t3, a0
    // 恢复寄存器
    ld ra,   0*8(t3)
    ld t0,   1*8(t3)
    // …其余寄存器…
    ld t2,   32*8(t3)
    csrw mepc, t2
    addi sp, sp, ((32+1)*8)
    mret

die:
    // 非法异常，进入死循环
1:  j 1b
