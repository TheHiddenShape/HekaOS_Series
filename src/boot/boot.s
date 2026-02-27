.global stack_bottom
.global stack_top
.global _start

# declare constants for the multiboot header.
.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# stack memory layout:
# stack_top <- %esp (initial stack pointer, stack grows downwards)
# ... 16 KiB of stack space ...
# stack_bottom (bottom of stack)

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.type _start, @function
_start:
	mov $stack_top, %esp
	call kernel_main
	cli	# disable interrupts FLAG IF = 0
1:	hlt
	jmp 1b # without the JMP, the CPU would continue to execute the following in memory → undefined behavior / crash.

.size _start, . - _start