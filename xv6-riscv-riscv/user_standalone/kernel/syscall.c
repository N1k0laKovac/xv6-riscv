#include "context.h"
#include "syscall.h"
#include <stdint.h>

static inline void sbi_putchar(int ch) {
    register uint64_t a0 asm("a0") = ch;
    register uint64_t a7 asm("a7") = 1; // SBI_CONSOLE_PUTCHAR
    asm volatile("ecall" : : "r"(a0), "r"(a7));
}

context_t* syscall_handler(context_t *old_ctx) {
    uint64_t num = old_ctx->regs[17];
    uint64_t a0 = old_ctx->regs[10];
    uint64_t a1 = old_ctx->regs[11];
    uint64_t a2 = old_ctx->regs[12];
    uint64_t ret = -1;

    switch (num) {
    case SYS_write:
        if ((int)a0 == 1) {
            char *buf = (char*)a1;
            for (int i = 0; i < (int)a2; i++)
                sbi_putchar(buf[i]);
            ret = a2;
        }
        break;
    case SYS_exit:
        while (1); // 死循环不再调度
    default:
        break;
    }

    old_ctx->regs[10] = ret;
    old_ctx->epc += 4;
    return old_ctx;
}
