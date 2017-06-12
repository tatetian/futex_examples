#include "mutex.h"
#include "atomic.h"

int mutex_init(mutex_t* mutex) {
	mutex->lock = 0;
	return 0;
}

int mutex_lock(mutex_t* mutex) {
	int tries = 100;
	while (mutex->lock && tries--);

	while (mutex->lock || a_cas(&mutex->lock, 0, 1)) {
		futex((int*) (&mutex->lock), FUTEX_WAIT, 1, NULL, NULL, 0);
	};
	return 0;
}

int mutex_unlock(mutex_t* mutex) {
	a_store(&mutex->lock, 0);
	futex((int*) (&mutex->lock), FUTEX_WAKE, 1, NULL, NULL, 0);
	return 0;
}
