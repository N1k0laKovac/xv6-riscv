#include "user.h"

static const char msg[] = "HELLO\n";

void user_hello() {
    while (1) {
        write(1, msg, 6);
        for (volatile int i = 0; i < 10000000; i++);
    }
}