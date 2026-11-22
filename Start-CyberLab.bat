@echo off
setlocal enabledelayedexpansion

title CyberLab OS :: VirtualBox & Hypervisor Launcher

echo ================================================================================
echo          CYBERLAB OS :: HYPERVISOR VIRTUAL MACHINE LAUNCHER                     
echo ================================================================================

:: 1. Locate VBoxManage
set VBOX_CMD=""
where VBoxManage >nul 2>nul
if %ERRORLEVEL% equ 0 (
    set VBOX_CMD=VBoxManage
) else (
    if exist "C:\Program Files\Oracle\VirtualBox\VBoxManage.exe" (
        set VBOX_CMD="C:\Program Files\Oracle\VirtualBox\VBoxManage.exe"
    ) else if exist "C:\Program Files (x86)\Oracle\VirtualBox\VBoxManage.exe" (
        set VBOX_CMD="C:\Program Files (x86)\Oracle\VirtualBox\VBoxManage.exe"
    )
)

if %VBOX_CMD%=="" (
    echo [ERROR] Oracle VM VirtualBox was not detected in PATH or standard installation folders.
    echo Please install VirtualBox from https://www.virtualbox.org/
    echo.
    echo [Option 2] Would you like to run the standalone RustOS kernel in QEMU instead? (Y/N)
    set /p CHOICE="Selection: "
    if /i "!CHOICE!"=="Y" (
        call rustos\run-qemu.bat
        exit /b 0
    )
    pause
    exit /b 1
)

:: 2. Check ISO
set ISO_PATH=cyberlab-os\build\cyberlab-os.iso
if not exist "%ISO_PATH%" (
    echo [!] CyberLab OS ISO not found at '%ISO_PATH%'.
    echo In a production build environment, generate it via: sudo ./cyberlab-os/build.sh
    echo.
    echo Searching for any available .iso in current directory...
    for %%F in (*.iso) do set ISO_PATH=%%F
)

echo [+] VirtualBox found: %VBOX_CMD%
echo [+] Target ISO: %ISO_PATH%

set VM_NAME=CyberLab-OS-Workstation

:: Check if VM already exists
%VBOX_CMD% showvminfo "%VM_NAME%" >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo [+] Existing virtual machine '%VM_NAME%' detected. Launching...
    %VBOX_CMD% startvm "%VM_NAME%" --type gui
    goto FINISH
)

echo [*] Creating new VirtualBox Machine: %VM_NAME%...
%VBOX_CMD% createvm --name "%VM_NAME%" --ostype "Debian_64" --register

echo [*] Configuring Hardware: 4096MB RAM, 2 vCPUs, 64MB VRAM, VMSVGA Graphics...
%VBOX_CMD% modifyvm "%VM_NAME%" --memory 4096 --cpus 2 --vram 64 --graphicscontroller vmsvga
%VBOX_CMD% modifyvm "%VM_NAME%" --nic1 nat --nictype1 82540EM
%VBOX_CMD% modifyvm "%VM_NAME%" --boot1 dvd --boot2 disk --boot3 none

echo [*] Configuring Storage Controllers...
%VBOX_CMD% storagectl "%VM_NAME%" --name "IDE Controller" --add ide
if exist "%ISO_PATH%" (
    %VBOX_CMD% storageattach "%VM_NAME%" --storagectl "IDE Controller" --port 0 --device 0 --type dvddrive --medium "%ISO_PATH%"
) else (
    echo [!] Notice: Attaching empty optical drive. You can mount the ISO when generated.
    %VBOX_CMD% storageattach "%VM_NAME%" --storagectl "IDE Controller" --port 0 --device 0 --type dvddrive --medium emptydrive
)

echo [+] Launching CyberLab OS in VirtualBox...
%VBOX_CMD% startvm "%VM_NAME%" --type gui

:FINISH
echo ================================================================================
echo Virtual Machine launch sequence completed.
echo ================================================================================
pause
