# A Simple Linux Kernel

In the course ECE 391 (Operating Systems), myself and three other teammates designed and implemented a simplified Linux Operating System.

## Project structure

```
├───fish    # Files for fish program
├───fsdir   # Files stored in filesystem
├───student-distrib  # Kernel implemented here
│       boot.S
│       DEBUG
│       debug.h
│       debug.sh
│       excepts.c  # System exceptions
│       excepts.h
│       excepts_s.h
│       excepts_s.S
│       file.c    
│       file.h
│       filedir.c  # Read only filesystem implementation
│       filedir.h
│       filesystem.c  #  Filesystem helper functions
│       filesystem.h
│       filesys_img
│       i8259.c
│       i8259.h
│       INSTALL
│       kernel.c  # Kernel Launch
│       keyboard.c    # Keyboard driver
│       keyboard.h
│       lib.c    # Library functions
│       lib.h
│       Makefile
│       mp3.img
│       multiboot.h
│       page.h    # Paging Implementation
│       page.S
│       paging.c
│       paging.h
│       pcb.c    # Process Control Block Setup
│       pcb.h
│       pit.c    # Programmable Interrupt driver
│       pit.h
│       rtc.c    # Real time clock driver
│       rtc.h
│       scheduler.c  # Process scheduler
│       scheduler.h
│       systemcall.c  # System call driver
│       systemcall.h
│       system_s.h
│       system_s.S
│       terminal.c    # Terminal driver
│       terminal.h
│       tests.c
│       tests.h    # Test cases for functionality
│       types.h
│       x86_desc.h
│       x86_desc.S
└───syscalls
```

## Features

- Memory paging
- i8259 PIC interrupt handling
- Exception handling
- Support for devices: keyboard, real-time clock, programmable interrupt controller
- In memory read-only filesystem
- Round-robin process scheduling based on Programmable Interrupt Timer (allows for up to 6 processes to run seemingly simultaneously on single processor system)

## **My contribution:**

- Implemented interrupt descriptor table
- Implemented drivers for the following devices: RTC, Programmable Interrupt Controller, Keyboard
- Implemented paging with 4KB page and 4MB page for both kernel and user space
- Implemented system calls and provided support for running six tasks concurrently from system images in the file system
- Implemented the terminal driver with ability to switch between 3 separate terminals
- Implemented process scheduling to switch between currently running processes
- Built testcases to test functionality of each of the features I implemented
- Overall project debugging
- Implemented terminal switching (for three separate terminals)
