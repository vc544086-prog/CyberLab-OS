// CyberLab OS - 8254 PIT (Programmable Interval Timer) Driver
// Controls system clock frequency and tracks elapsed kernel ticks

use crate::vga_buffer::outb;

const PIT_CHANNEL_0: u16 = 0x40;
const PIT_COMMAND: u16 = 0x43;
const BASE_FREQUENCY: u32 = 1193182;
pub const TARGET_HZ: u32 = 100;

static mut SYSTEM_TICKS: u64 = 0;

pub unsafe fn init() {
    let divisor = (BASE_FREQUENCY / TARGET_HZ) as u16;

    // Channel 0, Access mode low/high byte, Mode 3 (Square wave generator), Binary
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL_0, (divisor & 0xFF) as u8);
    outb(PIT_CHANNEL_0, ((divisor >> 8) & 0xFF) as u8);
}

pub fn on_tick() {
    unsafe {
        SYSTEM_TICKS += 1;
    }
}

pub fn get_ticks() -> u64 {
    unsafe { SYSTEM_TICKS }
}

pub fn get_uptime_seconds() -> u64 {
    unsafe { SYSTEM_TICKS / (TARGET_HZ as u64) }
}
