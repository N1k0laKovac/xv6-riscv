// user_standalone/user.h
// ɾ�����׼���ͻ��������ʹ��xv6�Զ���ʵ��
#pragma once

typedef unsigned int uint;

// ϵͳ���ýӿ�
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int exit(int) __attribute__((noreturn));
int fork(void);
int wait(int*);
int open(const char*, int);

// �û�̬�⺯������ulib.c��ʵ�֣�
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char);
uint strlen(const char*);
void *memset(void*, int, uint);
int memcmp(const void*, const void*, uint);
void *memcpy(void*, const void*, uint);

// �Զ���printf���Ǳ�׼��汾��
void printf(const char*, ...) __attribute__ ((format (printf, 1, 2)));
void fprintf(int, const char*, ...) __attribute__ ((format (printf, 2, 3)));

// �Զ����ڴ�����Ǳ�׼��malloc��
void* malloc(uint);
void free(void*);