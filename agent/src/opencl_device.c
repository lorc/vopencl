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


#include "agent.h"
#include "agent_opencl.h"

CLARA_POST_CLFUN_HANDLER(clGetDeviceIDs)
{
	args->argv[0] = CL_SUCCESS;
	global_id_t platform = args->argv[1];
	cl_device_type device_type = args->argv[2];
	cl_uint num_entries = args->argv[3];
	cl_device_id *devices = ARG2PTR(args->argv[4]);
	cl_uint *num_devices = ARG2PTR(args->argv[5]);

#if 0
	debug_printff("(%lx, %lx, %lu, %lx, %lx)", (arg_t)platform,
		(arg_t)device_type, (arg_t)num_entries,
		PTR2ARG(devices), PTR2ARG(num_devices));
#endif

	if (num_devices != NULL)
		*num_devices = 0;

	if (platform == INVALID_ID)
		return;

	int b;

	platform_t platf;
	device_t device;

	platf = agent_find_platform_by_global_id(args->source->agent, platform);
	if (platf == NULL)
	{
		args->argv[0] = CL_INVALID_PLATFORM;
		return;
	}

	if (num_devices != NULL)
	{
		*num_devices = set_count(platf->devices);
	}

	if (devices != NULL)
	{
		b = 0;

		set_start(platf->devices);

		while ((device = set_next(platf->devices)) != NULL)
		{
			if (b == num_entries)
				break;
	
			/* doesn't respect default type */
			if ((device->type & device_type) == 0)
				continue;
	
			devices[b++] = ARG2PTR(device->global_id);
		}

		set_finish(platf->devices);
	}
}

CLARA_PRE_CLFUN_HANDLER(clGetDeviceInfo)
{
	args->params->hint = 0;
}

