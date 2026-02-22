.global enable_interrupts
.global disable_interrupts
.global halt_cpu
.global disable_interrupts_and_halt
.global get_esp

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

halt_cpu:
    hlt
    ret

disable_interrupts_and_halt:
    cli
    hlt
    jmp disable_interrupts_and_halt

get_esp:
    mov %esp, %eax
    ret
