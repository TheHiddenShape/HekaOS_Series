#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

/*
https://wiki.osdev.org/Paging
https://wiki.osdev.org/Setting_Up_Paging
*/

void paging_init (void);

void load_page_directory (uint32_t *page_directory);
void enable_paging (void);
uint32_t read_cr0 (void);
uint32_t read_cr2 (void);
uint32_t read_cr3 (void);

void paging_test (void);

#endif