CLARA_POST_CLFUN_HANDLER(clGetDeviceInfo)
{
	args->argv[0] = CL_SUCCESS;

	global_id_t device_id = args->argv[1];
	cl_device_info param_name = (cl_device_info)args->argv[2];
	size_t param_value_size = (size_t)args->argv[3];
	void *param_value = ARG2PTR(args->argv[4]);
	size_t *param_value_size_ret = ARG2PTR(args->argv[5]);

	device_t device = agent_find_device_by_global_id(args->source->agent, device_id);
	cl_bool device_available;
	void *param_buf;

	if (device == NULL)
	{
		args->argv[0] = CL_INVALID_DEVICE;
		return;
	}

	size_t value_size = 0;

	switch (param_name)
	{
	case CL_DEVICE_TYPE:
		param_buf = &device->type;
		value_size = sizeof(cl_device_type);
		break;

	case CL_DEVICE_VENDOR_ID:
		param_buf = &device->vendor_id;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_MAX_COMPUTE_UNITS:
		param_buf = &device->max_compute_units;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:
		param_buf = &device->max_work_item_dimensions;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_MAX_WORK_ITEM_SIZES:
		param_buf = device->max_work_item_sizes;
		value_size = sizeof(size_t) * device->max_work_item_dimensions;
		break;

	case CL_DEVICE_MAX_WORK_GROUP_SIZE:
		param_buf = &device->max_work_group_size;
		value_size = sizeof(size_t);
		break;

	case CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
		param_buf = &device->preferred_vector_width_char;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
		param_buf = &device->preferred_vector_width_short;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT:
		param_buf = &device->preferred_vector_width_int;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
		param_buf = &device->preferred_vector_width_long;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
		param_buf = &device->preferred_vector_width_float;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:
		param_buf = &device->preferred_vector_width_double;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_MAX_CLOCK_FREQUENCY:
		param_buf = &device->max_clock_frequency;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_ADDRESS_BITS:
		param_buf = &device->address_bits;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_MAX_MEM_ALLOC_SIZE:
		param_buf = &device->max_mem_alloc_size;
		value_size = sizeof(cl_ulong);
		break;

	case CL_DEVICE_IMAGE_SUPPORT:
		param_buf = &device->image_support;
		value_size = sizeof(cl_bool);
		break;

	case CL_DEVICE_MAX_READ_IMAGE_ARGS:
		param_buf = &device->max_read_image_args;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_MAX_WRITE_IMAGE_ARGS:
		param_buf = &device->max_write_image_args;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_IMAGE2D_MAX_WIDTH:
		param_buf = &device->image2d_max_width;
		value_size = sizeof(size_t);
		break;

	case CL_DEVICE_IMAGE2D_MAX_HEIGHT:
		param_buf = &device->image2d_max_height;
		value_size = sizeof(size_t);
		break;

	case CL_DEVICE_IMAGE3D_MAX_WIDTH:
		param_buf = &device->image3d_max_width;
		value_size = sizeof(size_t);
		break;

	case CL_DEVICE_IMAGE3D_MAX_HEIGHT:
		param_buf = &device->image3d_max_height;
		value_size = sizeof(size_t);
		break;

	case CL_DEVICE_IMAGE3D_MAX_DEPTH:
		param_buf = &device->image3d_max_depth;
		value_size = sizeof(size_t);
		break;

	case CL_DEVICE_MAX_SAMPLERS:
		param_buf = &device->max_samplers;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_MAX_PARAMETER_SIZE:
		param_buf = &device->max_parameter_size;
		value_size = sizeof(size_t);
		break;

	case CL_DEVICE_MEM_BASE_ADDR_ALIGN:
		param_buf = &device->mem_base_addr_align;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE:
		param_buf = &device->min_data_type_align_size;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_SINGLE_FP_CONFIG:
		param_buf = &device->single_fp_config;
		value_size = sizeof(cl_device_fp_config);
		break;

	case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE:
		param_buf = &device->global_mem_cache_type;
		value_size = sizeof(cl_device_mem_cache_type);
		break;

	case CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE:
		param_buf = &device->global_mem_cacheline_size;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_GLOBAL_MEM_CACHE_SIZE:
		param_buf = &device->global_mem_cache_size;
		value_size = sizeof(cl_ulong);
		break;

	case CL_DEVICE_GLOBAL_MEM_SIZE:
		param_buf = &device->global_mem_size;
		value_size = sizeof(cl_ulong);
		break;

	case CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:
		param_buf = &device->max_constant_buffer_size;
		value_size = sizeof(cl_ulong);
		break;

	case CL_DEVICE_MAX_CONSTANT_ARGS:
		param_buf = &device->max_constant_args;
		value_size = sizeof(cl_uint);
		break;

	case CL_DEVICE_LOCAL_MEM_TYPE:
		param_buf = &device->local_mem_type;
		value_size = sizeof(cl_device_local_mem_type);
		break;

	case CL_DEVICE_LOCAL_MEM_SIZE:
		param_buf = &device->local_mem_size;
		value_size = sizeof(cl_ulong);
		break;

	case CL_DEVICE_ERROR_CORRECTION_SUPPORT:
		param_buf = &device->error_correction_support;
		value_size = sizeof(cl_bool);
		break;

	case CL_DEVICE_PROFILING_TIMER_RESOLUTION:
		param_buf = &device->profiling_timer_resolution;
		value_size = sizeof(size_t);
		break;

	case CL_DEVICE_ENDIAN_LITTLE:
		param_buf = &device->endian_little;
		value_size = sizeof(cl_bool);
		break;

	case CL_DEVICE_AVAILABLE:
		device_available = agent_device_is_available(device);
		param_buf = &device_available;
		value_size = sizeof(cl_bool);
		break;

	case CL_DEVICE_COMPILER_AVAILABLE:
		param_buf = &device->compiler_available;
		value_size = sizeof(cl_bool);
		break;

	case CL_DEVICE_EXECUTION_CAPABILITIES:
		param_buf = &device->execution_capabilities;
		value_size = sizeof(cl_device_exec_capabilities);
		break;

	case CL_DEVICE_QUEUE_PROPERTIES:
		param_buf = &device->queue_properties;
		value_size = sizeof(cl_command_queue_properties);
		break;

	case CL_DEVICE_PLATFORM:
		param_buf = &device->platform_id;
		value_size = sizeof(cl_platform_id);
		break;

	case CL_DEVICE_NAME:
		param_buf = device->name;
		value_size = strlen(param_buf) + 1;
		break;

	case CL_DEVICE_VENDOR:
		param_buf = device->vendor;
		value_size = strlen(param_buf) + 1;
		break;

	case CL_DRIVER_VERSION:
		param_buf = device->driver_version;
		value_size = strlen(param_buf) + 1;
		break;

	case CL_DEVICE_PROFILE:
		param_buf = device->profile;
		value_size = strlen(param_buf) + 1;
		break;

	case CL_DEVICE_VERSION:
		param_buf = device->version;
		value_size = strlen(param_buf) + 1;
		break;

	case CL_DEVICE_EXTENSIONS:
		param_buf = device->extensions;
		value_size = strlen(param_buf) + 1;
		break;

	default:
		args->argv[0] = CL_INVALID_VALUE;
		return;
	}

	if (param_value_size_ret != NULL)
		*param_value_size_ret = value_size;

	if (param_value != NULL)
	{
		if (param_value_size < value_size)
		{
			args->argv[0] = CL_INVALID_VALUE;
			return;
		}
		memcpy(param_value, param_buf, value_size);
	}
}

