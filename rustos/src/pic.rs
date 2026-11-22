// CyberLab OS - 8259 PIC (Programmable Interrupt Controller) Driver
// Remaps master/slave PIC from default IBM BIOS vectors (0x08-0x0F) to (0x20-0x2F)
// to prevent collisions with CPU exception vectors.

use crate::vga_buffer::outb;

const PIC1_COMMAND: u16 = 0x20;
const PIC1_DATA: u16 = 0x21;
const PIC2_COMMAND: u16 = 0xA0;
const PIC2_DATA: u16 = 0xA1;

const ICW1_INIT: u8 = 0x11;
const ICW4_8086: u8 = 0x01;
const PIC_EOI: u8 = 0x20;

pub unsafe fn init() {
    // ICW1: Start initialization sequence in cascade mode
    outb(PIC1_COMMAND, ICW1_INIT);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT);
    io_wait();

    // ICW2: Vector offset (Master = 32 / 0x20, Slave = 40 / 0x28)
    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    // ICW3: Tell Master PIC there is a slave at IRQ2 (0000 0100)
    outb(PIC1_DATA, 0x04);
    io_wait();
    // Tell Slave PIC its cascade identity (0000 0010)
    outb(PIC2_DATA, 0x02);
    io_wait();

    // ICW4: Set 8086/88 mode
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // Unmask IRQ0 (Timer) and IRQ1 (Keyboard), mask others for now
    outb(PIC1_DATA, 0xFC); // 1111 1100 -> IRQ0 and IRQ1 enabled
    outb(PIC2_DATA, 0xFF); // Disable all slave IRQs
}

pub unsafe fn send_eoi(irq: u8) {
    if irq >= 8 {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

#[inline]
pub unsafe fn io_wait() {
    outb(0x80, 0);
}
