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

#if !defined(__SET_H__)
#define __SET_H__

#include <stdbool.h>
#include <stdint.h>

#if !defined(GLOBAL_ID_T)
#define GLOBAL_ID_T
typedef uint64_t global_id_t;
#define INVALID_ID	((global_id_t)0)
#endif

#include "rb.h"
#include "lock.h"

typedef struct _item {
	global_id_t key;
	void *data;
} *item_t;

struct _set {
	lock_t lock;
	rb_tree *tree;
	rb_traverser trav;
};

#if !defined(SET_T)
#define SET_T
typedef struct _set *set_t;
#endif

bool set_create(set_t *);
bool set_destroy(set_t);

bool set_insert(set_t, global_id_t, void *);
bool set_remove(set_t, global_id_t);
bool set_find(set_t, global_id_t, void **);

static inline void
set_start(set_t set)
{
	lock_wait(set->lock);
	rb_init_traverser(&set->trav);
}

static inline void
set_finish(set_t set)
{
	lock_post(set->lock);
}

void *set_next(set_t);

static inline int
set_count(set_t set)
{
	return rb_count(set->tree);
}

#endif /* __SET_H__ */

