#if !defined(__OBJECT_H__)
#define __OBJECT_H__

#include "agent.h"

typedef enum _object_type {
	CL_PLATFORM,
	CL_DEVICE,
	CL_CONTEXT,
	CL_COMMANDQUEUE,
	CL_MEMOBJECT,
	CL_SAMPLER,
	CL_PROGRAM,
	CL_KERNEL,
	CL_EVENT
} object_type_t;

typedef struct _object {

	global_id_t id;
	void *real_id;
	uint32_t refcnt;
	lock_t lock;
	provider_t provider;
	consumer_t consumer;

} *object_t;

bool object_create(object_t *result, provider_t provider, void *real_id, global_id_t id);
bool object_retain(object_t object, consumer_t consumer);
bool object_release(object_t object);
bool object_destroy(object_t object);

#endif /* __OBJECT_H__ */
