#include "kmem_dyn_alloc.h"
#include "klib.h"
#include "kpanic.h"
#include "paging.h"
#include "printk.h"
#include <stddef.h>
#include <stdint.h>

#define ALIGN8(sz) (((sz) + 7) & ~(uint32_t)7)
#define BLOCK_MIN (sizeof (kmem_block_t) + 8)

typedef struct kmem_block
{
    uint32_t size;
    uint32_t used;
    struct kmem_block *next;
    struct kmem_block *prev;
} kmem_block_t;

static kmem_block_t *kmem_heap_head = NULL;
static uint8_t *kmem_heap_end = (uint8_t *)KHEAP_VIRT_BASE;

static void
kmem_heap_grow (uint32_t nbytes)
{
    uint32_t pages = (nbytes + (uint32_t)sizeof (kmem_block_t) + PAGE_SIZE - 1)
                     / PAGE_SIZE;
    if (!pages)
    {
        pages = 1;
    }

    uint32_t total = pages * PAGE_SIZE;

    if ((uint32_t)kmem_heap_end + total > KHEAP_VIRT_MAX)
    {
        kpanic ("kmem_dyn_alloc: kernel heap limit reached");
    }

    for (uint32_t i = 0; i < pages; i++)
    {
        void *vaddr = (void *)(kmem_heap_end + i * PAGE_SIZE);
        if (!alloc_page (vaddr, PAGE_PRESENT | PAGE_RW))
        {
            kpanic ("kmem_dyn_alloc: alloc_page failed in heap_grow");
        }
    }

    kmem_block_t *last = NULL;
    kmem_block_t *cur = kmem_heap_head;
    while (cur)
    {
        last = cur;
        cur = cur->next;
    }

    if (last && !last->used
        && ((uint8_t *)last + sizeof (kmem_block_t) + last->size
            == kmem_heap_end))
    {
        last->size += total;
    }
    else
    {
        kmem_block_t *blk = (kmem_block_t *)kmem_heap_end;
        blk->size = total - (uint32_t)sizeof (kmem_block_t);
        blk->used = 0;
        blk->next = NULL;
        blk->prev = last;
        if (last)
        {
            last->next = blk;
        }
        else
        {
            kmem_heap_head = blk;
        }
    }

    kmem_heap_end += total;
}

void
kmem_dyn_alloc_init (void)
{
    kmem_heap_head = NULL;
    kmem_heap_end = (uint8_t *)KHEAP_VIRT_BASE;
    kmem_heap_grow (0);
    pr_info ("kmem_dyn_alloc initialized: base=0x%x end=0x%x\n",
             KHEAP_VIRT_BASE, (uint32_t)kmem_heap_end);
    printk ("\n");
}

void *
kmem_brk (uint32_t nbytes)
{
    if (!nbytes)
    {
        return kmem_heap_end;
    }
    kmem_heap_grow (nbytes);
    return kmem_heap_end;
}

void *
kmem_alloc (size_t size)
{
    if (!size)
    {
        return NULL;
    }

    size = ALIGN8 (size);

    kmem_block_t *blk = kmem_heap_head;
    while (blk)
    {
        if (!blk->used && blk->size >= size)
        {
            break;
        }
        blk = blk->next;
    }

    if (!blk)
    {
        kmem_heap_grow (size);
        blk = kmem_heap_head;
        while (blk)
        {
            if (!blk->used && blk->size >= size)
            {
                break;
            }
            blk = blk->next;
        }
        if (!blk)
        {
            kpanic ("kmem_dyn_alloc: out of memory");
        }
    }

    if (blk->size >= size + BLOCK_MIN)
    {
        kmem_block_t *split
            = (kmem_block_t *)((uint8_t *)blk + sizeof (kmem_block_t) + size);
        split->size = blk->size - size - (uint32_t)sizeof (kmem_block_t);
        split->used = 0;
        split->next = blk->next;
        split->prev = blk;
        if (blk->next)
        {
            blk->next->prev = split;
        }
        blk->next = split;
        blk->size = size;
    }

    blk->used = 1;
    return (uint8_t *)blk + sizeof (kmem_block_t);
}

void
kmem_free (void *addr)
{
    if (!addr)
    {
        return;
    }

    kmem_block_t *blk
        = (kmem_block_t *)((uint8_t *)addr - sizeof (kmem_block_t));

    if (!blk->used)
    {
        kpanic ("kmem_free: double free detected");
    }

    blk->used = 0;

    if (blk->next && !blk->next->used)
    {
        kmem_block_t *nxt = blk->next;
        blk->size += (uint32_t)sizeof (kmem_block_t) + nxt->size;
        blk->next = nxt->next;
        if (nxt->next)
        {
            nxt->next->prev = blk;
        }
    }

    if (blk->prev && !blk->prev->used)
    {
        kmem_block_t *prv = blk->prev;
        prv->size += (uint32_t)sizeof (kmem_block_t) + blk->size;
        prv->next = blk->next;
        if (blk->next)
        {
            blk->next->prev = prv;
        }
    }
}

size_t
kmem_size (void *addr)
{
    if (!addr)
    {
        return 0;
    }
    return ((kmem_block_t *)((uint8_t *)addr - sizeof (kmem_block_t)))->size;
}

