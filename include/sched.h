#ifndef SCHED_H
#define SCHED_H

#include "task.h"

/* one tick = 1000 / PIT_TICK_HZ ms (currently 10 ms); 50 ticks ≈ 500 ms,
 * deliberately slow so a human can observe scheduling transitions */
#define PROC_QUANTUM 50

/* set by schedule() when a context switch is requested; consumed by
 * irq_common_stub on IRQ exit. */
extern volatile uint32_t need_resched;
extern struct task *next_task;

struct task *pick_next_task (void);

void schedule (void);

#endif
