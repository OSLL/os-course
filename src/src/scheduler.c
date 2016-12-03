#include <scheduler.h>
#include <stdatomic.h>


/* We actually don't need atomic here until our compiler generates code
 * that might partialy write ints, which would mean that the compiler
 * is completely mad. But to make everything fair-and-square we still
 * use atomic. */
static atomic_int preempt_cnt;

void disable_preempt(void)
{
	atomic_fetch_add_explicit(&preempt_cnt, 1, memory_order_relaxed);
}

void enable_preempt(void)
{
	atomic_fetch_sub_explicit(&preempt_cnt, 1, memory_order_relaxed);
}

int preempt_enabled(void)
{
	return atomic_load_explicit(&preempt_cnt, memory_order_relaxed) != 0;
}
