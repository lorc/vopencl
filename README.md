# vopencl
Virtual network-remote OpenCL library. Cloned from https://sourceforge.net/projects/clara/
Note: This is preliminary documentation. Things are subjected to changes.

=== Introduction ===

CLara is a distributed OpenCL implementation. It consists of three components:

 * a reverse proxy server (clarad)
 * an OpenCL platform provider (clarac)
 * an OpenCL application library (libclara)

The major goal of CLara is to be able to execute OpenCL applications on hosts
that doesn't have a GPGPU-capable graphics adapter or configuration.

At first you install and run the reverse proxy server on an arbitrary host in
the network. The next step is to start the OpenCL provider software on the
hosts that have an GPGPU-capable device installed. The provider requires the
vendor-specific OpenCL library on these machines. It connects to the reverse
proxy that will provide the OpenCL device to so called consumers. The
consumer is your OpenCL application that you may have already written and
compiled. You need to link it to the CLara library that will handle the
communication with the reverse proxy and the provider. The following drawing
shows the components and layers of this model:

  [ NVIDIA GeForce device  ]    [ ATI Radeon device  ]    [ IBM CELL device  ]
  [      NVIDIA CUDA       ]    [    ATI Stream      ]    [     libspe2      ]
  [     NVIDIA OpenCL      ]    [    AMD OpenCL      ]    [    IBM OpenCL    ]
  [ ####### clarac ####### ]    [ ##### clarac ##### ]    [ #### clarac #### ]
              |                           |                         |
              |                           |                         |
               \                          |                        /
                \_____________________    |    ___________________/
                                      \   |   /
                                       \  |  /
                                        \ | /
                                 [ +++ clarad +++ ]
                                        / | \
                                       /  |  \
                              ________/   |   \_________
                             /            |             \
                            /             |              \
       [ **** libclara **** ]   [ **** libclara **** ]   [ **** libclara **** ]
       [ OpenCL application ]   [ OpenCL application ]   [ OpenCL application ]


=== Installation ===

The source tree consists of several directories. There are three directories
called 'provider', 'agent' and 'consumer'.

Change to the directory of the component that you want to install on the
local machine and type:

  $ ./configure
  $ make
  $ su
  # make install

 * The provider installs 'clarac' to /usr/local/bin.
 * The agent installs 'clarad' to /usr/local/bin.
 * The consumer installs headers to /usr/local/include
   and the library 'libclara.so' to /usr/local/lib.


=== Usage ===

Start the agent on the host that acts as reverse proxy:

  $ /usr/local/bin/clarad

Execute the provider tool on the host that have a GPGPU-capable device and let
it connect to the agent, in this example 192.168.4.42.

  $ /usr/local/bin/clarac -a 192.168.4.42

On the host that executes the OpenCL application set these two
environment variables to configure the location of the agent:

  $ export AGENT_ADDR=192.168.4.42
  $ export AGENT_PORT=52428

Compile your OpenCL application with the following options:

  -I/usr/local/include -L/usr/local/lib -lclara

or pre-load the library if it is already compiled and linked:

  env LD_PRELOAD=/usr/local/lib/libclara.so ./your-application

That's it. The application should run within the limits of CLara. These limits are:

 * no image support yet
 * no clEnqueueNativeKernel
 * no clMap* functions

Keep also in mind that CLara is far away from being mature.
It's in pre-alpha state.

--

Bjoern Koenig
Berlin, Germany
2010-02-20

