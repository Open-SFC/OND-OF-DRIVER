/**************************************************************************
 * Copyright 2011-2012, Freescale Semiconductor, Inc. All rights reserved.
 **************************************************************************/
/*
 * File:  pspsem.h
 *
 * Description: 
 *
 *
 */
/*
 * History
 * 11 Apr 2012 - Initial implemetation 
 *
 */
/******************************************************************************/

#ifndef	_CNTLRSEM_H
#define	_CNTLREM_H
#include <unistd.h> /* size_t */
#include <linux/futex.h>
#include <stdio.h>

//#define __NR_futex 221
#define __NR_sys_futex __NR_futex

struct futex
{
	int count;
};

/*#if defined(__powerpc__)
#include "psp-ppc.h"
#elif defined(__i386__)
#include "psp-x86.h"
#endif*/
#define FUTEX_PASSED (-(1024 * 1024 * 1024))

/* Returns -1 on fail, 0 on wakeup, 1 on pass, 2 on didn't sleep */
static inline int rwlock_futex_down_slow(struct futex *futx, int val, struct timespec *rel)
{
#if 0
  if (sys_futex(&futx->count, FUTEX_WAIT, val, rel) == 0) {
#else
  if (syscall(__NR_sys_futex, &futx->count, FUTEX_WAIT, val, rel, NULL, NULL) == 0) {
#endif
    /* <= in case someone else decremented it */
    if (futx->count <= FUTEX_PASSED) {
      futx->count = -1;
      return 1;
    }
    return 0;
  }
  /* EWOULDBLOCK just means value changed before we slept: loop */
  if (errno == EWOULDBLOCK)
    return 2;
  return -1;
}

static inline int rwlock_futex_up_slow(struct futex *futx)
{
  futx->count = 1;
  rwlock_futex_commit();
#if 0
  return sys_futex(&futx->count, FUTEX_WAKE, 1, NULL);
#else

  return syscall(__NR_sys_futex, &futx->count, FUTEX_WAKE, 1, NULL, NULL, NULL);
#endif
}

static inline int futex_down_timeout(struct futex *futx, struct timespec *rel)
{
	int val, woken = 0;

	/* Returns new value */
	while ((val = rwlock_futex_down(&futx->count)) != 0) {
		switch (rwlock_futex_down_slow(futx, val, rel)) {
		case -1: return -1; /* error */
		case 1: 
			return 0; /* passed */
		case 0: woken = 1; break; /* slept */
		}
	}
	/* If we were woken, someone else might be sleeping too: set to -1 */
	if (woken) {
		futx->count = -1;
	}
	return 0;
}

/* If rwlock_futex_down decrements from 1 to 0, we have it.  Otherwise sleep. */
static inline int futex_down(struct futex *futx)
{
	return futex_down_timeout(futx, NULL);
}

static inline int futex_trydown(struct futex *futx)
{
	return (rwlock_futex_down(&futx->count) == 0 ? 0: -1);
}

/* If rwlock_futex_up increments count from 0 -> 1, noone was waiting.
   Otherwise, set to 1 and tell kernel to wake them up. */
static inline int futex_up(struct futex *futx)
{
	if (!rwlock_futex_up(&futx->count))
		return rwlock_futex_up_slow(futx);
	return 0;
}

static inline void futex_init(struct futex *futx)
{
    futx->count = 1;
      rwlock_futex_commit();
}
#endif /* _CNTLRSEM_H */
