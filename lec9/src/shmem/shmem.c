#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	const char *mem_name = "shared_mem_name";
	const int mem_size = 4096;
	const int fd = shm_open(mem_name, O_CREAT | O_RDWR,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

	if (fd < 0) {
		perror("shm_open failed");
		exit(1);
	}

	if (ftruncate(fd, mem_size) == -1) {
		perror("ftruncate failed");
		exit(1);
	}

	void *ptr = mmap(/* addr hint = */0,
			/* size =  */mem_size,
			/* protection = */PROT_READ | PROT_WRITE,
			/* flags (private/shared) = */MAP_SHARED,
			/* file descriptor */fd,
			/* offset = */0);
	if (ptr == MAP_FAILED) {
		perror("mmap failed");
		exit(1);
	}

	int c;

	while ((c = getopt(argc, argv, "p:g")) != -1) {
		switch (c) {
		case 'p':
			strcpy(ptr, optarg);
			break;
		case 'g':
			puts(ptr);
			break;
		}
	}

	munmap(ptr, mem_size);
	return 0;
}
