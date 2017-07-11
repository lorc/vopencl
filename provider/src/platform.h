#if !defined(__PLATFORM_H__)
#define __PLATFORM_H__

typedef struct _platform {

	cl_platform_id id;
	global_id_t global_id;

	set_t devices;

} *platform_t;

#endif /* __PLATFORM_H__ */
