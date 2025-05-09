#include "user.h"

static const char msg[] = "WORLD\n";

void user_world() {
    while (1) {
        write(1, msg, 6);
        for (volatile int i = 0; i < 10000000; i++);
    }
}