void
kmem_dyn_alloc_test (void)
{
    pr_info ("#### kmem_dyn_alloc test ####\n");

    /* 1. basic alloc + write/read */
    {
        uint32_t *p = kmem_alloc (sizeof (uint32_t));
        if (!p)
        {
            kpanic ("kmem_dyn_alloc test: basic alloc returned NULL");
        }
        *p = 0xDEADBEEF;
        if (*p != 0xDEADBEEF)
        {
            kpanic ("kmem_dyn_alloc test: write/read failed");
        }
        pr_info ("kmem_dyn: alloc+write at 0x%x\n", (uint32_t)p);
        kmem_free (p);
    }

    /* 2. kmem_size reports at least the requested size */
    {
        void *p = kmem_alloc (100);
        if (!p)
        {
            kpanic ("kmem_dyn_alloc test: kmem_size alloc returned NULL");
        }
        if (kmem_size (p) < 100)
        {
            kpanic ("kmem_dyn_alloc test: kmem_size too small");
        }
        pr_info ("kmem_dyn: kmem_size = %u (requested 100)\n",
                 (uint32_t)kmem_size (p));
        kmem_free (p);
    }

    /* 3. multiple allocs return unique, non-overlapping pointers */
    {
        void *p1 = kmem_alloc (32);
        void *p2 = kmem_alloc (64);
        void *p3 = kmem_alloc (128);
        if (!p1 || !p2 || !p3)
        {
            kpanic ("kmem_dyn_alloc test: multi-alloc returned NULL");
        }
        if (p1 == p2 || p2 == p3 || p1 == p3)
        {
            kpanic ("kmem_dyn_alloc test: duplicate pointers");
        }
        pr_info ("kmem_dyn: 3 unique allocs p1=0x%x p2=0x%x p3=0x%x\n",
                 (uint32_t)p1, (uint32_t)p2, (uint32_t)p3);

        /* 4. kmem_free + reuse: p4 must fall within the heap range */
        kmem_free (p1);
        void *p4 = kmem_alloc (16);
        if (!p4)
        {
            kpanic ("kmem_dyn_alloc test: realloc after free returned NULL");
        }
        if ((uint32_t)p4 < KHEAP_VIRT_BASE || (uint32_t)p4 > KHEAP_VIRT_MAX)
        {
            kpanic ("kmem_dyn_alloc test: reallocated pointer outside heap");
        }
        pr_info ("kmem_dyn: realloc after free at 0x%x\n", (uint32_t)p4);

        /* 5. kmem_free(NULL) is a no-op */
        kmem_free (NULL);
        pr_info ("kmem_dyn: kmem_free(NULL) is no-op\n");

        /* 6. coalesce + large alloc: after freeing everything, a single
         * contiguous block should satisfy a larger request */
        kmem_free (p4);
        kmem_free (p2);
        kmem_free (p3);
        void *big = kmem_alloc (512);
        if (!big)
        {
            kpanic (
                "kmem_dyn_alloc test: large alloc after full coalesce failed");
        }
        pr_info ("kmem_dyn: 512-byte alloc after coalesce at 0x%x\n",
                 (uint32_t)big);
        kmem_free (big);
    }

    /* 7. kmem_brk extends the heap */
    {
        void *end_before = kmem_brk (0);
        kmem_brk (PAGE_SIZE);
        void *end_after = kmem_brk (0);
        if ((uint32_t)end_after <= (uint32_t)end_before)
        {
            kpanic ("kmem_dyn_alloc test: kmem_brk did not extend heap");
        }
        pr_info ("kmem_dyn: kmem_brk extended heap 0x%x -> 0x%x\n",
                 (uint32_t)end_before, (uint32_t)end_after);
    }

    /* 8. alignment: all returned pointers are 8-byte aligned */
    {
        void *p1 = kmem_alloc (1);
        void *p2 = kmem_alloc (3);
        void *p3 = kmem_alloc (13);
        if (!p1 || !p2 || !p3)
        {
            kpanic ("kmem_dyn_alloc test: alignment alloc returned NULL");
        }
        if ((uint32_t)p1 % 8 || (uint32_t)p2 % 8 || (uint32_t)p3 % 8)
        {
            kpanic ("kmem_dyn_alloc test: pointer not 8-byte aligned");
        }
        pr_info ("kmem_dyn: alignment ok p1=0x%x p2=0x%x p3=0x%x\n",
                 (uint32_t)p1, (uint32_t)p2, (uint32_t)p3);
        kmem_free (p1);
        kmem_free (p2);
        kmem_free (p3);
    }

    pr_info ("kmem_dyn_alloc test passed\n\n");
}

void
kmem_dyn_alloc_query (kmem_stats_t *s)
{
    memset (s, 0, sizeof (*s));
    s->heap_end = (uint32_t)kmem_heap_end;

    kmem_block_t *blk = kmem_heap_head;
    while (blk)
    {
        s->total_blocks++;
        if (blk->used)
        {
            s->used_blocks++;
            s->used_bytes += blk->size;
        }
        else
        {
            s->free_blocks++;
            s->free_bytes += blk->size;
        }
        blk = blk->next;
    }
}
