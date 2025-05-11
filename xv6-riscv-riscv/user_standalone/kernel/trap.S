.section .text
.globl timervec
timervec:
    # 读取 scause
    csrr    t0, scause

    # 检查是否为中断（最高位 scause[63]）
    srli    t1, t0, 63          # t1 = scause >> 63
    beqz    t1, NotInterrupt    # 如果最高位为0，说明不是中断

    # 是中断，再看低 7 位：判断是否为 supervisor timer interrupt (code = 5)
    andi    t2, t0, 0x7f        # t2 = scause[6:0]
    li      t1, 5
    beq     t2, t1, handle_timer

NotInterrupt:
    # 判断是否为 ecall from user-mode（scause = 8）
    li      t1, 8
    beq     t0, t1, handle_ecall

    # 其他异常类型，进入死循环
    j       die

##################################################
# 中断处理：定时器中断
##################################################
handle_timer:
    # 预留空间：保存 x1 ~ x31 以及 mepc
    addi sp, sp, -((32+1)*8)

    # 保存寄存器（注意寄存器名和编号的匹配）
    sd ra,   0*8(sp)
    sd sp,   2*8(sp)
    sd gp,   3*8(sp)
    sd tp,   4*8(sp)
    sd t0,   5*8(sp)
    sd t1,   6*8(sp)
    sd t2,   7*8(sp)
    sd s0,   8*8(sp)
    sd s1,   9*8(sp)
    sd a0,  10*8(sp)
    sd a1,  11*8(sp)
    sd a2,  12*8(sp)
    sd a3,  13*8(sp)
    sd a4,  14*8(sp)
    sd a5,  15*8(sp)
    sd a6,  16*8(sp)
    sd a7,  17*8(sp)
    sd s2,  18*8(sp)
    sd s3,  19*8(sp)
    sd s4,  20*8(sp)
    sd s5,  21*8(sp)
    sd s6,  22*8(sp)
    sd s7,  23*8(sp)
    sd s8,  24*8(sp)
    sd s9,  25*8(sp)
    sd s10, 26*8(sp)
    sd s11, 27*8(sp)
    sd t3,  28*8(sp)
    sd t4,  29*8(sp)
    sd t5,  30*8(sp)
    sd t6,  31*8(sp)

    # 保存 mepc（保存中断发生时 PC）
    csrr t2, mepc
    sd t2, 32*8(sp)

    # 调用调度器
    mv a0, sp
    call schedule

    # 获取调度返回的新上下文地址（返回值 a0）
    mv t3, a0

    # 恢复寄存器
    ld ra,   0*8(t3)
    ld sp,   2*8(t3)
    ld gp,   3*8(t3)
    ld tp,   4*8(t3)
    ld t0,   5*8(t3)
    ld t1,   6*8(t3)
    ld t2,   7*8(t3)
    ld s0,   8*8(t3)
    ld s1,   9*8(t3)
    ld a0,  10*8(t3)
    ld a1,  11*8(t3)
    ld a2,  12*8(t3)
    ld a3,  13*8(t3)
    ld a4,  14*8(t3)
    ld a5,  15*8(t3)
    ld a6,  16*8(t3)
    ld a7,  17*8(t3)
    ld s2,  18*8(t3)
    ld s3,  19*8(t3)
    ld s4,  20*8(t3)
    ld s5,  21*8(t3)
    ld s6,  22*8(t3)
    ld s7,  23*8(t3)
    ld s8,  24*8(t3)
    ld s9,  25*8(t3)
    ld s10, 26*8(t3)
    ld s11, 27*8(t3)
    ld t3,  28*8(t3)
    ld t4,  29*8(t3)
    ld t5,  30*8(t3)
    ld t6,  31*8(t3)

    # 恢复 mepc
    ld t2, 32*8(t3)
    csrw mepc, t2

    # 回收栈空间
    addi sp, sp, ((32+1)*8)

    # 返回用户态
    sret

##################################################
# 同步异常：ecall 处理
##################################################
handle_ecall:
    addi sp, sp, -((32+1)*8)

    # 保存寄存器
    sd ra,   0*8(sp)
    sd sp,   2*8(sp)
    sd gp,   3*8(sp)
    sd tp,   4*8(sp)
    sd t0,   5*8(sp)
    sd t1,   6*8(sp)
    sd t2,   7*8(sp)
    sd s0,   8*8(sp)
    sd s1,   9*8(sp)
    sd a0,  10*8(sp)
    sd a1,  11*8(sp)
    sd a2,  12*8(sp)
    sd a3,  13*8(sp)
    sd a4,  14*8(sp)
    sd a5,  15*8(sp)
    sd a6,  16*8(sp)
    sd a7,  17*8(sp)
    sd s2,  18*8(sp)
    sd s3,  19*8(sp)
    sd s4,  20*8(sp)
    sd s5,  21*8(sp)
    sd s6,  22*8(sp)
    sd s7,  23*8(sp)
    sd s8,  24*8(sp)
    sd s9,  25*8(sp)
    sd s10, 26*8(sp)
    sd s11, 27*8(sp)
    sd t3,  28*8(sp)
    sd t4,  29*8(sp)
    sd t5,  30*8(sp)
    sd t6,  31*8(sp)

    # 保存 mepc
    csrr t2, mepc
    sd t2, 32*8(sp)

    # 调用 syscall_handler(sp)
    mv a0, sp
    call syscall_handler

    # 恢复寄存器
    mv t3, a0
    ld ra,   0*8(t3)
    ld sp,   2*8(t3)
    ld gp,   3*8(t3)
    ld tp,   4*8(t3)
    ld t0,   5*8(t3)
    ld t1,   6*8(t3)
    ld t2,   7*8(t3)
    ld s0,   8*8(t3)
    ld s1,   9*8(t3)
    ld a0,  10*8(t3)
    ld a1,  11*8(t3)
    ld a2,  12*8(t3)
    ld a3,  13*8(t3)
    ld a4,  14*8(t3)
    ld a5,  15*8(t3)
    ld a6,  16*8(t3)
    ld a7,  17*8(t3)
    ld s2,  18*8(t3)
    ld s3,  19*8(t3)
    ld s4,  20*8(t3)
    ld s5,  21*8(t3)
    ld s6,  22*8(t3)
    ld s7,  23*8(t3)
    ld s8,  24*8(t3)
    ld s9,  25*8(t3)
    ld s10, 26*8(t3)
    ld s11, 27*8(t3)
    ld t3,  28*8(t3)
    ld t4,  29*8(t3)
    ld t5,  30*8(t3)
    ld t6,  31*8(t3)

    # 恢复 mepc
    ld t2, 32*8(t3)
    csrw mepc, t2

    addi sp, sp, ((32+1)*8)
    sret

##################################################
# 死循环
##################################################
die:
1:  j 1b
