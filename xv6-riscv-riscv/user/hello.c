#include "user/user.h"  // 确保包含头文件

int main() {
    write(1,"Hello, world!\n",13);  // 通过ecall触发系统调用
    printff("helloworld!");
    MYwrite(1,"helloWorld",13);
    exit(0);
}