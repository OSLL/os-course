#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	char buf[4096];
	int size;
	int fd[2];

	if (pipe(fd) < 0) {
		perror("pipe failed");
		exit(1);
	}

	const pid_t pid = fork();

	if (pid < 0) {
		perror("fork failed");
		close(fd[0]);
		close(fd[1]);
		exit(1);
	}

	if (pid) {
		while ((size = read(0, buf, sizeof(buf) - 1)) > 0) {
			buf[size] = 0;
			printf("Send: %s\n", buf);
			write(fd[1], buf, size + 1);
		}
	} else {
		while ((size = read(fd[0], buf, sizeof(buf) - 1)) > 0)
			printf("Received: %s\n", buf);
	}

	close(fd[0]);
	close(fd[1]);
	return 0;
}
