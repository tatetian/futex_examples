#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "futex.h"

typedef struct {
	volatile int	lock;
} mutex_t;

#define MUTEX_INITIALIZER { .lock = 0 }

int mutex_init(mutex_t* mutex);
int mutex_lock(mutex_t* mutex);
int mutex_unlock(mutex_t* mutex);

#endif
