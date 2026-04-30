#ifndef KMALLOC_H
#define KMALLOC_H

#include "kmem_dyn_alloc.h"
#include <stddef.h>
#include <stdint.h>

/* thin interface kept for existing callers — implementation delegates to
 * kmem_dyn_alloc. will be replaced by the slab allocator in phase 3. */

/* KHEAP_VIRT_BASE / KHEAP_VIRT_MAX are defined in kmem_dyn_alloc.h */

void heap_init (void);
void *kmalloc (size_t size);
void kfree (void *addr);
void *kbrk (uint32_t nbytes);
size_t ksize (void *addr);

typedef kmem_stats_t kmalloc_stats_t;

void kmalloc_test (void);
void kmalloc_query (kmalloc_stats_t *stats);

#endif
