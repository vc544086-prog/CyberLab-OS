@echo off
setlocal enabledelayedexpansion

echo ===================================================================
echo   CYBERLAB OS :: RustOS Bare-Metal Kernel QEMU Launcher
echo ===================================================================

:: Look for QEMU in common paths
set QEMU_BIN=""
where qemu-system-i386 >nul 2>nul
if %ERRORLEVEL% equ 0 (
    set QEMU_BIN=qemu-system-i386
) else (
    if exist "C:\Program Files\qemu\qemu-system-i386.exe" (
        set QEMU_BIN="C:\Program Files\qemu\qemu-system-i386.exe"
    ) else if exist "C:\Program Files (x86)\qemu\qemu-system-i386.exe" (
        set QEMU_BIN="C:\Program Files (x86)\qemu\qemu-system-i386.exe"
    )
)

if %QEMU_BIN%=="" (
    echo [ERROR] QEMU executable (qemu-system-i386) was not found in PATH or standard directories.
    echo Please install QEMU for Windows from https://www.qemu.org/download/
    echo or ensure qemu-system-i386 is added to your system PATH.
    pause
    exit /b 1
)

set KERNEL_BIN=target\i686-rustos\release\rustos
if not exist %KERNEL_BIN% (
    echo [!] Release binary not found, checking debug build...
    set KERNEL_BIN=target\i686-rustos\debug\rustos
)

if not exist %KERNEL_BIN% (
    echo [*] Kernel binary not found. Compiling RustOS...
    cargo build --release
    set KERNEL_BIN=target\i686-rustos\release\rustos
)

if not exist %KERNEL_BIN% (
    echo [ERROR] Failed to locate or compile RustOS kernel.
    pause
    exit /b 1
)

echo [+] Launching RustOS bare-metal kernel in QEMU...
%QEMU_BIN% -kernel %KERNEL_BIN% -m 128M -serial stdio -vga std
pause
