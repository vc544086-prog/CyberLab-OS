// CyberLab OS - Interactive Kernel Shell
// Supported commands: help, clear, about, version, time, meminfo, sysinfo, echo, shutdown

use crate::vga_buffer::{WRITER, Color, outb};
use crate::pit;
use crate::keyboard;
use crate::{print, println};

const MAX_CMD_LEN: usize = 64;
static mut CMD_BUFFER: [u8; MAX_CMD_LEN] = [0; MAX_CMD_LEN];
static mut CMD_LEN: usize = 0;

pub fn prompt() {
    unsafe {
        WRITER.set_color(Color::LightCyan, Color::Black);
        print!("cyberlab@rustos");
        WRITER.set_color(Color::LightMagenta, Color::Black);
        print!(":~$ ");
        WRITER.set_color(Color::White, Color::Black);
    }
}

pub fn handle_char(ch: u8) {
    match ch {
        b'\n' => {
            println!();
            execute_buffer();
            prompt();
        }
        0x08 => { // Backspace
            unsafe {
                if CMD_LEN > 0 {
                    CMD_LEN -= 1;
                    WRITER.backspace();
                }
            }
        }
        byte => {
            unsafe {
                if CMD_LEN < MAX_CMD_LEN - 1 {
                    CMD_BUFFER[CMD_LEN] = byte;
                    CMD_LEN += 1;
                    WRITER.write_byte(byte);
                }
            }
        }
    }
}

fn execute_buffer() {
    let len = unsafe { CMD_LEN };
    if len == 0 {
        return;
    }

    let cmd_str = match core::str::from_utf8(unsafe { &CMD_BUFFER[..len] }) {
        Ok(s) => s.trim(),
        Err(_) => {
            println!("Error: Invalid UTF-8 sequence");
            unsafe { CMD_LEN = 0; }
            return;
        }
    };

    let mut parts = cmd_str.splitn(2, ' ');
    let command = parts.next().unwrap_or("");
    let args = parts.next().unwrap_or("");

    match command {
        "help" => cmd_help(),
        "clear" => cmd_clear(),
        "about" => cmd_about(),
        "version" => cmd_version(),
        "time" => cmd_time(),
        "meminfo" => cmd_meminfo(),
        "sysinfo" => cmd_sysinfo(),
        "echo" => cmd_echo(args),
        "shutdown" => cmd_shutdown(),
        "" => {}
        unknown => {
            println!("Command '{}' not found. Type 'help' for available commands.", unknown);
        }
    }

    unsafe {
        CMD_LEN = 0;
    }
}

fn cmd_help() {
    println!("Available RustOS Kernel Commands:");
    println!("  help      - Display this list of shell commands");
    println!("  clear     - Clear the VGA text terminal screen");
    println!("  about     - Information regarding the CyberLab OS Dual Platform");
    println!("  version   - Kernel version and build architecture");
    println!("  time      - PIT timer ticks and elapsed system uptime");
    println!("  meminfo   - Physical memory map and buffer assignments");
    println!("  sysinfo   - Hardware status, CPU privilege, and interrupt controller");
    println!("  echo <str>- Print provided arguments back to terminal");
    println!("  shutdown  - Initiate hardware ACPI/APM shutdown or system halt");
}

fn cmd_clear() {
    unsafe {
        WRITER.clear_screen();
    }
}

fn cmd_about() {
    unsafe {
        WRITER.set_color(Color::LightCyan, Color::Black);
        println!("============================================================");
        println!(" CYBERLAB OS - Dual Architecture Operating Systems Platform");
        println!("============================================================");
        WRITER.set_color(Color::White, Color::Black);
        println!("Author: Vishnu C (25BTRCL282) - 3-AIML E");
        println!("Institution: JAIN (Deemed-to-be University)");
        println!("Dual-OS Architecture:");
        println!(" 1. CyberLab Linux: Debian 12 Bookworm + KDE Plasma Live OS");
        println!(" 2. RustOS: 32-bit Freestanding Bare-Metal x86 Kernel");
        println!("Research Paper: Fairness vs Throughput in CPU Scheduling");
    }
}

fn cmd_version() {
    println!("RustOS Kernel v0.1.0-baremetal");
    println!("Target: i686-unknown-none (x86 32-bit Protected Mode)");
    println!("Toolchain: Rust 2021 Edition (no_std, no_main, custom GDT/IDT/PIC)");
}

fn cmd_time() {
    let uptime = pit::get_uptime_seconds();
    let ticks = pit::get_ticks();
    println!("System Uptime: {} seconds ({} clock ticks @ {} Hz)", uptime, ticks, pit::TARGET_HZ);
}

fn cmd_meminfo() {
    println!("Memory Information (Kernel Direct Mappings):");
    println!("  VGA Video RAM:      0x000B8000 - 0x000B8FA0 (80x25x2 bytes)");
    println!("  Kernel Code Base:   0x00100000 (1 MB physical entry)");
    println!("  GDT / IDT Tables:   Physical static allocations in .bss");
    println!("  Paging / MMU:       Identity flat 4GB segmentation (Ring 0)");
    println!("  [Note: Full E820 ACPI dynamic memory map not yet enumerated]");
}

fn cmd_sysinfo() {
    println!("System & Hardware Architecture:");
    println!("  Architecture:       x86 32-bit Protected Mode (i686)");
    println!("  Privilege Level:    Ring 0 (Kernel Supervisor)");
    println!("  Interrupt Controller: Dual 8259 PIC (Remapped to 0x20..0x2F)");
    println!("  PIT Channel 0:      1193182 Hz oscillator / Divisor = 100 Hz");
    println!("  Input Driver:       PS/2 Keyboard Controller (Port 0x60)");
    println!("  Display Driver:     VGA 80x25 Character Mode (Ports 0x3D4/0x3D5)");
}

fn cmd_echo(args: &str) {
    println!("{}", args);
}

fn cmd_shutdown() {
    println!("Attempting ACPI / QEMU hypervisor poweroff...");
    unsafe {
        // QEMU ACPI shutdown port (0x604)
        outb(0x604, 0x2000 as u8);
        // Bochs / older QEMU shutdown port (0xB004)
        outb(0xB004, 0x00);
        outb(0xB004, 0x2000 as u8);
        // APM power-off (0x8900)
        outb(0x8900, 0);
    }
    
    // If running on bare metal without ACPI emulator:
    println!("ACPI poweroff signal issued. If hardware remains powered, halting CPU.");
    loop {
        unsafe {
            core::arch::asm!("cli; hlt");
        }
    }
}

pub fn poll() {
    while let Some(ch) = keyboard::pop_key() {
        handle_char(ch);
    }
}
