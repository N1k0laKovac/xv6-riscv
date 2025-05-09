/* user.ld */
ENTRY(_start)

MEMORY {
    RAM  (rwx) : ORIGIN = 0x80000000, LENGTH = 128K
    USER (rwx) : ORIGIN = 0x80010000, LENGTH = 64K
}

SECTIONS {
  /* --------- 内核 + 用户 代码 --------- */
  . = 0x80000000;
  .text : {
    *(.text)      /* start.o, trap.o, kernel.o, syscall.o, user/*.o 全部 .text */
    *(.text*)     /* 以及所有子节 */
  } > RAM

  /* --------- 只要把 .rodata/.data/.bss 放到 RAM 就好 --------- */
  .rodata : {
    *(.rodata*)
    *(.srodata*)
  } > RAM

  .data : {
    *(.data*)
  } > RAM

  .bss : {
    *(.bss*)
  } > RAM

  /* --------- 内核栈区（NOLOAD） --------- */
  .stack (NOLOAD) : {
    __stack_start = .;
    . = . + 16K;      /* 给内核模式下留 16KB 的栈 */
    __stack_end   = .;
  } > RAM

  /* --------- 如果要给用户模式留栈，可在 USER 区另起一段 --------- */

  /DISCARD/ : { *(.comment) }
}
