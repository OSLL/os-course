	.text
	.global main
main:
	/* write to stdout */
	movq $1, %rax		// rax - syscall number
	movq $1, %rdi		// rdi - file descriptor
	movq $msg_begin, %rsi	// rsi - string pointer
	movq msg_len, %rdx	// rdx - size
	syscall

	/* exit */
	movq $60, %rax
	movq $0, %rdi
	syscall

	.data
msg_begin:
	.ascii "Hello, World\n"
msg_len:
	.quad msg_len - msg_begin
