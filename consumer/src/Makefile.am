AM_CFLAGS = -I../../include
ACLOCAL_AMFLAGS=-I m4
SUBDIRS = include
lib_LTLIBRARIES = libclara.la
libclara_la_LDFLAGS = -version-info `grep -v '^\#' $(srcdir)/libtool-version` $(AM_LTLDFLAGS)
libclara_la_SOURCES = \
	consumer.c \
	opencl_cmdqueue.c \
	opencl_context.c \
	opencl_device.c \
	opencl_event.c \
	opencl_image.c \
	opencl_kernel.c \
	opencl_memobj.c \
	opencl_platform.c \
	opencl_program.c \
	opencl_sampler.c \
	../../common/aux.c \
	../../common/net.c \
	../../common/block.c \
	../../common/stream.c \
	../../common/api.c \
	../../common/rpc_client.c

run:
	$(CC) $(CFLAGS) -o $(TARGET) $(LIBS) *.o ../testsuite/$(TARGET).c
	env AGENT_ADDR=192.168.4.106 ./$(TARGET)
