#ifndef VMALLOC_H
#define VMALLOC_H

#include <stdint.h>

/* vmalloc zone: above kmalloc heap (0xEFFFFFFF), below recursive-PT window */
#define VMALLOC_BASE 0xF0000000
#define VMALLOC_MAX 0xFFBFFFFF

void vmalloc_init (void);
void *vmalloc (uint32_t npages);
void vfree (void *addr);
void *vbrk (void);
uint32_t vsize (void);

typedef struct
{
    uint32_t used_pages;
    uint32_t total_pages;
    uint32_t region_count;
} vmalloc_stats_t;

void vmalloc_test (void);
void vmalloc_query (vmalloc_stats_t *stats);

#endif
