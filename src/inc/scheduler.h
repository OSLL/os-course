#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

void disable_preempt(void);
void enable_preempt(void);
int preempt_enabled(void);

#endif /* __SCHEDULER_H__ */
