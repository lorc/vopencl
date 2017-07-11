AM_CFLAGS = -I../../include
bin_PROGRAMS = clarac
clarac_SOURCES = \
	provider.c \
	provider_device.c \
	query.c \
	opencl_device.c \
	opencl_context.c \
	opencl_cmdqueue.c \
	opencl_memobj.c \
	opencl_program.c \
	opencl_kernel.c \
	opencl_event.c \
	../../common/aux.c \
	../../common/net.c \
	../../common/block.c \
	../../common/stream.c \
	../../common/api.c \
	../../common/rpc_server.c \
	../../common/map.c \
	../../common/alloc.c \
	../../common/rb.c \
	../../common/set.c \
	../../common/lock.c

run:
	./$(bin_PROGRAMS) -a 192.168.4.106 -d 0
