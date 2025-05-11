#include <stdint.h>
static inline void putc(char c) {
    register uint64_t a0 asm("a0") = c;
    register uint64_t a7 asm("a7") = 1; // SBI_CONSOLE_PUTCHAR
    asm volatile("ecall" : : "r"(a0), "r"(a7));
}
static void print(const char *s) {
    for (; *s; s++) putc(*s);
}

void user_world(void) {
  while (1) {
    print("WORLD\n");
    for (volatile int i = 0; i < 10000000; i++);
  }
}
