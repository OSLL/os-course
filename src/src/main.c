#include <serial.h>

static void qemu_gdb_hang(void)
{
#ifdef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}

void main(void)
{
	qemu_gdb_hang();

	serial_setup();
	serial_write("Hello, World!\n", sizeof("Hello, World!\n"));

	while (1);
}
