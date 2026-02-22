#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

void enable_interrupts (void);
void disable_interrupts (void);
void halt_cpu (void);
void disable_interrupts_and_halt (void);
uint32_t get_esp (void);

#endif
