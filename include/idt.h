#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/*
https://wiki.osdev.org/Interrupt_Descriptor_Table
Intel SDM Vol. 3A 6.11 "IDT Descriptors"
*/

struct idt_entry
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t base_high;
} __attribute__ ((packed));

struct idt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__ ((packed));

// CPU exceptions
extern void isr13 (void); // general Protection Fault
extern void isr14 (void); // page Fault

// hardware interrupts (IRQs)
extern void irq1 (void);  // keyboard (IRQ 1)

void idt_init (void);
void idt_set_gate (uint8_t num, uint32_t base, uint16_t selector,
                   uint8_t flags);

#endif
