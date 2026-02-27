#include "../include/idt.h"

#define IDT_ENTRIES 256

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idp;

extern void idt_load (uint32_t);

void
idt_set_gate (uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;

    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
}

void
idt_init (void)
{
    idp.limit = (sizeof (struct idt_entry) * IDT_ENTRIES) - 1;
    idp.base = (uint32_t)&idt;

    for (int i = 0; i < IDT_ENTRIES; i++)
    {
        idt_set_gate (i, 0, 0, 0);
    }

    idt_set_gate (13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate (14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate (33, (uint32_t)irq1, 0x08, 0x8E);

    idt_load ((uint32_t)&idp);
}
