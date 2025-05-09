// user_standalone/kernel/kernel.c

#include <stdint.h>
#include "context.h"
#include "syscall.h"

#define CLINT_MTIME       0x200BFF8UL
#define CLINT_MTIMECMP(h) (0x2004000UL + 8*(h))  // h=0 ��ʾ Hart 0

extern void user_hello(void);
extern void user_world(void);
extern void user_mcat(void);

extern char __stack_end;   // ���ӽű��ж���
#define STACK_PER_PROC 8192
#define NPROC 3

static context_t ctx[NPROC];
static int cur = 0;

// Round-robin �����������ձ����ϵ� old �����ģ����� ctx[cur]��Ȼ�󷵻���һ�� ctx[cur]
void schedule(context_t *old) {
  // ����ձ����ϵ�������
  ctx[cur] = *old;
  // �ֵ���һ��
  cur = (cur + 1) % NPROC;
  // ������һ�λ�����ʱ���ж�
  volatile uint64_t *mtime    = (uint64_t*)CLINT_MTIME;
  volatile uint64_t *mtimecmp = (uint64_t*)CLINT_MTIMECMP(0);
  *mtimecmp = *mtime + 1000000;  // ����ʱ��Ƭ����
  // ������һ�������ĵ�ַ�� a0
  __asm__ volatile("mv a0, %0" :: "r"(&ctx[cur]));
}

// // SBI ����̨���һ���ַ�
// static inline void sbi_putchar(int ch) {
//   register uint64_t a0 asm("a0") = ch;
//   register uint64_t a7 asm("a7") = 1; // SBI_CONSOLE_PUTCHAR
//   asm volatile("ecall" : : "r"(a0), "r"(a7));
// }

// // ϵͳ���ô����� old_ctx ȡ���Ĵ���ֵ�����ݺŷַ���Ȼ����������ֵ�� mepc
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
//       // �򻯣�stdin ��֧�֣����� 0 ��ʾ EOF
//       ret = 0;
//       break;
//     case SYS_exit:
//       // �����˳���ͣ����ѭ�������ٵ���
//       while (1) { asm volatile("wfi"); }
//       break;
//     default:
//       break;
//   }

//   // ����������д�ط���ֵ
//   old_ctx->regs[10] = ret;
//   // ���� ecall ָ�� (4 �ֽ�)
//   old_ctx->epc += 4;
//   return old_ctx;
// }

// �ں��������
void kmain() {
  // 1. ��װ�ж������� timervec
  extern void timervec();
  __asm__ volatile("la t0, timervec; csrw mtvec, t0");

  // 2. ��ʼ��ÿ�����̵������ģ�regs ���ջָ�� sp��x2�����Լ���� epc
  for (int i = 0; i < NPROC; i++) {
    for (int j = 0; j < 32; j++)
      ctx[i].regs[j] = 0;
    // ջ���� __stack_end ���·���
    ctx[i].regs[2] = (uint64_t)&__stack_end - (uint64_t)(i * STACK_PER_PROC);
  }
  ctx[0].epc = (uint64_t)user_hello;
  ctx[1].epc = (uint64_t)user_world;
  ctx[2].epc = (uint64_t)user_mcat;

  // 3. ʹ�ܻ�����ʱ���жϣ����ó��� mtimecmp���� MIE.MTIE �� MSTATUS.MIE
  volatile uint64_t *mtime    = (uint64_t*)CLINT_MTIME;
  volatile uint64_t *mtimecmp = (uint64_t*)CLINT_MTIMECMP(0);
  *mtimecmp = *mtime + 1000000;

  // �� MIE.MTIE
  __asm__ volatile("csrr t0, mie; li t1, 0x80; or t0, t0, t1; csrw mie, t0");
  // ��ȫ���ж� MSTATUS.MIE
  __asm__ volatile("csrr t0, mstatus; li t1, 0x8; or t0, t0, t1; csrw mstatus, t0");

  // 4. �״��л����� 0 �����񣺻ָ��Ĵ��� && mret
  context_t *first = &ctx[0];
  __asm__ volatile(
    "mv sp, %0\n\t"           // SP ָ�򱣴���
    "ld ra,   0*8(sp)\n\t"    // �ָ� ra
    "ld t0,   1*8(sp)\n\t"    // �ָ� t0
    // �� ���λָ� s0�Cs11 �Ĵ��� ��
    "ld t1, 32*8(sp)\n\t"     // �ָ� mepc
    "csrw mepc, t1\n\t"
    "addi sp, sp, %1\n\t"     // �ָ� SP ������ջ��
    "mret\n\t"
    :: "r"(first), "i"((32+1)*8)
  );

  // ��Զ�������
  while (1) { asm volatile("wfi"); }
}
