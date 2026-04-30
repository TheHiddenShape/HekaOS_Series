#include "kmalloc.h"
#include "kmem_dyn_alloc.h"

void
heap_init (void)
{
    kmem_dyn_alloc_init ();
}

void *
kmalloc (size_t size)
{
    return kmem_alloc (size);
}

void
kfree (void *addr)
{
    kmem_free (addr);
}

void *
kbrk (uint32_t nbytes)
{
    return kmem_brk (nbytes);
}

size_t
ksize (void *addr)
{
    return kmem_size (addr);
}

void
kmalloc_test (void)
{
    kmem_dyn_alloc_test ();
}

void
kmalloc_query (kmalloc_stats_t *s)
{
    kmem_dyn_alloc_query (s);
}
