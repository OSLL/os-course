#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	const pid_t pid = fork();

	if (pid < 0) {
		perror("fork failed");
		exit(1);
	}

	if (!pid)
		printf("Process %d says: I'm child\n",
					getpid());
	else
		printf("Process %d says: I'm parent of %d\n",
					getpid(), pid);

	return 0;
}
