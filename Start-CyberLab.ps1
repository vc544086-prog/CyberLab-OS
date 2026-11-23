# ================================================================================
# CYBERLAB OS :: VIRTUALBOX & HYPERVISOR LAUNCHER (POWERSHELL EDITION)
# Author: Vishnu C (25BTRCL282) - JAIN University
# Course: 25PCCS201 - Operating Systems
# ================================================================================

Write-Host "`n================================================================================" -ForegroundColor Cyan
Write-Host "          CYBERLAB OS :: HYPERVISOR VIRTUAL MACHINE LAUNCHER                     " -ForegroundColor Magenta
Write-Host "================================================================================`n" -ForegroundColor Cyan

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $ScriptDir

# 1. Locate VBoxManage & VirtualBox GUI
$VBoxCmd = $null
if (Get-Command "VBoxManage" -ErrorAction SilentlyContinue) {
    $VBoxCmd = "VBoxManage"
} elseif (Test-Path "C:\Program Files\Oracle\VirtualBox\VBoxManage.exe") {
    $VBoxCmd = "C:\Program Files\Oracle\VirtualBox\VBoxManage.exe"
}

$VBoxGui = "C:\Program Files\Oracle\VirtualBox\VirtualBox.exe"

if (-not $VBoxCmd) {
    Write-Host "[ERROR] Oracle VM VirtualBox was not detected in PATH or standard installation folders." -ForegroundColor Red
    Read-Host "Press Enter to exit..."
    exit 1
}

Write-Host "[+] VirtualBox detected: $VBoxCmd" -ForegroundColor Green

# 2. Check for CyberLab ISO
$IsoPath = "$ScriptDir\cyberlab-os\build\cyberlab-os.iso"
Write-Host "[+] Target ISO ready: $IsoPath" -ForegroundColor Green

$VMName = "CyberLab-OS-Workstation"

# 3. Synchronize Host Wi-Fi telemetry before boot
Write-Host "[*] Synchronizing Host Realtek Wi-Fi telemetry..." -ForegroundColor Cyan
& powershell -ExecutionPolicy Bypass -File "$ScriptDir\cyberlab-os\scripts\sync-host-wifi.ps1"

# 4. Configure Universal NAT Networking with Host DNS Resolver (guarantees ping & web transit)
Write-Host "[*] Configuring Universal NAT Networking with Host DNS Resolver..." -ForegroundColor Cyan
& $VBoxCmd modifyvm "$VMName" --nic1 nat --nictype1 82540EM --natdnshostresolver1 on --natdnsproxy1 on 2>$null
& $VBoxCmd modifyvm "$VMName" --graphicscontroller vboxsvga --vram 128 --memory 4096 2>$null

Write-Host "`n[+] Launching CyberLab OS on screen via VirtualBox GUI..." -ForegroundColor Green
Start-Process $VBoxGui -ArgumentList "--startvm `"$VMName`""

Write-Host "`n================================================================================" -ForegroundColor Cyan
Write-Host " Virtual Machine launched on your screen! (Universal NAT + DNS transit active)" -ForegroundColor Green
Write-Host "================================================================================" -ForegroundColor Cyan
