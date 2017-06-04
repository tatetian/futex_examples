#include <pthread.h>
#include <stdio.h>
#include "mutex.h"

#define NTHREADS		5

struct thread_data {
	mutex_t			mutex;
	unsigned long		local_count;
	volatile unsigned long	global_count;
};

void* incr(void* _data) {
	struct thread_data* data = (struct thread_data*) _data;
	unsigned long local_count = data->local_count;
	while (local_count--) {
		mutex_lock(&data->mutex);
		data->global_count++;
		mutex_unlock(&data->mutex);
	}
	return NULL;
}

int main() {
	struct thread_data data = {
		.mutex = MUTEX_INITIALIZER,
		.local_count = 1000000,
		.global_count = 0
	};
	unsigned long expected_global_count = NTHREADS * data.local_count;

	pthread_t incr_threads[NTHREADS];
	for (int ti = 0; ti < NTHREADS; ti++)
		pthread_create(&incr_threads[ti], NULL, incr, (void*) &data);
	for (int ti = 0; ti < NTHREADS; ti++)
		pthread_join(incr_threads[ti], NULL);

	if (expected_global_count == data.global_count) {
		printf("Result is correct!\n");
	}
	else {
		printf("Result is incorrect!\n");
	}
	return 0;
}
