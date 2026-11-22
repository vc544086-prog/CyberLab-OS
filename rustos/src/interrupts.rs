// CyberLab OS - Interrupt Descriptor Table (IDT) & Hardware Handlers

use crate::pic;
use crate::pit;
use crate::keyboard;
use crate::println;

#[repr(C, packed)]
#[derive(Copy, Clone)]
pub struct IdtEntry {
    offset_low: u16,
    selector: u16,
    zero: u8,
    type_attr: u8,
    offset_high: u16,
}

impl IdtEntry {
    pub const fn empty() -> Self {
        IdtEntry {
            offset_low: 0,
            selector: 0,
            zero: 0,
            type_attr: 0,
            offset_high: 0,
        }
    }

    pub fn set_handler(&mut self, handler: u32, sel: u16, flags: u8) {
        self.offset_low = (handler & 0xFFFF) as u16;
        self.selector = sel;
        self.zero = 0;
        self.type_attr = flags;
        self.offset_high = ((handler >> 16) & 0xFFFF) as u16;
    }
}

#[repr(C, packed)]
pub struct IdtPtr {
    limit: u16,
    base: u32,
}

static mut IDT: [IdtEntry; 256] = [IdtEntry::empty(); 256];
static mut IDT_PTR: IdtPtr = IdtPtr { limit: 0, base: 0 };

pub unsafe fn init() {
    IDT_PTR.limit = (core::mem::size_of::<[IdtEntry; 256]>() - 1) as u16;
    IDT_PTR.base = IDT.as_ptr() as u32;

    // Set exception handlers
    IDT[0].set_handler(isr0 as u32, 0x08, 0x8E);  // Divide Error
    IDT[8].set_handler(isr8 as u32, 0x08, 0x8E);  // Double Fault
    IDT[13].set_handler(isr13 as u32, 0x08, 0x8E); // General Protection Fault
    IDT[14].set_handler(isr14 as u32, 0x08, 0x8E); // Page Fault

    // Set IRQ handlers (remapped to 32..47)
    IDT[32].set_handler(irq0 as u32, 0x08, 0x8E); // IRQ0: PIT Timer
    IDT[33].set_handler(irq1 as u32, 0x08, 0x8E); // IRQ1: Keyboard

    // Load IDT
    core::arch::asm!(
        "lidt [{0}]",
        in(reg) &IDT_PTR,
        options(nostack)
    );
}

// Enable interrupts
pub unsafe fn enable() {
    core::arch::asm!("sti", options(nomem, nostack));
}

// Disable interrupts
#[allow(dead_code)]
pub unsafe fn disable() {
    core::arch::asm!("cli", options(nomem, nostack));
}

// ISR 0: Divide by Zero
#[naked]
unsafe extern "C" fn isr0() {
    core::arch::asm!(
        "pusha",
        "call isr0_handler",
        "popa",
        "iretd",
        options(noreturn)
    );
}

#[no_mangle]
extern "C" fn isr0_handler() {
    println!("\n[EXCEPTION] Divide by Zero occurred! CPU execution paused.");
    loop {
        unsafe { core::arch::asm!("hlt"); }
    }
}

// ISR 8: Double Fault
#[naked]
unsafe extern "C" fn isr8() {
    core::arch::asm!(
        "pusha",
        "call isr8_handler",
        "popa",
        "add esp, 4", // error code
        "iretd",
        options(noreturn)
    );
}

#[no_mangle]
extern "C" fn isr8_handler() {
    println!("\n[PANIC: EXCEPTION 8] Double Fault detected!");
    loop {
        unsafe { core::arch::asm!("hlt"); }
    }
}

// ISR 13: General Protection Fault
#[naked]
unsafe extern "C" fn isr13() {
    core::arch::asm!(
        "pusha",
        "call isr13_handler",
        "popa",
        "add esp, 4", // error code
        "iretd",
        options(noreturn)
    );
}

#[no_mangle]
extern "C" fn isr13_handler() {
    println!("\n[EXCEPTION 13] General Protection Fault!");
}

// ISR 14: Page Fault
#[naked]
unsafe extern "C" fn isr14() {
    core::arch::asm!(
        "pusha",
        "call isr14_handler",
        "popa",
        "add esp, 4",
        "iretd",
        options(noreturn)
    );
}

#[no_mangle]
extern "C" fn isr14_handler() {
    println!("\n[EXCEPTION 14] Page Fault!");
}

// IRQ 0: PIT Timer
#[naked]
unsafe extern "C" fn irq0() {
    core::arch::asm!(
        "pusha",
        "call irq0_handler",
        "popa",
        "iretd",
        options(noreturn)
    );
}

#[no_mangle]
extern "C" fn irq0_handler() {
    pit::on_tick();
    unsafe {
        pic::send_eoi(0);
    }
}

// IRQ 1: Keyboard
#[naked]
unsafe extern "C" fn irq1() {
    core::arch::asm!(
        "pusha",
        "call irq1_handler",
        "popa",
        "iretd",
        options(noreturn)
    );
}

#[no_mangle]
extern "C" fn irq1_handler() {
    unsafe {
        keyboard::on_keyboard_interrupt();
        pic::send_eoi(1);
    }
}
