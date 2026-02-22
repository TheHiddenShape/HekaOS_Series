#include "../include/interrupts.h"
#include "../include/io.h"
#include "../include/pic.h"
#include <stdint.h>

extern void shell_add_char (char c);

const char *exception_messages[] = { "General Protection Fault",
};

void
isr_handler (void)
{
    // for now, just halt the system
    // todo: Later add proper exception handling
    disable_interrupts_and_halt ();
}

// Scancode to ASCII table (US QWERTY, lowercase)
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
