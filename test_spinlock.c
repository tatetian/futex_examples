#include "spinlock.h"
#include <pthread.h>
#include <stdio.h>

#define NTHREADS		2

typedef struct {
	spinlock_t		lock;
	unsigned long		nrepeats;
	volatile int		inside_critical;
} thread_data_t;

static inline void pause() {
	int nrepeats = 100;
	while (nrepeats--) cpu_relax();
}

void* spin(void* _data) {
	thread_data_t* data = (thread_data_t*) _data;
	spinlock_t* lock = &data->lock;
	unsigned long nrepeats = data->nrepeats;
	while (nrepeats--) {
		spin_lock(lock);
		if (data->inside_critical) {
			spin_unlock(lock);
			return (void*)-1;
		}
		data->inside_critical = 1;
		data->inside_critical = 0;
		spin_unlock(lock);
	}
	return (void*)0;
}

int main() {
	pthread_t threads[NTHREADS];
	thread_data_t data = {
		SPIN_LOCK_INITIALIZER,
		10000000UL,
		0
	};
	for (int ti = 0; ti < NTHREADS; ti++) {
		pthread_create(&threads[ti], NULL, spin, &data);
	}
	for (int ti = 0; ti < NTHREADS; ti++) {
		long ret = 0;
		pthread_join(threads[ti], (void**)&ret);
		if (ret) {
			printf("Error!\n");
			return -1;
		}
	}
	return 0;
}
