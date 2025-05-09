/* start.S */
    .section .bss
    .align 12               /* 对齐到 4K */
    .globl __stack_start
__stack_start:
    .space 0                /* 大小由链接脚本里的 .stack 决定 */
    .globl __stack_end
__stack_end:

    .section .text
    .globl _start
_start:
    /* 1. 切换到内核模式下的栈顶 */
    la   sp, __stack_end

    /* 2. 调用 C 入口 kmain */
    call kmain

    /* 3. 如果 kmain 返回，进入死循环 */
1:  wfi
    j   1b
