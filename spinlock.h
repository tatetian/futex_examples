#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	volatile unsigned int lock;
} spinlock_t;

/*
 * Originally, Linux defines these macros to introduce sub-sections.
 * I don't think it is going to hurt the correctness of spinlock by removing
 * sub-sections.
 *
 * What is sub-sections in assembly code?
 *
 * Assembled bytes conventionally fall into two sections: text and data. You may
 * have separate groups of data in named sections that you want to end up near
 * to each other in the object file, even though they are not contiguous in the
 * assembler source. as allows you to use subsections for this purpose. Within
 * each section, there can be numbered subsections with values from 0 to 8192.
 * Objects assembled into the same subsection go into the object file together
 * with other objects in the same subsection.
 */
#define LOCK_SECTION_NAME
#define LOCK_SECTION_START(extra)
#define LOCK_SECTION_END


/*
 * CPU relax = REP NOP = PAUSE, which is a good thing to insert into
 * busy-wait loops.
 */
#define cpu_relax()   rep_nop()
static inline void rep_nop(void)
{
	__asm__ __volatile__("rep;nop": : :"memory");
}

#define barrier()		__asm__ __volatile__("": : :"memory")
#define preempt_enable()	barrier()
#define preempt_disable()	barrier()
#define likely(x)		__builtin_expect(!!(x), 1)
#define unlikely(x)		__builtin_expect(!!(x), 0)
#define inline			__inline__ __attribute__((always_inline))

/*
 * Init
 */
#define SPIN_LOCK_INITIALIZER	{ 1 }
#define SPIN_LOCK_UNLOCKED	(spinlock_t) { 1 }
#define spin_lock_init(x)	do { *(x) = SPIN_LOCK_UNLOCKED; } while(0)

/*
 * Check whether it is locked
 */
#define spin_is_locked(x)	(*(volatile signed char *)(&(x)->lock) <= 0)
#define spin_unlock_wait(x)	do { barrier(); } while(spin_is_locked(x))

/*
 * Try lock
 */
#define spin_trylock(lock)	({preempt_disable(); _raw_spin_trylock(lock) ? \
				1 : ({preempt_enable(); 0;});})

static inline int _raw_spin_trylock(spinlock_t *lock)
{
	char oldval;
	__asm__ __volatile__(
		"xchgb %b0,%1"
		:"=q" (oldval), "=m" (lock->lock)
		:"0" (0) : "memory");
	return oldval > 0;
}

/*
 * Lock
 */
#define spin_lock(lock) \
do { \
	preempt_disable(); \
	if (unlikely(!_raw_spin_trylock(lock))) \
		__preempt_spin_lock(lock); \
} while (0)

/*
 * Note: jmp <name>f means jumping to the first found label <name> search
 * __forward__ and jmp <name>b means jumping to the first found label <name>
 * searching __back__.
 */
#define spin_lock_string \
	"\n1:\t" \
	"lock ; decb %0\n\t" \
	"js 2f\n" \
	LOCK_SECTION_START("") \
	"2:\t" \
	"rep;nop\n\t" \
	"cmpb $0,%0\n\t" \
	"jle 2b\n\t" \
	"jmp 1b\n" \
	LOCK_SECTION_END

static inline void _raw_spin_lock(spinlock_t *lock)
{
	__asm__ __volatile__(
		spin_lock_string
		:"=m" (lock->lock) : : "memory");
}

static inline void __preempt_spin_lock(spinlock_t *lock)
{
	do {
		preempt_enable();
		while (spin_is_locked(lock))
			cpu_relax();
		preempt_disable();
	} while (!_raw_spin_trylock(lock));
}

/*
 * Unlock
 */
#define spin_unlock(lock) \
do { \
	_raw_spin_unlock(lock); \
	preempt_enable(); \
} while (0)

#define spin_unlock_string \
	"xchgb %b0, %1" \
		:"=q" (oldval), "=m" (lock->lock) \
		:"0" (oldval) : "memory"

static inline void _raw_spin_unlock(spinlock_t *lock)
{
	char oldval = 1;
	__asm__ __volatile__(
		spin_unlock_string
	);
}

#ifdef __cplusplus
}
#endif

#endif /* __SPINLOCK_H__ */
