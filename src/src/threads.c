#include <stdint.h>

#include <threads.h>
#include <scheduler.h>
#include <alloc.h>
#include <debug.h>


struct thread_switch_frame {
	uint64_t rflags;
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t rbx;
	uint64_t rbp;
	uint64_t rip;
} __attribute__((packed));


static const size_t STACK_ORDER = 1;

static struct mem_cache thread_cache;
static struct thread *current;


static struct thread *thread_alloc(void)
{
	return mem_cache_alloc(&thread_cache);
}

static void thread_free(struct thread *thread)
{
	mem_cache_free(&thread_cache, thread);
}

void threads_setup(void)
{
	const size_t size = sizeof(struct thread);
	const size_t align = 64;

	mem_cache_setup(&thread_cache, size, align);
	current = thread_alloc();
	BUG_ON(!current);
}

void thread_entry(void (*fptr)(void *), void *arg)
{
	enable_ints();
	fptr(arg);
	thread_exit();
}

struct thread *__thread_create(void (*fptr)(void *), void *arg, int order)
{
	void __thread_entry(void);

	const size_t stack_size = (size_t)1 << (PAGE_SHIFT + order);
	struct page *stack = __page_alloc(order);

	if (!stack)
		return 0;

	struct thread *thread = thread_alloc();
	struct thread_switch_frame *frame;

	thread->stack = stack;
	thread->stack_order = order;
	thread->stack_addr = (uintptr_t)va(page_addr(stack));
	thread->stack_ptr = thread->stack_addr + stack_size - sizeof(*frame);
	thread->state = THREAD_BLOCKED;

	frame = (struct thread_switch_frame *)thread->stack_ptr;
	frame->r12 = (uintptr_t)fptr;
	frame->r13 = (uintptr_t)arg;
	frame->rbp = 0;
	frame->rflags = (1ul << 2);
	frame->rip = (uintptr_t)__thread_entry;
	return thread;
}

struct thread *thread_create(void (*fptr)(void *), void *arg)
{
	return __thread_create(fptr, arg, STACK_ORDER);
}

void thread_destroy(struct thread *thread)
{
	__page_free(thread->stack, thread->stack_order);
	thread_free(thread);
}

struct thread *thread_current(void)
{ return current; }

void thread_activate(struct thread *thread)
{
	thread->state = THREAD_ACTIVE;
	scheduler_thread_activate(thread);
}

void thread_exit(void)
{
	thread_current()->state = THREAD_FINISHING;
	while (1)
		schedule();
}

void thread_join(struct thread *thread)
{
	while (thread->state != THREAD_FINISHED)
		schedule();
}

void thread_switch_to(struct thread *next)
{
	void __thread_switch(uintptr_t *prev_state, uintptr_t next_state);

	struct thread *prev = thread_current();

	/* This is only required because we don't set current in the
	 * thread_entry. */
	current = next;
	__thread_switch(&prev->stack_ptr, next->stack_ptr);
	current = prev;
}
