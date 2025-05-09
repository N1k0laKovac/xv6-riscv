// user/mcat.c
#include "types.h"
#include "fcntl.h"
#include "user.h"

void user_mcat(void) {
    char buf[512];
    int n;
    // ? stdin
    while ((n = read(0, buf, sizeof(buf))) > 0) {
        if (write(1, buf, n) != n)
            exit(1);
    }
    if (n < 0)
        exit(1);
    exit(0);
}
