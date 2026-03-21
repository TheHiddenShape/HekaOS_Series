#include "kpanic.h"
#include "interrupts.h"
#include "io.h"

static void
panic_clear_screen (volatile uint16_t *vga)
{
    int i;
    uint16_t blank = (uint16_t)' ' | ((uint16_t)VGA_PANIC_COLOR << 8);

    for (i = 0; i < VGA_PANIC_COLS * VGA_PANIC_ROWS; i++)
    {
        vga[i] = blank;
    }
}

static void
panic_print (volatile uint16_t *vga, int pos, const char *str)
{
    while (*str && pos < VGA_PANIC_COLS * VGA_PANIC_ROWS)
    {
        vga[pos++] = (uint16_t)*str++ | ((uint16_t)VGA_PANIC_COLOR << 8);
    }
}

static void
panic_disable_cursor (void)
{
    outb (0x3D4, 0x0A);
    outb (0x3D5, 0x20);
}

void
kpanic (const char *msg)
{
    volatile uint16_t *vga = (volatile uint16_t *)VGA_PANIC_ADDR;

    disable_interrupts ();
    panic_disable_cursor ();
    panic_clear_screen (vga);
    panic_print (vga, 0, "KERNEL PANIC: ");
    panic_print (vga, 14, msg);
    panic_print (vga, VGA_PANIC_COLS, "System halted.");

    disable_interrupts_and_halt ();
}
