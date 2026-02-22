ENTRY(_start)

SECTIONS
{

	/*. -> correspond to the location counter, it's like a cursor */

	. = 0x00000800;
    .gdt :
    {
        *(.gdt)
    }

	/* Kernel at 2M, to avoid overlaps with low memory */
	. = 2M;

	/* 	BLOCK(4K): Synonym for ALIGN(4K) in this context. Ensures that the section starts at an address that is a multiple of 4K.
		ALIGN(4K): Identical. Both exist for historical compatibility. You can use either one. */

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		/* Collects "common" symbols uninitialized global variables declared without an extern in multiple files. In C */
		*(COMMON)
		*(.bss)
	}
}
