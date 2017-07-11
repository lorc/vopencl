/*-
 * Copyright 2009, 2010 Bjoern Koenig
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "clara.h"
#include "lock.h"

bool
lock_create(lock_t *result)
{
	lock_t lock;

	lock = malloc(sizeof(*lock));
	if (lock == NULL)
		return false;

	assert((lock->sem = malloc(sizeof(*lock->sem))) != NULL);
	assert(sem_init(lock->sem, 0, 1) == 0);

	lock->owner = 0;
	lock->cnt = 0;

	*result = lock;
//	debug_printff("(%lx)", lock);
	return true;
}

bool
lock_destroy(lock_t lock)
{
	sem_t *sem;
//	debug_printff("(%lx)", lock);

	if (lock == NULL)
		return false;

	lock_wait(lock);
	sem = lock->sem;
	if (lock->cnt != 1)
	{
		debug_printff("(%lx) you destroy a locked lock"
			" (cnt=%u, owner=%lx, self=%lx)",
			lock, lock->cnt, lock->owner, pthread_self());
	}

	free(lock);
	assert(sem_destroy(sem) == 0);
	free(sem);

	return true;
}

void
lock_wait(lock_t lock)
{
	pthread_t tid = pthread_self();
	assert(tid != 0);

	if ((lock->owner == 0) || (lock->owner != tid))
	{
		assert(sem_wait(lock->sem) == 0);
		if (lock->owner == 0)
			lock->owner = tid;
	}
	lock->cnt++;
//	debug_printff("(%lx) tid=%lx cnt=%u", lock, tid, lock->cnt);
}

void
lock_post(lock_t lock)
{
	pthread_t tid = pthread_self();
//	debug_printff("(%lx) tid=%lx cnt=%u", lock, tid, lock->cnt);
	assert(tid != 0);
	assert(lock->owner == tid);
	if (--lock->cnt == 0)
	{ 
		lock->owner = 0;
		assert(sem_post(lock->sem) == 0);
	}
}

