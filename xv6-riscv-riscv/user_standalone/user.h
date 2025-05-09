// user_standalone/user.h
// 删除与标准库冲突的声明，使用xv6自定义实现
#pragma once

typedef unsigned int uint;

// 系统调用接口
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int exit(int) __attribute__((noreturn));
int fork(void);
int wait(int*);
int open(const char*, int);

// 用户态库函数（在ulib.c中实现）
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char);
uint strlen(const char*);
void *memset(void*, int, uint);
int memcmp(const void*, const void*, uint);
void *memcpy(void*, const void*, uint);

// 自定义printf（非标准库版本）
void printf(const char*, ...) __attribute__ ((format (printf, 1, 2)));
void fprintf(int, const char*, ...) __attribute__ ((format (printf, 2, 3)));

// 自定义内存管理（非标准库malloc）
void* malloc(uint);
void free(void*);