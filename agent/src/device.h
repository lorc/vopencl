#if !defined(__DEVICE_H__)
#define __DEVICE_H__

#include "agent.h"

#pragma pack(push, 1)
typedef struct _device {
	cl_device_type	type;
	cl_uint			vendor_id;
	cl_uint			max_compute_units;
	cl_uint			max_work_item_dimensions;
	size_t *		max_work_item_sizes;
	size_t			max_work_group_size;
	cl_uint			preferred_vector_width_char;
	cl_uint			preferred_vector_width_short;
	cl_uint			preferred_vector_width_int;
	cl_uint			preferred_vector_width_long;
	cl_uint			preferred_vector_width_float;
	cl_uint			preferred_vector_width_double;
	cl_uint			max_clock_frequency;
	cl_uint			address_bits;
	cl_ulong		max_mem_alloc_size;
	cl_bool			image_support;
	cl_uint			max_read_image_args;
	cl_uint			max_write_image_args;
	size_t			image2d_max_width;
	size_t			image2d_max_height;
	size_t			image3d_max_width;
	size_t			image3d_max_height;
	size_t			image3d_max_depth;
	cl_uint			max_samplers;
	size_t			max_parameter_size;
	cl_uint			mem_base_addr_align;
	cl_uint			min_data_type_align_size;
	cl_device_fp_config		single_fp_config;
	cl_device_mem_cache_type	global_mem_cache_type;
	cl_uint			global_mem_cacheline_size;
	cl_ulong		global_mem_cache_size;
	cl_ulong		global_mem_size;
	cl_ulong		max_constant_buffer_size;
	cl_uint			max_constant_args;
	cl_device_local_mem_type	local_mem_type;
	cl_ulong		local_mem_size;
	cl_bool			error_correction_support;
	size_t			profiling_timer_resolution;
	cl_bool			endian_little;
	cl_bool			available;
	cl_bool			compiler_available;
	cl_device_exec_capabilities		execution_capabilities;
	cl_command_queue_properties		queue_properties;
	cl_platform_id	platform_id;
	char *			name;
	char *			vendor;
	char *			driver_version;
	char *			profile;
	char *			version;
	char *			extensions;

	provider_t		provider;
	cl_device_id id;
	global_id_t global_id;

} *device_t;
#pragma pack(pop)

bool device_create(device_t *pdv, provider_t provider);
bool device_destroy(device_t dv);
bool device_init(device_t device, const char *ptr);
device_t agent_find_device_by_global_id(agent_t agent, global_id_t id);
cl_bool agent_device_is_available(device_t device);

#endif /* __DEVICE_H__ */
