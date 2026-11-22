// CyberLab OS - PS/2 Keyboard Driver (Scan Code Set 1)

use crate::vga_buffer::inb;

const KEYBOARD_PORT: u16 = 0x60;

static mut SHIFT_PRESSED: bool = false;
static mut CAPS_LOCK: bool = false;

// Ring buffer for keyboard characters
const KEY_BUFFER_SIZE: usize = 128;
static mut KEY_BUFFER: [u8; KEY_BUFFER_SIZE] = [0; KEY_BUFFER_SIZE];
static mut BUF_HEAD: usize = 0;
static mut BUF_TAIL: usize = 0;

static SCAN_CODE_MAP: [u8; 128] = [
    0,   27,  b'1', b'2', b'3', b'4', b'5', b'6', b'7', b'8', b'9', b'0', b'-', b'=', 0x08, /* Backspace */
    b'\t', b'q', b'w', b'e', b'r', b't', b'y', b'u', b'i', b'o', b'p', b'[', b']', b'\n', /* Enter */
    0,   /* Ctrl */
    b'a', b's', b'd', b'f', b'g', b'h', b'j', b'k', b'l', b';', b'\'', b'`',
    0,   /* Left Shift */
    b'\\', b'z', b'x', b'c', b'v', b'b', b'n', b'm', b',', b'.', b'/',
    0,   /* Right Shift */
    b'*',
    0,   /* Alt */
    b' ', /* Space */
    0,   /* CapsLock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F1 - F10 */
    0, 0, /* NumLock, ScrollLock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
];

static SCAN_CODE_MAP_SHIFT: [u8; 128] = [
    0,   27,  b'!', b'@', b'#', b'$', b'%', b'^', b'&', b'*', b'(', b')', b'_', b'+', 0x08,
    b'\t', b'Q', b'W', b'E', b'R', b'T', b'Y', b'U', b'I', b'O', b'P', b'{', b'}', b'\n',
    0,
    b'A', b'S', b'D', b'F', b'G', b'H', b'J', b'K', b'L', b':', b'"', b'~',
    0,
    b'|', b'Z', b'X', b'C', b'V', b'B', b'N', b'M', b'<', b'>', b'?',
    0,
    b'*',
    0,
    b' ',
    0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
];

pub unsafe fn on_keyboard_interrupt() {
    let scancode = inb(KEYBOARD_PORT);

    // Released key has highest bit set (0x80)
    if scancode & 0x80 != 0 {
        let released = scancode & 0x7F;
        if released == 0x2A || released == 0x36 {
            SHIFT_PRESSED = false;
        }
        return;
    }

    // Pressed key
    match scancode {
        0x2A | 0x36 => {
            SHIFT_PRESSED = true;
        }
        0x3A => {
            CAPS_LOCK = !CAPS_LOCK;
        }
        code if (code as usize) < SCAN_CODE_MAP.len() => {
            let use_shift = SHIFT_PRESSED ^ CAPS_LOCK;
            let ch = if use_shift {
                SCAN_CODE_MAP_SHIFT[code as usize]
            } else {
                SCAN_CODE_MAP[code as usize]
            };

            if ch != 0 {
                push_key(ch);
            }
        }
        _ => {}
    }
}

pub fn push_key(ch: u8) {
    unsafe {
        let next = (BUF_TAIL + 1) % KEY_BUFFER_SIZE;
        if next != BUF_HEAD {
            KEY_BUFFER[BUF_TAIL] = ch;
            BUF_TAIL = next;
        }
    }
}

pub fn pop_key() -> Option<u8> {
    unsafe {
        if BUF_HEAD == BUF_TAIL {
            None
        } else {
            let ch = KEY_BUFFER[BUF_HEAD];
            BUF_HEAD = (BUF_HEAD + 1) % KEY_BUFFER_SIZE;
            Some(ch)
        }
    }
}
