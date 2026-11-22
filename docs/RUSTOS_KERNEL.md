# RustOS Kernel Engineering Deep-Dive

Comprehensive documentation of the bare-metal 32-bit Rust kernel:
- Memory: Physical flat segmentation via GDT.
- Interrupt Vectoring: Remapping 8259 PIC from BIOS vectors (8-15) to vectors (32-47).
- PIT: Generating 100 Hz ticks on IRQ0.
- Ring-0 Shell: Dispatching commands while executing in supervisor mode.
