#ifndef TASK_H
#define TASK_H

#include "signal.h"
#include "trap_frame.h"
#include <stdint.h>

enum task_state
{
    TASK_RUNNABLE = 0, /* ready to be scheduled */
    TASK_RUNNING,      /* currently executing on the CPU */
    TASK_BLOCKED,      /* waiting on an event (I/O, sleep, lock) */
    TASK_ZOMBIE, /* terminated, waiting for parent to collect exit status */
};

/*
 * Architecture-specific CPU state for a process.
 * Each process has exactly one execution context — isolated here so arch
 * extensions (FPU, segments) can be added without touching struct task.
 */
struct task_thread
{
    struct trap_frame
        *tf;      /* register snapshot saved at interrupt or context switch */
    uint32_t esp; /* kernel stack pointer at the moment of the switch */
};

/*
 * task descriptor — represents a process.
 *
 * Planned extensions: mm_struct for userland memory (paging, ring3), children
 * and sibling pointers for a full process tree, and an exit_code collected by
 * the parent on TASK_ZOMBIE.
 */
struct task
{
    enum task_state state; /* current lifecycle state of this task */
    struct task_thread
        thread; /* arch CPU state, saved and restored on switch */

    uint32_t pid;        /* unique process identifier */
    struct task *parent; /* direct pointer to parent task (NULL for init) */

    void *kstack; /* base address of this task's kernel stack */

    struct task *next; /* intrusive singly-linked list for the scheduler,
                        * will become a list_head when needed */

    sigset_t pending_signals;            /* bitmask of undelivered signals */
    sig_handler_t signal_handlers[NSIG]; /* per-signal handler */
};

#endif /* TASK_H */
