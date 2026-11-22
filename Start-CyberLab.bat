@echo off
setlocal enabledelayedexpansion

title CyberLab OS Virtual Machine Launcher

echo ================================================================================
echo          CYBERLAB OS :: HYPERVISOR VIRTUAL MACHINE LAUNCHER                     
echo ================================================================================

:: 1. Locate VBoxManage
set "VBOX_CMD="
where VBoxManage >nul 2>nul
if %ERRORLEVEL% equ 0 (
    set "VBOX_CMD=VBoxManage"
) else (
    if exist "C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" (
        set "VBOX_CMD=C:\Program Files\Oracle\VirtualBox\VBoxManage.exe"
    ) else if exist "C:\Program Files (x86)\Oracle\VirtualBox\VBoxManage.exe" (
        set "VBOX_CMD=C:\Program Files (x86)\Oracle\VirtualBox\VBoxManage.exe"
    )
)

if not defined VBOX_CMD (
    echo [ERROR] Oracle VM VirtualBox was not detected in PATH or standard installation folders.
    echo Please install VirtualBox from https://www.virtualbox.org/
    pause
    exit /b 1
)

echo [+] VirtualBox detected: "%VBOX_CMD%"

:: 2. Check ISO
set "ISO_PATH=cyberlab-os\build\cyberlab-os.iso"
if not exist "%ISO_PATH%" (
    for %%F in (*.iso) do set "ISO_PATH=%%F"
)

echo [+] Target ISO: %ISO_PATH%

set "VM_NAME=CyberLab-OS-Workstation"

:: Check if VM already exists
"%VBOX_CMD%" showvminfo "%VM_NAME%" >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo [+] Existing virtual machine '%VM_NAME%' detected. Launching Desktop...
    "%VBOX_CMD%" modifyvm "%VM_NAME%" --graphicscontroller vboxvga --memory 4096 --vram 64 >nul 2>nul
    "%VBOX_CMD%" storageattach "%VM_NAME%" --storagectl "IDE Controller" --port 0 --device 0 --type dvddrive --medium "%ISO_PATH%" >nul 2>nul
    "%VBOX_CMD%" startvm "%VM_NAME%" --type gui
    goto FINISH
)

echo [*] Creating new VirtualBox Machine: %VM_NAME%...
"%VBOX_CMD%" createvm --name "%VM_NAME%" --ostype "Debian_64" --register

echo [*] Configuring Hardware: 4096MB RAM, 2 vCPUs, 64MB VRAM, VBoxVGA Graphics...
"%VBOX_CMD%" modifyvm "%VM_NAME%" --memory 4096 --cpus 2 --vram 64 --graphicscontroller vboxvga
"%VBOX_CMD%" modifyvm "%VM_NAME%" --nic1 nat --nictype1 82540EM
"%VBOX_CMD%" modifyvm "%VM_NAME%" --boot1 dvd --boot2 disk --boot3 none

echo [*] Configuring Storage Controllers...
"%VBOX_CMD%" storagectl "%VM_NAME%" --name "IDE Controller" --add ide
"%VBOX_CMD%" storageattach "%VM_NAME%" --storagectl "IDE Controller" --port 0 --device 0 --type dvddrive --medium "%ISO_PATH%"

echo [+] Launching CyberLab OS in VirtualBox...
"%VBOX_CMD%" startvm "%VM_NAME%" --type gui

:FINISH
echo ================================================================================
echo Virtual Machine launch sequence completed.
echo ================================================================================
pause
