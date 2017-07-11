#if !defined(__PLATFORM_H__)
#define __PLATFORM_H__

#include "agent.h"

typedef struct platform {

	char *profile;
	char *version;
	char *name;
	char *vendor;
	char *extensions;

	provider_t provider;
	consumer_t consumer;
	cl_platform_id id;
	global_id_t global_id;

	set_t devices;				/* set of device_t */

} *platform_t;

bool platform_create(platform_t *ppf, provider_t);
bool platform_destroy(platform_t pf);
bool platform_init(platform_t, const char *);
platform_t agent_find_platform_by_global_id(agent_t, global_id_t);

#endif /* __PLATFORM_H__ */
