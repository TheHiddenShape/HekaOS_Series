.global load_page_directory
.global enable_paging
.global read_cr0
.global read_cr2
.global read_cr3

# CR3 (page directory base register): holds the physical address of the current page directory
# changing CR3 switches the entire address space, each process has its own pg

load_page_directory:
    push %ebp
    mov %esp, %ebp
    mov 8(%esp), %eax
    mov %eax, %cr3
    mov %ebp, %esp
    pop %ebp
    ret

# CR0: controls CPU modes, bit 0 (PE) enables protected mode, bit 31 (PG) enables paging

enable_paging:
    push %ebp
    mov %esp, %ebp
    mov %cr0, %eax
    or $0x80000001, %eax
    mov %eax, %cr0
    mov %ebp, %esp
    pop %ebp
    ret

read_cr0:
    mov %cr0, %eax
    ret

read_cr2:
    mov %cr2, %eax
    ret

read_cr3:
    mov %cr3, %eax
    ret
