#include <serial.h>
#include <memory.h>
#include <balloc.h>
#include <paging.h>
#include <alloc.h>
#include <ints.h>
#include <time.h>

static void qemu_gdb_hang(void)
{
#ifdef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}

void main(void *bootstrap_info)
{
	qemu_gdb_hang();

	serial_setup();
	ints_setup();
	time_setup();
	balloc_setup(bootstrap_info);
	paging_setup();
	page_alloc_setup();
	mem_alloc_setup();
	enable_ints();

	while (1);
}
