#ifndef __ATOMIC_H__
#define __ATOMIC_H__

static inline int a_cas(volatile int *p, int t, int s)
{
	__asm__( "lock ; cmpxchg %3, %1"
		: "=a"(t), "=m"(*p) : "a"(t), "r"(s) : "memory" );
	return t;
}

static inline void a_store(volatile int *p, int x)
{
	__asm__( "mov %1, %0" : "=m"(*p) : "r"(x) : "memory" );
}

#endif
