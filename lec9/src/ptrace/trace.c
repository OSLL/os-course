#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

static int ptrace_get_regs(pid_t pid, struct user_regs_struct *regs)
{
	static const size_t word_size = sizeof(unsigned long);
	static const size_t data_size = sizeof(*regs);

	unsigned long *ptr = (unsigned long *)regs;

	for (size_t word = 0; word != sizeof(*regs)/word_size; ++word) {
		errno = 0; /* thanks to stupid glibc wrapper */
		ptr[word] = ptrace(PTRACE_PEEKUSER, pid, word * word_size, 0);
		if (errno) {
			perror("ptrace PEEKUSER failed");
			return -1;
		}
	}
	return 0;
}

static void ptrace_dump_x86_64_regs(const struct user_regs_struct *regs)
{
	#define FIELD(x)	((unsigned long)(regs->x))
	printf("pc = %lx:%lx, sp = %lx:%lx, flags = %lx, es = %lx, ds = %lx\n"
		"fs = %lx [base = %lx], gs = %lx [base = %lx],\n"
		"rax = %lx, rbx = %lx, rcx = %lx, rdx = %lx,\n"
		"rsi = %lx, rdi = %lx, rbp = %lx,\n"
		"r8 = %lx, r9 = %lx, r10 = %lx, r11 = %lx,\n"
		"r12 = %lx, r13 = %lx, r14 = %lx, r15 = %lx\n",
		FIELD(cs), FIELD(rip), FIELD(ss), FIELD(rsp),
		FIELD(eflags), FIELD(es), FIELD(ds),
		FIELD(fs), FIELD(fs_base), FIELD(gs), FIELD(gs_base),
		FIELD(rax), FIELD(rbx), FIELD(rcx), FIELD(rdx),
		FIELD(rsi), FIELD(rdi), FIELD(rbp),
		FIELD(r8), FIELD(r9), FIELD(r10), FIELD(r11),
		FIELD(r12), FIELD(r13), FIELD(r14), FIELD(r15));
	#undef FIELD
}

int main(int argc, char **argv)
{
	const pid_t pid = fork();

	if (pid < 0) {
		perror("fork failed");
		exit(1);
	}

	if (!pid) {
		char **args = calloc(argc, sizeof(char *));

		for (int i = 0; i != argc - 1; ++i)
			args[i] = strdup(argv[i + 1]);
		args[argc - 1] = 0;

		ptrace(PTRACE_TRACEME, 0, 0, 0);
		if (execvp(args[0], args) < 0)
			exit(1);
		return 0;
	}

	const int syscall = SIGTRAP | 0x80;
	int status;

	waitpid(pid, &status, 0);
	ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
	while (!WIFEXITED(status)) {
		struct user_regs_struct regs;

		ptrace(PTRACE_SYSCALL, pid, 0, 0);
		waitpid(pid, &status, 0);

		if (!WIFSTOPPED(status) || WSTOPSIG(status) != syscall)
			continue;

		if (ptrace_get_regs(pid, &regs) < 0) {
			kill(pid, SIGKILL);
			waitpid(pid, &status, 0);
			exit(1);
		}
		printf("syscall regs:\n");
		ptrace_dump_x86_64_regs(&regs);

		ptrace(PTRACE_SYSCALL, pid, 0, 0);
		waitpid(pid, &status, 0);
	}

	return 0;
}
