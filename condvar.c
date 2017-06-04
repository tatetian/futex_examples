#include "condvar.h"

int condvar_init(condvar_t* condvar) {
	condvar->futex = 0;
	return 0;
}

/* Wait on the condition until being signaled
 *
 * Precondition:
 *	1) the calling thread holds the mutex
 *	2) this conditional varaible is always protected by this mutex
 * Postcondition: the calling thread still holds the mutex
 * */
int condvar_wait(condvar_t* condvar, mutex_t* mutex) {
	if (mutex == NULL) return -1;

	int curr_futex = condvar->futex;
	mutex_unlock(mutex);

	futex((unsigned int*) (&condvar->futex), FUTEX_WAIT, curr_futex, NULL, NULL, 0);
	mutex_lock(mutex);
	return 0;
}

/* Wake up one thread that is waiting on the condition
 *
 * Precondition: the calling thread holds the mutex
 * */
int condvar_signal(condvar_t* condvar) {
	condvar->futex++;
	futex((unsigned int*) (&condvar->futex), FUTEX_WAKE, 1, NULL, NULL, 0);
	return 0;
}
