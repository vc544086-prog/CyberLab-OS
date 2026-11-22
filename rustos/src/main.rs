// CyberLab OS - RustOS: 32-bit Bare-Metal Freestanding Operating System Kernel
// Author: Vishnu C (25BTRCL282) - 3-AIML E, JAIN (Deemed-to-be University)
// Course: 25PCCS201: Operating Systems

#![no_std]
#![no_main]

mod vga_buffer;
mod gdt;
mod pic;
mod pit;
mod keyboard;
mod interrupts;
mod shell;

use core::panic::PanicInfo;
use vga_buffer::{WRITER, Color};

// Multiboot 1 Header
#[repr(C, align(4))]
pub struct MultibootHeader {
    magic: u32,
    flags: u32,
    checksum: u32,
}

#[used]
#[no_mangle]
#[link_section = ".multiboot_header"]
pub static MULTIBOOT_HEADER: MultibootHeader = MultibootHeader {
    magic: 0x1BADB002,
    flags: 0x00000003,
    checksum: 0xE4524FFB, // -(0x1BADB002 + 0x00000003)
};

#[no_mangle]
pub extern "C" fn _start() -> ! {
    // 1. Initialize GDT
    unsafe {
        gdt::init();
    }

    // 2. Initialize PIC (remap IRQ 0..15 to 32..47)
    unsafe {
        pic::init();
    }

    // 3. Initialize IDT
    unsafe {
        interrupts::init();
    }

    // 4. Initialize PIT (100 Hz timer)
    unsafe {
        pit::init();
    }

    // 5. Clear VGA and display CyberLab OS Header
    unsafe {
        WRITER.clear_screen();
        WRITER.set_color(Color::LightCyan, Color::Black);
        println!("================================================================================");
        println!("       CYBERLAB OS :: RUSTOS BARE-METAL X86 KERNEL (32-BIT FREESTANDING)       ");
        println!("================================================================================");
        WRITER.set_color(Color::LightMagenta, Color::Black);
        println!(" * Architecture:  x86 (i686 Protected Mode, Ring 0 Supervisor)");
        println!(" * Author:        Vishnu C (25BTRCL282) | 3-AIML E | JAIN University");
        println!(" * Kernel Core:   Rust 2021 (no_std, no_main, Custom GDT/IDT/PIC/PIT)");
        println!(" * Subsystems:    VGA 80x25 Driver, 8259 PIC Remap, PIT 100Hz, PS/2 Keyboard");
        WRITER.set_color(Color::LightGreen, Color::Black);
        println!(" [OK] GDT loaded (Flat 4GB Code & Data Segments)");
        println!(" [OK] Dual 8259 PIC remapped (Master: 0x20-0x27, Slave: 0x28-0x2F)");
        println!(" [OK] IDT loaded with 256 gates (Exceptions + IRQ0-IRQ15)");
        println!(" [OK] PIT Channel 0 initialized at 100 Hz");
        println!(" [OK] PS/2 Keyboard driver active");
        WRITER.set_color(Color::LightGray, Color::Black);
        println!("--------------------------------------------------------------------------------");
        println!("Type 'help' for shell commands, 'sysinfo' for hardware, 'about' for overview.\n");
    }

    // 6. Enable CPU interrupts
    unsafe {
        interrupts::enable();
    }

    // 7. Initial shell prompt
    shell::prompt();

    // 8. Main Kernel Idle & Shell Polling Loop
    loop {
        shell::poll();
        unsafe {
            // Halt CPU until next interrupt (timer, keyboard) arrives
            core::arch::asm!("hlt");
        }
    }
}

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    unsafe {
        interrupts::disable();
        WRITER.set_color(Color::White, Color::Red);
        print!("\n!!! KERNEL PANIC: ");
        if let Some(location) = info.location() {
            print!("at {}:{}: ", location.file(), location.line());
        }
        if let Some(args) = info.message() {
            println!("{}", args);
        } else {
            println!("Fatal unrecoverable error!");
        }
    }
    loop {
        unsafe {
            core::arch::asm!("cli; hlt");
        }
    }
}
