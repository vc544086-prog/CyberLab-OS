# RustOS: 32-bit Freestanding Bare-Metal Operating System Kernel

**Author**: Vishnu C (25BTRCL282) - 3-AIML E  
**Institution**: JAIN (Deemed-to-be University), Faculty of Engineering and Technology  
**Course**: 25PCCS201: Operating Systems  

---

## 1. Architectural Distinction: Linux vs Bare-Metal Kernel
In **CyberLab OS**, we intentionally implement and maintain two distinct operating system architectures to demonstrate deep systems engineering principles:

| Architectural Dimension | CyberLab Linux (Debian 12 Live) | RustOS (Bare-Metal Kernel) |
| :--- | :--- | :--- |
| **Execution Environment** | Hosted user-space atop Linux Monolithic Kernel | Freestanding Bare-Metal (`no_std`, `no_main`) |
| **Privilege Level** | Ring 3 (User space applications) | Ring 0 (Hardware Supervisor / Kernel space) |
| **Memory Management** | Multi-level Paging, Virtual Memory, Inverted Tables | Flat 4GB Segmentation via GDT, Direct Physical Addresses |
| **Interrupt Handling** | Linux Kernel Virtual File System / D-Bus / Signals | Direct IDT (Interrupt Descriptor Table) & 8259 PIC vectors |
| **Display Pipeline** | X11 / Wayland, DRM/KMS, Mesa, KDE Plasma Compositor | Direct VGA Memory Buffer (`0xB8000`) & CRTC Ports (`0x3D4/0x3D5`) |
| **Input Subsystem** | `evdev`, `libinput`, XKB keyboard maps | Raw PS/2 Port `0x60` Scan Code Set 1 decoding |

---

## 2. Kernel Features
- **Bootloader Compatibility**: Multiboot 1 standard (`0x1BADB002`). Can be booted directly by GRUB2, QEMU `-kernel`, or Limine.
- **Global Descriptor Table (GDT)**: Flat 32-bit memory model (Code selector `0x08`, Data selector `0x10`).
- **Programmable Interrupt Controller (8259 PIC)**: Cascaded master/slave PIC remapped from IRQ 0..15 to CPU vectors 32..47 (`0x20..0x2F`).
- **Programmable Interval Timer (8254 PIT)**: Configured at 100 Hz square wave mode.
- **PS/2 Keyboard Driver**: Interrupt-driven ring buffer handling Shift and CapsLock state machines.
- **Interactive Ring-0 Shell**:
  - `help`: Command documentation
  - `clear`: Screen blanking
  - `about`: Project identity
  - `version`: Target architecture and toolchain
  - `time`: PIT timer ticks and uptime
  - `meminfo`: Physical memory map and buffer assignments
  - `sysinfo`: CPU privilege, interrupt state, hardware registers
  - `echo`: Parameter echoing
  - `shutdown`: ACPI / QEMU port `0x604` / APM `0x8900` shutdown sequence

---

## 3. Building and Running

### Prerequisites
- Rust Nightly or Stable with `rust-src` component:
  ```bash
  rustup component add rust-src
  ```
- QEMU (`qemu-system-i386`)

### Build:
```bash
cargo build --release
```

### Run in QEMU:
- On Windows:
  ```cmd
  run-qemu.bat
  ```
- On Linux:
  ```bash
  ./run-qemu.sh
  ```
