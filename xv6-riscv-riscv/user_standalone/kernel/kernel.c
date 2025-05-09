// user_standalone/kernel/kernel.c

#include <stdint.h>
#include "context.h"
#include "syscall.h"

#define CLINT_MTIME       0x200BFF8UL
#define CLINT_MTIMECMP(h) (0x2004000UL + 8*(h))  // h=0 表示 Hart 0

extern void user_hello(void);
extern void user_world(void);
extern void user_mcat(void);

extern char __stack_end;   // 链接脚本中定义
#define STACK_PER_PROC 8192
#define NPROC 3

static context_t ctx[NPROC];
static int cur = 0;

// Round-robin 调度器：接收被抢断的 old 上下文，更新 ctx[cur]，然后返回下一个 ctx[cur]
void schedule(context_t *old) {
  // 保存刚被抢断的上下文
  ctx[cur] = *old;
  // 轮到下一个
  cur = (cur + 1) % NPROC;
  // 安排下一次机器定时器中断
  volatile uint64_t *mtime    = (uint64_t*)CLINT_MTIME;
  volatile uint64_t *mtimecmp = (uint64_t*)CLINT_MTIMECMP(0);
  *mtimecmp = *mtime + 1000000;  // 调整时间片长度
  // 返回下一个上下文地址到 a0
  __asm__ volatile("mv a0, %0" :: "r"(&ctx[cur]));
}

// // SBI 控制台输出一个字符
// static inline void sbi_putchar(int ch) {
//   register uint64_t a0 asm("a0") = ch;
//   register uint64_t a7 asm("a7") = 1; // SBI_CONSOLE_PUTCHAR
//   asm volatile("ecall" : : "r"(a0), "r"(a7));
// }

// // 系统调用处理：从 old_ctx 取出寄存器值，根据号分发，然后修正返回值和 mepc
// context_t* syscall_handler(context_t *old_ctx) {
//   uint64_t num = old_ctx->regs[17];  // a7 = syscall number
//   uint64_t a0  = old_ctx->regs[10];  // fd or return value
//   uint64_t a1  = old_ctx->regs[11];  // buf
//   uint64_t a2  = old_ctx->regs[12];  // len
//   uint64_t ret = -1;

//   switch(num) {
//     case SYS_write:
//       if ((int)a0 == 1) {
//         char *buf = (char*)a1;
//         for(int i = 0; i < (int)a2; i++)
//           sbi_putchar(buf[i]);
//         ret = a2;
//       }
//       break;
//     case SYS_read:
//       // 简化：stdin 不支持，返回 0 表示 EOF
//       ret = 0;
//       break;
//     case SYS_exit:
//       // 任务退出：停在死循环，不再调度
//       while (1) { asm volatile("wfi"); }
//       break;
//     default:
//       break;
//   }

//   // 在上下文中写回返回值
//   old_ctx->regs[10] = ret;
//   // 跳过 ecall 指令 (4 字节)
//   old_ctx->epc += 4;
//   return old_ctx;
// }

// 内核启动入口
void kmain() {
  // 1. 安装中断向量到 timervec
  extern void timervec();
  __asm__ volatile("la t0, timervec; csrw mtvec, t0");

  // 2. 初始化每个进程的上下文：regs 与堆栈指针 sp（x2），以及入口 epc
  for (int i = 0; i < NPROC; i++) {
    for (int j = 0; j < 32; j++)
      ctx[i].regs[j] = 0;
    // 栈顶从 __stack_end 向下分配
    ctx[i].regs[2] = (uint64_t)&__stack_end - (uint64_t)(i * STACK_PER_PROC);
  }
  ctx[0].epc = (uint64_t)user_hello;
  ctx[1].epc = (uint64_t)user_world;
  ctx[2].epc = (uint64_t)user_mcat;

  // 3. 使能机器定时器中断：设置初次 mtimecmp，打开 MIE.MTIE 与 MSTATUS.MIE
  volatile uint64_t *mtime    = (uint64_t*)CLINT_MTIME;
  volatile uint64_t *mtimecmp = (uint64_t*)CLINT_MTIMECMP(0);
  *mtimecmp = *mtime + 1000000;

  // 打开 MIE.MTIE
  __asm__ volatile("csrr t0, mie; li t1, 0x80; or t0, t0, t1; csrw mie, t0");
  // 打开全局中断 MSTATUS.MIE
  __asm__ volatile("csrr t0, mstatus; li t1, 0x8; or t0, t0, t1; csrw mstatus, t0");

  // 4. 首次切换到第 0 号任务：恢复寄存器 && mret
  context_t *first = &ctx[0];
  __asm__ volatile(
    "mv sp, %0\n\t"           // SP 指向保存区
    "ld ra,   0*8(sp)\n\t"    // 恢复 ra
    "ld t0,   1*8(sp)\n\t"    // 恢复 t0
    // … 依次恢复 s0–s11 寄存器 …
    "ld t1, 32*8(sp)\n\t"     // 恢复 mepc
    "csrw mepc, t1\n\t"
    "addi sp, sp, %1\n\t"     // 恢复 SP 到进程栈顶
    "mret\n\t"
    :: "r"(first), "i"((32+1)*8)
  );

  // 永远不会回来
  while (1) { asm volatile("wfi"); }
}
