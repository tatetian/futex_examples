#include "futex.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

static __inline long __syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
	unsigned long ret;
	register long r10 __asm__("r10") = a4;
	register long r8 __asm__("r8") = a5;
	register long r9 __asm__("r9") = a6;
	__asm__ __volatile__ ("syscall" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2),
						  "d"(a3), "r"(r10), "r"(r8), "r"(r9) : "rcx", "r11", "memory");
	return ret;
}


long futex(unsigned int *uaddr, int op, int val,
		struct timespec *utime, unsigned int *uaddr2, int val3) {
	return __syscall6(__NR_futex, (long)uaddr, (long)op, (long)val, (long)utime, (long)uaddr2, (long)val3);
}
