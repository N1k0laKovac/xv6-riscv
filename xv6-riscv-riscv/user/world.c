#include "user.h"

int main() {
    while(1) {
        printf("world\n");
        sleep(10); // 主动让出CPU（可选）
    }
    return 0;
}