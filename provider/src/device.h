#if !defined(__DEVICE_H__)
#define __DEVICE_H__

typedef struct _device {

	cl_device_id id;
	global_id_t global_id;

} *device_t;

#endif /* __DEVICE_H__ */
