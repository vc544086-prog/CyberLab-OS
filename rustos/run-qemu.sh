#!/bin/bash
# CyberLab OS - RustOS Linux / macOS QEMU Launcher

KERNEL_BIN="target/i686-rustos/release/rustos"
if [ ! -f "$KERNEL_BIN" ]; then
    KERNEL_BIN="target/i686-rustos/debug/rustos"
fi

if [ ! -f "$KERNEL_BIN" ]; then
    echo "[*] Building RustOS..."
    cargo build --release
    KERNEL_BIN="target/i686-rustos/release/rustos"
fi

if ! command -v qemu-system-i386 &> /dev/null; then
    echo "[ERROR] qemu-system-i386 is not installed. Install via: sudo apt install qemu-system-x86"
    exit 1
fi

echo "[+] Booting RustOS in QEMU..."
qemu-system-i386 -kernel "$KERNEL_BIN" -m 128M -serial stdio -vga std
