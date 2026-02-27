.macro ISR_NOERRCODE num
.global isr\num
isr\num:
    cli
    push $0
    push $\num
    jmp isr_common_stub
.endm

.macro ISR_ERRCODE num
.global isr\num
isr\num:
    cli
    push $\num
    jmp isr_common_stub
.endm

ISR_ERRCODE   13
ISR_ERRCODE   14

.macro IRQ num, irq_num
.global irq\num
irq\num:
    cli
    push $0
    push $\irq_num
    jmp irq_common_stub
.endm

IRQ 1, 33

isr_common_stub:
    pusha

    mov %ds, %ax
    push %eax

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    # int_no at 36(%esp), err_code at 40(%esp)
    push 40(%esp)      # err_code
    push 40(%esp)      # int_no (offset shifted by previous push)
    call isr_handler
    add $8, %esp

    pop %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    popa
    add $8, %esp
    iret

irq_common_stub:
    pusha

    mov %ds, %ax
    push %eax

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    call irq_handler

    pop %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    popa
    add $8, %esp
    iret
