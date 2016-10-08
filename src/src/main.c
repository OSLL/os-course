#include <serial.h>
#include <ints.h>

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
	ints_setup();

	__asm__ volatile ("int $0");

	while (1);
}
