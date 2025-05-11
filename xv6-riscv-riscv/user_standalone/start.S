    .section .text
    .align 2
    .globl _start
_start:
    # —— 1) 打印一次 'D' 用来验证我们到这里了 —— 
    li   a0, 'D'
    li   a7, 1       # SBI_CONSOLE_PUTCHAR
    ecall

    # —— 2) 设置好 Supervisor 模式下的栈 —— 
    la   sp, __stack_end

    # —— 3) 安装 S-mode 异常向量（在 M-mode 写也是可以的） —— 
    la   t0, timervec
    csrw stvec, t0

    # —— 4) 启用 S-mode 定时器中断 —— 
    csrr t0, sie
    li   t1, (1 << 5)    # SIE_STIE
    or   t0, t0, t1
    csrw sie, t0

    # —— 5) 启用全局 S-mode 中断 —— 
    csrr t0, sstatus
    li   t1, (1 << 1)    # SSTATUS_SIE
    or   t0, t0, t1
    csrw sstatus, t0

     # —— 从 M-mode 切到 S-mode 执行 kmain —— 

    # 1) 清除 mstatus.MPP[12:11] —— 掩码 = 3<<11 = 0x1800
    li    t1, 0x1800
    csrrc zero, mstatus, t1

    # 2) 置 mstatus.MPP = 01b —— 只要置 bit11 即可
    li    t1, 0x0800
    csrrs zero, mstatus, t1

    # 3) 把 kmain 的地址写入 mepc
    la    t0, kmain
    csrw  mepc, t0

    # 4) mret → 真正进入 S-mode 的 kmain
    mret

    # —— 下面定义栈边界，由链接脚本算好 —— 
    .section .bss
    .align 12           # 4KB 对齐
    .globl __stack_start
__stack_start:
    . = .
    .globl __stack_end
__stack_end:
    . = .
