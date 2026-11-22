@echo off
setlocal enabledelayedexpansion

title CyberLab OS Virtual Machine Launcher

echo ================================================================================
echo          CYBERLAB OS :: HYPERVISOR VIRTUAL MACHINE LAUNCHER                     
echo ================================================================================

cd /d "%~dp0"

echo [*] Synchronizing Host Realtek Wi-Fi telemetry to VM...
powershell -ExecutionPolicy Bypass -File "cyberlab-os\scripts\sync-host-wifi.ps1"

echo [*] Launching CyberLab OS on screen...
start "" "C:\Program Files\Oracle\VirtualBox\VirtualBox.exe" --startvm "CyberLab-OS-Workstation"

echo ================================================================================
echo Virtual Machine launched successfully.
echo ================================================================================
pause
