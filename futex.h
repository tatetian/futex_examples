#ifndef __FUTEX_H__
#define __FUTEX_H__

#include <linux/futex.h>
#include <time.h>

long futex(unsigned int *uaddr, int op, int val,
		struct timespec *utime, unsigned int *uaddr2, int val3);

#endif
