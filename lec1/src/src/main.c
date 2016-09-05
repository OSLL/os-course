static void qemu_gdb_hang(void)
{
#ifndef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}

void main(void)
{
	qemu_gdb_hang();
	while (1);
}
