#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void handle_term(int no, siginfo_t *info, void *context)
{
	static const char msg[] = "Stil alive!\n";

	(void) no;
	(void) info;
	(void) context;

	write(1, msg, sizeof(msg) - 1);
}

int main()
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = &handle_term;
	sa.sa_flags = SA_SIGINFO;

	if (sigaction(SIGTERM, &sa, NULL) < 0) {
		perror("sigaction failed");
		exit(1);
	}

	if (sigaction(SIGINT, &sa, NULL) < 0) {
		perror("sigaction failed");
		exit(1);
	}

	while (1)
		sleep(1);

	return 0;
}
