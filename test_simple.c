#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "futex.h"

#define NTHREADS	5

void* wait(void* data) {
	int* val = (int*) data;
	futex(val, FUTEX_WAIT, 1, NULL, NULL, 0);
	return NULL;
}

int main() {
	pthread_t sleeping_threads[NTHREADS];
	int val = 1;
	for (int ti = 0; ti < NTHREADS; ti++) {
		pthread_create(&sleeping_threads[ti], NULL, wait, &val);
	}
	sleep(1);
	val = 0;
	futex(&val, FUTEX_WAKE, NTHREADS, NULL, NULL, 0);
	for (int ti = 0; ti < NTHREADS; ti++)
		pthread_join(sleeping_threads[ti], NULL);
	return 0;
}
