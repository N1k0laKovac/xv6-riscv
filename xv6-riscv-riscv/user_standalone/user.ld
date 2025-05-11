ENTRY(_start)

MEMORY {
    /* OpenSBI 固件通常驻扎在 0x80000000 ~ 0x8004xxxx，
       payload 会从 0x80000000 的偏移开始加载，覆盖在固件之后。 */
    RAM  (rwx) : ORIGIN = 0x80200000, LENGTH = 256K
    USER (rwx) : ORIGIN = 0x80210000, LENGTH =  64K
}

SECTIONS {
    /* 1) 把 start.o、trap.o、kernel.o、syscall.o 都放在 RAM 起点 */
    . = 0x80200000;
    .text : {
        *(.text)        /* 包含 start.S、trap.S、kernel/kernel.c 编译出的 .text */
        *(.text*)       /* 以及它们的子节 */
    } > RAM

    /* 2) 接着放只读数据 */
    .rodata : {
        *(.rodata*)
        *(.srodata*)
    } > RAM

    /* 3) 放 data 段 */
    .data : {
        *(.data*)
    } > RAM

    /* 4) 放 bss 段 */
    .bss : {
        *(.bss*)
    } > RAM

    /* 5) 专门给用户程序腾一个区域 */
    . = 0x80210000;
    .user.text : {
        user/hello.o(.text)
        user/world.o(.text)
        user/mcat.o(.text)
    } > USER

    /* 6) 栈空间（3 个进程 × 8KB） */
    .stack (NOLOAD) : {
        __stack_start = .;
        . += 3 * 8192;
        __stack_end   = .;
    } > RAM

    /* 丢弃注释节 */
    /DISCARD/ : { *(.comment) }
}
