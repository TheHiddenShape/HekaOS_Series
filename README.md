# HekaOS

An x86 monolithic hybrid kernel written from scratch in **C** and **Rust**.

This is a series leading to v1.0.0, the first stable release of a fully functional OS. Design decisions and architectural choices are documented on my personal [blog](https://ammons-organization-1.gitbook.io/thehiddenshape/system-and-networks/kfs-kernel-from-scratch-series).

## Core Features

### CPU & Interrupts
- **GDT (Global Descriptor Table)** - 7-entry segmentation setup with kernel (ring 0) and user (ring 3) code/data/stack segments
- **IDT (Interrupt Descriptor Table)** - 256-entry table handling CPU exceptions (GPF, page fault) and hardware interrupts
- **PIC (8259)** - remapped IRQ 0-15 to INT 32-47
- **ISR handlers** - exception handling with CR2 reporting for page faults

### Memory Management
- **PMM (Physical Memory Manager)** - bitmap-based frame allocator managing 64 MiB of physical RAM (4 KiB frames starting at 4 MiB)
- **Paging (VMM)** - 32-bit protected mode paging with identity-mapped first 4 MiB, recursive page directory (PD[1023]), 3GB/1GB kernel/user split
- **kmalloc / kfree** - kernel heap allocator (`0xC0000000`–`0xEFFFFFFF`) with first-fit strategy, 8-byte alignment, forward/backward coalescing, and auto-growing pages. Includes `kbrk`, `ksize`, `kmalloc_query`
- **vmalloc / vfree** - virtual page allocator (`0xF0000000`–`0xFFBFFFFF`) for large virtually contiguous allocations across physically fragmented pages. Includes `vbrk`, `vsize`, `vmalloc_query`
- **Kernel panic** - halts CPU with interrupts disabled on unrecoverable errors

### Drivers & I/O
- **VGA text mode** - 80x25 terminal with scrolling, cursor tracking, color palette, and a themed status bar
- **Keyboard driver** - PS/2 keyboard input via IRQ1 with US QWERTY scancode-to-ASCII conversion
- **I/O ports** - `inb`/`outb`/`outw`/`io_wait` primitives

### Kernel Utilities
- **printk** - printf-style kernel logging (`%s`, `%d`, `%u`, `%x`, `%p`, `%c`) with log levels (emerg → debug) and 4 KiB circular ring buffer
- **memset / strlen / strcmp** - basic klib string and memory utilities
- **Kernel stack info** - runtime stack layout and usage reporting

### Shell
- **hekashell** - interactive command-line interface with the following built-in commands:

| Command    | Description                                         |
|------------|-----------------------------------------------------|
| `help`     | Show available commands                             |
| `dmesg`    | Display the kernel ring buffer                      |
| `memdump`  | Show memory usage (heap/vmalloc stats)              |
| `reboot`   | Reboot the system                                   |
| `shutdown` | Power off the system (ACPI)                         |
| `halt`     | Halt the CPU                                        |
## Building & Running

### Build with Docker (recommended)

```bash
make docker-build   # Builds cross-compiler + kernel inside Docker
make run-iso        # Boot the ISO in QEMU
```

### Build with a local cross-compiler

```bash
make all            # Produces kernel/hekaos.bin and kernel/hekaos.iso
make run-iso        # Boot the ISO in QEMU
# or
make run-bin        # Boot the raw binary in QEMU (no GRUB)
```

## Roadmap

### v0.1.0 - Boot Sequence Primitives ✅

A bootable kernel loaded by GRUB, built on an assembly entry point, with a minimal library providing basic types and utility functions. Writes to VGA for screen output. Configures and loads the GDT and IDT to handle keyboard interrupts.

### v0.2.0 - Memory ✅

A complete memory management system with pagination handling. R/W rights on memory. User space and kernel space separation. Physical and virtual memory management. Kernel heap allocator (kmalloc, kfree, ksize, kbrk): returns virtual addresses backed by physically contiguous pages, operating on top of the paging layer. Virtual memory allocator (vmalloc, vfree, vsize, vbrk): provides large, virtually contiguous allocations mapped across physically fragmented pages through page tables, suitable for large buffers where physical contiguity unavailable. Kernel panic handling.

### v0.3.0 - Interrupts

Hardware and software interrupts. A full interrupt descriptor table. Signal handling and scheduling. Global panic fault handling. Register cleaning and stack saving.

### v0.4.0 - Processes

Basic data structures for processes. Process interconnection (kinship, signals, sockets). Process ownership and rights. Helpers for syscalls: `fork`, `wait`, `_exit`, `getuid`, `signal`, `kill`. Process interruptions, memory separation, and multitasking.

### v0.5.0 - Filesystem

A complete interface to read/write an IDE. A complete interface to read/write/delete an ext2 filesystem. A basic file tree (`/sys`, `/var`, `/dev`, `/proc`, `/sys`).

### v0.6.0 - Syscalls, Sockets & Environment

A complete syscall table and syscall system. A complete Unix environment. User accounts with login and password. Password protection. Inter-process communication sockets. A Unix-like filesystem hierarchy.

### v0.7.0 - Modules

Registering kernel modules (creation/destruction). Loading modules at boot time. Communication and callback interfaces between the kernel and modules.

### v0.8.0 - ELF

A complete interface to read, parse, store, and execute ELF files. Syscalls to read ELF files and launch processes from them. A kernel module in ELF format, ready to be inserted at runtime.

### v1.0.0 - First Stable Release

Fully functional basic binaries (`/bin/*`). A libc implementation. A POSIX-compliant shell.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
