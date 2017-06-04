#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "condvar.h"

#define NTHREADS		15

#define BSIZE			256

/* Multi-producer, multi-consumer buffer */
typedef struct {
	unsigned char	buf[BSIZE];
	int		occupied;
	int		nextin;
	int		nextout;
	mutex_t		mutex;
	condvar_t	not_full;
	condvar_t	not_empty;
} mpmc_buffer_t;

#define MPMC_BUFFER_INITIALIZER			\
{						\
	.buf		= {0},			\
	.occupied	= 0,			\
	.nextin		= 0,			\
	.nextout	= 0,			\
	.mutex		= MUTEX_INITIALIZER,	\
	.not_full	= CONDVAR_INITIALIZER,	\
	.not_empty	= CONDVAR_INITIALIZER	\
}

void produce(mpmc_buffer_t* buf, unsigned char c) {
	mutex_lock(&buf->mutex);
	while (buf->occupied == BSIZE) {
		condvar_wait(&buf->not_full, &buf->mutex);
	}
	buf->buf[buf->nextin++] = c;
	if (buf->nextin == BSIZE) buf->nextin = 0;
	buf->occupied++;
	condvar_signal(&buf->not_empty);
	mutex_unlock(&buf->mutex);
}

unsigned char consume(mpmc_buffer_t* buf) {
	char c;
	mutex_lock(&buf->mutex);
	while (buf->occupied == 0) {
		condvar_wait(&buf->not_empty, &buf->mutex);
	}
	c = buf->buf[buf->nextout++];
	if (buf->nextout == BSIZE) buf->nextout = 0;
	buf->occupied--;
	condvar_signal(&buf->not_full);
	mutex_unlock(&buf->mutex);
	return c;
}


struct thread_data {
	mpmc_buffer_t		buf;
	unsigned int		total;
	unsigned long		producer_sum;
	unsigned long		consumer_sum;
};

void* producer(void* _data) {
	struct thread_data* data = (struct thread_data*) _data;
	unsigned char c = 0;
	unsigned int remain = data->total;
	while (remain--) {
		c = rand() % 256;
		data->producer_sum += c;
		produce(&data->buf, c);
	}
	return NULL;
}

void* consumer(void* _data) {
	struct thread_data* data = (struct thread_data*) _data;
	unsigned char c = 0;
	unsigned int remain = data->total;
	while (remain--) {
		c = consume(&data->buf);
		data->consumer_sum += c;
	}
	return NULL;
}

int main() {
	struct thread_data data = {
		.buf = MPMC_BUFFER_INITIALIZER,
		.total = 100000,
		.producer_sum = 0,
		.consumer_sum = 0,
	};

	pthread_t producer_thread, consumer_thread;
	pthread_create(&producer_thread, NULL, producer, (void*) &data);
	pthread_create(&consumer_thread, NULL, consumer, (void*) &data);
	pthread_join(producer_thread, NULL);
	pthread_join(consumer_thread, NULL);
	if (data.producer_sum == data.consumer_sum) {
		printf("Result correct!\n");
	}
	else {
		printf("Result incorrect!\n");
	}
	return 0;
}
