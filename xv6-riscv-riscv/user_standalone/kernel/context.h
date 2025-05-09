// kernel/context.h
#ifndef CONTEXT_H
#define CONTEXT_H

// 要保存的寄存器：x1~x31（x0 永远为 0），再加 epc（异常返回 PC）
typedef struct {
  unsigned long regs[32];
  unsigned long epc;
} context_t;

#endif
