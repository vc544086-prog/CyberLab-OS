# CyberLab OS: Dual-OS Systems Architecture

```
+-----------------------------------------------------------------------------------+
|                           CYBERLAB OS PLATFORM                                     |
+-----------------------------------------+-----------------------------------------+
|      CyberLab Linux Distribution        |         RustOS Bare-Metal Kernel        |
+-----------------------------------------+-----------------------------------------+
| Applications:                           | Kernel Execution:                       |
|  - CyberLab Network Center (Qt6/D-Bus)  |  - x86 32-bit Protected Mode (Ring 0)   |
|  - CyberLab System Monitor (/proc)      |  - Memory: Flat 4GB Segmentation (GDT)  |
|  - ByteRunner Arcade Engine (SDL2)      |  - Interrupts: IDT + 8259 PIC Remapped  |
|  - Academic Research CLI (/usr/local)   |  - Clock: 8254 PIT @ 100 Hz             |
|                                         |  - Video: Direct VGA Buffer (0xB8000)   |
| User Space Desktop:                     |  - Input: Raw PS/2 Port 0x60 Scancodes  |
|  - KDE Plasma + Floating Bottom Dock    |  - Interactive Supervisor Shell         |
|  - SDDM Display Manager                 |                                         |
|                                         | Hardware Environment:                   |
| Host OS Base:                           |  - Freestanding x86 Hardware / QEMU     |
|  - Monolithic Linux Kernel 6.x          |  - Zero libc, Zero OS dependency        |
|  - Virtual Memory / Multi-level Paging  |                                         |
+-----------------------------------------+-----------------------------------------+
```
