#include "agent.h"

agent_t agent;

void
shutdown_handler(int signum)
{
	if (!agent_shutdown(agent))
	{
		debug_printff("agent_shutdown failed");
	}
	else if (!agent_destroy(agent))
	{
		debug_printff("agent_destroy failed");
	}
}

int
main(int argc, char *argv[])
{
	int rc;

#if !defined(NDEBUG)
	clara_printf("Debug output: %sabled\n", clara_debug_init() ? "en" : "dis");
#endif

	struct agent_options opts = {
		.addr = AGENT_DEFAULT_ADDR,
		.port = AGENT_DEFAULT_PORT,
		.load_balancing = false,
		.reverse_proxy = false,
	};

	int c;

	while ((c = getopt(argc, argv, "bra:p:")) != -1)
	{
		switch (c)
		{
		case 'b':
			opts.load_balancing = true;
			break;
		case 'r':
			opts.reverse_proxy = true;
			break;
		case 'a':
			opts.addr = optarg;
			break;
		case 'p':
			opts.port = optarg;
			break;
		default:
			return 1;
			break;
	 	}
	}

	/* ignore SIGPIPE, because sending a message to a disappeared client would close the server */
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	act.sa_handler = SIG_IGN;
	act.sa_flags = 0;
	sigaction(SIGPIPE, &act, NULL);

	act.sa_handler = &shutdown_handler;
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGTERM, &act, NULL);

	/* init pseudo-random number generator */
	clara_srand();

	if (!agent_create(&agent, &opts))
	{
		clara_printf("Could not create agent\n");
		return 1;
	}
	else
	if (!agent_startup(agent))
	{
		clara_printf("Could not start agent\n");
		return 1;
	}
	else
	if ((rc = agent_main(agent)) != 0)
	{
		switch (errno)
		{
		case 0:
			/* don't report */
			break;
		case EINVAL:
			clara_printf("Invalid arguments.\n");
			break;
		case EINTR:
			clara_printf("Interrupted.\n");
			break;
		case EADDRNOTAVAIL:
			clara_printf("Cannot assign requested address.\n");
			break;
		default:
			debug_printf("main: agent failed, errno = %d\n", errno);
			break;
		}
	}

	return (rc);
}

