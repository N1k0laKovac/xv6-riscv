# user_standalone/Makefile

CC      := riscv64-unknown-elf-gcc
LD      := riscv64-unknown-elf-gcc

CMODEL  := -mcmodel=medany
CFLAGS  := -O2 -ffreestanding -nostdlib $(CMODEL) -I. -Ikernel -Iuser
LDFLAGS := -T user.ld -nostdlib -static $(CMODEL)

# 目标可执行文件改成 mcat（防止和 kernel/ 冲突）
PROG    := mcat

# 对应的 .o 列表
OBJS    := start.o \
           kernel/trap.o kernel/kernel.o kernel/syscall.o \
           user/hello.o user/world.o user/mcat.o

# 最终链接时要把 libuser.a 也拉进去
all: $(PROG)

# 链接命令：这里依次传入 .o，再传入 libuser.a
$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) libuser.a

# 以下各行前面必须是 Tab！
start.o: start.S
	$(CC) $(CFLAGS) -c $< -o $@

kernel/trap.o: kernel/trap.S
	$(CC) $(CFLAGS) -c $< -o $@

kernel/kernel.o: kernel/kernel.c kernel/context.h
	$(CC) $(CFLAGS) -c $< -o $@

kernel/syscall.o: kernel/syscall.c kernel/syscall.h
	$(CC) $(CFLAGS) -c $< -o $@

user/hello.o: user/hello.c
	$(CC) $(CFLAGS) -c $< -o $@

user/world.o: user/world.c
	$(CC) $(CFLAGS) -c $< -o $@

user/mcat.o: user/mcat.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(OBJS) mcat

.PHONY: all clean
