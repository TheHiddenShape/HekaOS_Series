.global gdt_flush
.extern gp

gdt_flush:
    mov 4(%esp), %eax
    lgdt (%eax)

# segment register purpose reminder:
#
# CS: fetch instructions
# DS: data memory access
# SS: stack access
# FS/GS: explicit override

    mov $0x10, %ax # 0x10 is the offset in the GDT to our data segment
    mov %ax, %ds # load Data Segment
    mov %ax, %es # load Extra Segment
    mov %ax, %fs # load Additional segment 1 (free use)
    mov %ax, %gs # load Additional segment 2 (free use)
    mov %ax, %ss # load Stack segment kernel space, todo: in user space: 0x30

    # 0x08 is the offset to our code segment: Far jump!
    # load simultaneously cs = 0x08 and eip = addr flush2
    ljmp $0x08, $flush2

flush2:
    ret