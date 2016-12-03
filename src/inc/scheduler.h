#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

struct thread;

void disable_preempt(void);
void enable_preempt(void);
int preempt_enabled(void);

static inline void scheduler_thread_activate(struct thread *thread)
{ (void)thread; }

#endif /* __SCHEDULER_H__ */
