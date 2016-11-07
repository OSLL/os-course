#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main()
{
	const pid_t pid = fork();

	if (pid < 0) {
		perror("fork failed");
		exit(1);
	}

	srand(time(0));
	if (!pid) {
		const int rc = rand() & 0xff;

		printf("Process %d says: return %d\n",
					getpid(), rc);
		exit(rc);
	} else {
		int status;

		waitpid(pid, &status, 0);
		printf("Process %d says: my child's died with value %d\n",
					getpid(), WEXITSTATUS(status));
	}

	return 0;
}
