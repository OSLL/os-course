#ifndef __THREADS_H__
#define __THREADS_H__

enum thread_state {
	THREAD_ACTIVE,
	THREAD_BLOCKED,
	THREAD_FINISHING,
	THREAD_FINISHED,
};

struct thread {
	enum thread_state state;
	unsigned long long time;
	struct page *stack;
	uintptr_t stack_addr;
	int stack_order;
	uintptr_t stack_ptr;
};

struct thread *__thread_create(void (*fptr)(void *), void *arg, int order);
struct thread *thread_create(void (*fptr)(void *), void *arg);
void thread_destroy(struct thread *thread);

struct thread *thread_current(void);
void thread_activate(struct thread *thread);
void thread_join(struct thread *thread);
void thread_exit(void);

void thread_switch_to(struct thread *next);

void threads_setup(void);

static inline void schedule(void) {}

#endif /*__THREADS_H__*/
