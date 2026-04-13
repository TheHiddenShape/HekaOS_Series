.global trigger_trap_test
trigger_trap_test:
    mov $0xAAAAAAAA, %eax
    mov $0xBBBBBBBB, %ebx
    mov $0xCCCCCCCC, %ecx
    mov $0xDDDDDDDD, %edx
    mov $0x11111111, %esi
    mov $0x22222222, %edi
    int $0x42
    ret

.global panic_clear_registers
panic_clear_registers:
    xor %eax, %eax
    xor %ebx, %ebx
    xor %ecx, %ecx
    xor %edx, %edx
    xor %esi, %esi
    xor %edi, %edi
    xor %ebp, %ebp
    ret
