#ifndef __CONDVAR_H__
#define __CONDVAR_H__

#include "futex.h"
#include "mutex.h"

typedef struct {
	volatile int	futex;
} condvar_t;

#define CONDVAR_INITIALIZER { .futex = 0 }

int condvar_init(condvar_t* condvar);
/* Wait on the condition until being signaled
 *
 * Precondition:
 *	1) the calling thread holds the mutex
 *	2) this conditional varaible is always protected by this mutex
 * Postcondition: the calling thread still holds the mutex
 * */
int condvar_wait(condvar_t* condvar, mutex_t* mutex);
/* Wake up one thread that is waiting on the condition
 *
 * Precondition: the calling thread holds the mutex
 * */
int condvar_signal(condvar_t* condvar);

#endif
