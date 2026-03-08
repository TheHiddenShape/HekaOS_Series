#include "interrupts.h"
#include "io.h"
#include "paging.h"
#include "pic.h"
#include "printk.h"
#include <stdint.h>

extern void shell_add_char (char c);

static const char *exception_names[] = {
    [13] = "General Protection Fault",
    [14] = "Page Fault",
};

void
isr_handler (uint32_t int_no, uint32_t err_code)
{
    const char *name = "Unknown Exception";
    if (int_no < sizeof (exception_names) / sizeof (exception_names[0])
        && exception_names[int_no])
    {
        name = exception_names[int_no];
    }

    pr_err ("EXCEPTION: %s (int %d, err=0x%x)\n", name, int_no, err_code);

    if (int_no == 14)
    {
        pr_err ("  CR2 (faulting address)=0x%x\n", read_cr2 ());
    }

    disable_interrupts_and_halt ();
}

// scancode to ASCII table (US QWERTY, lowercase)
static const char scancode_to_ascii[128]
    = { 0,   27,   '1',  '2', '3',  '4', '5', '6', '7', '8', '9', '0', '-',
        '=', '\b', '\t', 'q', 'w',  'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
        '[', ']',  '\n', 0,   'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
        ';', '\'', '`',  0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
        '.', '/',  0,    '*', 0,    ' ', 0,   0,   0,   0,   0,   0,   0,
        0,   0,    0,    0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,    0,    0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,    0,    0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,    0,    0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,    0,    0,   0,    0,   0,   0,   0,   0 };

void
irq_handler (void)
{
    uint8_t scancode = inb (0x60);

    // ignore key release (bit 7 set)
    if (!(scancode & 0x80))
    {
        char c = scancode_to_ascii[scancode];
        if (c)
        {
            shell_add_char (c);
        }
    }

    // send End of Interrupt to PIC
    pic_send_eoi (1); // IRQ 1 = keyboard
}
