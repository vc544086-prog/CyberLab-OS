# ================================================================================
# CyberLab OS :: Host Wi-Fi Telemetry & Bridge Synchronizer
# Syncs live host Wi-Fi discovery to VirtualBox VM Guest Properties
# ================================================================================

$vbox = "C:\Program Files\Oracle\VirtualBox\VBoxManage.exe"
$vm = "CyberLab-OS-Workstation"

# 1. Parse current connection
$ifaceOut = netsh wlan show interfaces
$connectedSSID = ""
$bssid = ""
$signal = ""
$channel = ""
$radio = ""

foreach ($line in ($ifaceOut -split "`n")) {
    $line = $line.Trim()
    if ($line -match "^\s*SSID\s*:\s*(.+)$") { $connectedSSID = $matches[1].Trim() }
    elseif ($line -match "^\s*AP BSSID\s*:\s*(.+)$") { $bssid = $matches[1].Trim() }
    elseif ($line -match "^\s*Signal\s*:\s*(.+)$") { $signal = $matches[1].Trim() }
    elseif ($line -match "^\s*Channel\s*:\s*(.+)$") { $channel = $matches[1].Trim() }
    elseif ($line -match "^\s*Radio type\s*:\s*(.+)$") { $radio = $matches[1].Trim() }
}

# 2. Parse all visible networks
$netOut = netsh wlan show networks mode=bssid
$networks = @()
$currSSID = ""

foreach ($line in ($netOut -split "`n")) {
    $line = $line.Trim()
    if ($line -match "^SSID \d+\s*:\s*(.*)$") {
        $currSSID = $matches[1].Trim()
        if ([string]::IsNullOrWhiteSpace($currSSID)) { $currSSID = "[Hidden SSID]" }
    } elseif ($line -match "^\s*Signal\s*:\s*(.+)$") {
        $sig = $matches[1].Trim()
        $networks += "$currSSID ($sig)"
    }
}

$networksList = ($networks | Select-Object -Unique) -join "; "

Write-Output "[+] Connected SSID: $connectedSSID ($signal) on Channel $channel"
Write-Output "[+] Visible Networks: $networksList"

# 3. Push to VirtualBox Guest Properties
& $vbox guestproperty set $vm "/VirtualBox/HostInfo/Wlan/ActiveSSID" $connectedSSID
& $vbox guestproperty set $vm "/VirtualBox/HostInfo/Wlan/ActiveBSSID" $bssid
& $vbox guestproperty set $vm "/VirtualBox/HostInfo/Wlan/ActiveSignal" $signal
& $vbox guestproperty set $vm "/VirtualBox/HostInfo/Wlan/ActiveChannel" $channel
& $vbox guestproperty set $vm "/VirtualBox/HostInfo/Wlan/AvailableNetworks" $networksList
& $vbox guestproperty set $vm "/VirtualBox/HostInfo/Wlan/HostAdapter" "Realtek 8852BE Wireless LAN WiFi 6 PCI-E NIC"

Write-Output "[+] Guest properties successfully synchronized to VM $vm"
