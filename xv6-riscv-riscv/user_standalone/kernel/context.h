// kernel/context.h
#ifndef CONTEXT_H
#define CONTEXT_H

// Ҫ����ļĴ�����x1~x31��x0 ��ԶΪ 0�����ټ� epc���쳣���� PC��
typedef struct {
  unsigned long regs[32];
  unsigned long epc;
} context_t;

#endif
