#ifndef PHYS_PAGE_FRAME_H
#define PHYS_PAGE_FRAME_H

#include <stdint.h>

/* bitmap-based frame allocator. https://wiki.osdev.org/Page_Frame_Allocation */

void phys_mem_init (void);
void *phys_alloc_frame (void);
void phys_free_frame (void *frame);
uint32_t phys_free_count (void);
void phys_mem_test (void);

#endif
