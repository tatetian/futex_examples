#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include "spinlock.h"
#include "futex.h"

#define NTHREADS	2
#define NSWITCHES	10000000

typedef struct {
	spinlock_t*	lock;
	volatile int*	count_addr;
	int		odd;
} thread_data_t;

void* switch2(void* _data) {
	thread_data_t* data = (thread_data_t*) _data;
	spinlock_t* lock = data->lock;
	volatile int* count_addr = data->count_addr;
	int odd = data->odd;
	int count;
	while (1) {
		spin_lock(lock);
		count = *count_addr;
		if (count <= 0) {
			spin_unlock(lock);
			break;
		}
		while (count % 2 == odd) {
			spin_unlock(lock);
			futex((int*)count_addr, FUTEX_WAIT, count, NULL, NULL, 0);
			count = *count_addr;
			spin_lock(lock);
		}
		(*count_addr)--;
		spin_unlock(lock);

		futex((int*)count_addr, FUTEX_WAKE, 1, NULL, NULL, 0);
	}
	return NULL;
}

int main() {
	pthread_t threads[NTHREADS];
	spinlock_t lock = SPIN_LOCK_INITIALIZER;
	volatile int count = NSWITCHES;
	thread_data_t data[2] = {
		{ &lock, &count, 0 },
		{ &lock, &count, 1 }
	};
	// Use only CPU #1 assuming there are more than one CPUs
	cpu_set_t cpu_mask; CPU_ZERO(&cpu_mask); CPU_SET(1, &cpu_mask);
	for (int ti = 0; ti < NTHREADS; ti++) {
		pthread_create(&threads[ti], NULL, switch2, &data[ti]);
		pthread_setaffinity_np(threads[ti], sizeof(cpu_set_t), &cpu_mask);
	}
	for (int ti = 0; ti < NTHREADS; ti++)
		pthread_join(threads[ti], NULL);
	printf("# of context switches = %d\n", NSWITCHES);
	return 0;
}
