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
#include <stdlib.h>

#include "set.h"

#define set_lock(set)		lock_wait(set->lock);
#define set_unlock(set)		lock_post(set->lock);

int
_comparison_func(const void *a, const void *b, void *p)
{
	item_t item_a = (item_t)a;
	item_t item_b = (item_t)b;

	if (item_a->key > item_b->key)
		return 1;
	else if (item_a->key < item_b->key)
		return -1;
	else /* item_a->key == item_b->key */
		return 0;
}

bool
set_create(set_t *result)
{
	set_t set;

	set = malloc(sizeof(*set));
	if (set == NULL)
		return false;
	
	assert(lock_create(&set->lock));
	set->tree = rb_create(_comparison_func, NULL);
	*result = set;

	return true;
}

bool
set_destroy(set_t set)
{
	lock_t lock;

	if (set == NULL)
		return false;

	set_lock(set);
	lock = set->lock;

	rb_destroy(set->tree, NULL);
	free(set);

	lock_post(lock);
	lock_destroy(lock);

	return true;
}

bool
set_insert(set_t set, global_id_t key, void *data)
{
	bool retval = false;
	item_t item;

	assert(item = malloc(sizeof(*item)));
	item->key = key;
	item->data = data;

	set_lock(set);
	item = rb_insert(set->tree, item);
	set_unlock(set);

	if (item != NULL)
	{
		free(item);
	}
	else
	{
		retval = true;
	}

	return retval;
}

bool
set_remove(set_t set, global_id_t key)
{
	bool retval = false;
	struct _item stub = { .key = key };
	item_t item;

	set_lock(set);
	item = rb_delete(set->tree, &stub);
	set_unlock(set);

	if (item != NULL)
	{
		free(item);
		retval = true;
	}

	return retval;
}

bool
set_find(set_t set, global_id_t key, void **result)
{
	bool retval = false;
	struct _item stub = { .key = key };
	item_t item;

	set_lock(set);
	item = rb_find(set->tree, &stub);
	set_unlock(set);

	if (item != NULL)
	{
		if (result != NULL)
			*result = item->data;
		retval = true;
	}

	return retval;
}

void *
set_next(set_t set)
{
	item_t item;

	set_lock(set);
	item = rb_traverse(set->tree, &set->trav);
	set_unlock(set);

	if (item == NULL)
		return NULL;

	return item->data;
}

