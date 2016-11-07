#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	sigset_t mask, orig;

	sigemptyset(&mask);
	sigaddset(&mask, SIGTERM);

	if (sigprocmask(SIG_BLOCK, &mask, &orig) < 0) {
		perror("sigprocmask failed");
		exit(1);
	}

	sleep(20);

	if (sigprocmask(SIG_SETMASK, &orig, NULL) < 0) {
		perror("sigprocmask failed");
		exit(1);
	}

	while (1)
		sleep(1);

	return 0;
}
