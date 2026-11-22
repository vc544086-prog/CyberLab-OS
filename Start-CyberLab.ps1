# ================================================================================
# CYBERLAB OS :: VIRTUALBOX & HYPERVISOR LAUNCHER (POWERSHELL EDITION)
# Author: Vishnu C (25BTRCL282) - JAIN University
# ================================================================================

Write-Host "`n================================================================================" -ForegroundColor Cyan
Write-Host "          CYBERLAB OS :: HYPERVISOR VIRTUAL MACHINE LAUNCHER                     " -ForegroundColor Magenta
Write-Host "================================================================================`n" -ForegroundColor Cyan

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $ScriptDir

# 1. Locate VBoxManage
$VBoxCmd = $null
if (Get-Command "VBoxManage" -ErrorAction SilentlyContinue) {
    $VBoxCmd = "VBoxManage"
} elseif (Test-Path "C:\Program Files\Oracle\VirtualBox\VBoxManage.exe") {
    $VBoxCmd = "C:\Program Files\Oracle\VirtualBox\VBoxManage.exe"
} elseif (Test-Path "C:\Program Files (x86)\Oracle\VirtualBox\VBoxManage.exe") {
    $VBoxCmd = "C:\Program Files (x86)\Oracle\VirtualBox\VBoxManage.exe"
}

if (-not $VBoxCmd) {
    Write-Host "[ERROR] Oracle VM VirtualBox was not detected in PATH or standard installation folders." -ForegroundColor Red
    Write-Host "Please install VirtualBox from: https://www.virtualbox.org/`n" -ForegroundColor Gray
    Read-Host "Press Enter to exit..."
    exit 1
}

Write-Host "[+] VirtualBox detected: $VBoxCmd" -ForegroundColor Green

# 2. Check for CyberLab ISO
$IsoPath = "$ScriptDir\cyberlab-os\build\cyberlab-os.iso"
if (-not (Test-Path $IsoPath)) {
    Write-Host "[!] Checking for ISO..." -ForegroundColor Yellow
    $fallbackIso = Get-ChildItem "$ScriptDir\*.iso" -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($fallbackIso) {
        $IsoPath = $fallbackIso.FullName
    }
}
Write-Host "[+] Target ISO ready: $IsoPath" -ForegroundColor Green

$VMName = "CyberLab-OS-Workstation"

# Check if VM already exists
$vmInfo = & $VBoxCmd showvminfo "$VMName" 2>&1
if ($LASTEXITCODE -eq 0) {
    Write-Host "[+] Configured virtual machine '$VMName' detected." -ForegroundColor Cyan
    # Ensure optimal graphics & ISO attached
    & $VBoxCmd modifyvm "$VMName" --graphicscontroller vboxvga --memory 4096 --vram 64
    & $VBoxCmd storageattach "$VMName" --storagectl "IDE Controller" --port 0 --device 0 --type dvddrive --medium "$IsoPath" 2>$null
    Write-Host "[+] Launching CyberLab OS Desktop in VirtualBox..." -ForegroundColor Green
    & $VBoxCmd startvm "$VMName" --type gui
    Write-Host "`n[+] VM launch sequence completed." -ForegroundColor Green
    exit 0
}

Write-Host "[*] Creating new VirtualBox VM: $VMName..." -ForegroundColor Cyan
& $VBoxCmd createvm --name "$VMName" --ostype "Debian_64" --register

Write-Host "[*] Provisioning Hardware: 4096MB RAM, 2 vCPUs, 64MB VRAM, VBoxVGA..." -ForegroundColor Cyan
& $VBoxCmd modifyvm "$VMName" --memory 4096 --cpus 2 --vram 64 --graphicscontroller vboxvga
& $VBoxCmd modifyvm "$VMName" --nic1 nat --nictype1 82540EM
& $VBoxCmd modifyvm "$VMName" --boot1 dvd --boot2 disk --boot3 none

Write-Host "[*] Configuring Virtual Storage & Attaching ISO..." -ForegroundColor Cyan
& $VBoxCmd storagectl "$VMName" --name "IDE Controller" --add ide
& $VBoxCmd storageattach "$VMName" --storagectl "IDE Controller" --port 0 --device 0 --type dvddrive --medium "$IsoPath"

Write-Host "`n[+] Launching CyberLab OS in VirtualBox..." -ForegroundColor Green
& $VBoxCmd startvm "$VMName" --type gui

Write-Host "`n================================================================================" -ForegroundColor Cyan
Write-Host " Virtual Machine launched successfully. Desktop is live!" -ForegroundColor Green
Write-Host "================================================================================" -ForegroundColor Cyan
