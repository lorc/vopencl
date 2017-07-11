#include "object.h"

bool
object_create(object_t *result, provider_t provider, void *real_id, global_id_t id)
{
	object_t object;

	if ((real_id == NULL) || (provider == NULL) || (id == INVALID_ID))
	{
		errno = EINVAL;
		return false;
	}

	if ((object = calloc(1, sizeof(*object))) == NULL)
	{
		errno = ENOMEM;
		return false;
	}

	assert(lock_create(&object->lock));

	object->provider = provider;

	object->real_id = real_id;
	object->id = id;

	agent_t agent = provider->agent;
	assert(set_insert(agent->objects, id, object));

	if (result != NULL)
		*result = object;

	return true;
}

bool
object_retain(object_t object, consumer_t consumer)
{
	lock_wait(object->lock);
	if ((object->refcnt > 0) && (consumer != object->consumer))
	{
		debug_printff("access violation");
		lock_post(object->lock);
		return false;
	}
	else if (object->refcnt == 0)
	{
		object->consumer = consumer;
		assert(set_insert(consumer->objects, object->id, object));
	}
	object->refcnt++;
	lock_post(object->lock);

	return true;
}

bool
object_release(object_t object)
{
	lock_wait(object->lock);
	object->refcnt--;

	if (object->refcnt == 0)
	{
		assert(set_remove(object->consumer->objects, object->id));
		object->consumer = NULL;
	}

	lock_post(object->lock);

	return true;
}

bool
object_destroy(object_t object)
{
//	for now assume that the object is locked
//	lock_wait(object->lock);
	if (object->refcnt != 0)
	{
		debug_printff("(%lx) refcnt = %u", object->refcnt);
		return false;
	}

	agent_t agent = object->provider->agent;
	assert(set_remove(agent->objects, object->id));

	lock_t lock;
	lock = object->lock;
	free(object);
	lock_post(lock);
	lock_destroy(lock);
	return true;
}

