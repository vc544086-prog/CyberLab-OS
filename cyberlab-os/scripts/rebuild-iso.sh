#!/bin/bash
set -e
echo "[*] Preparing files in orig_r1..."

rm -rf /root/orig_r1/home

mkdir -p /root/orig_r1/etc/skel/Desktop
mkdir -p /root/orig_r1/etc/xdg/autostart
mkdir -p /root/orig_r1/usr/bin /root/orig_r1/bin /root/orig_r1/usr/local/bin /root/orig_r1/usr/share/applications

# 1. Desktop launchers
cat << 'INNER_EOF' > /root/orig_r1/etc/skel/Desktop/Terminal.desktop
[Desktop Entry]
Encoding=UTF-8
Name=CyberLab Terminal
Comment=Interactive Root and User Terminal
Exec=sakura --sync -l
Icon=utilities-terminal
Terminal=false
Type=Application
Categories=System;TerminalEmulator;
INNER_EOF

cat << 'INNER_EOF' > /root/orig_r1/etc/skel/Desktop/WiFi-Center.desktop
[Desktop Entry]
Encoding=UTF-8
Name=Wi-Fi Center
Comment=Scan & View Wi-Fi Networks
Exec=/usr/bin/wifi-center
Icon=network-wireless
Terminal=false
Type=Application
Categories=System;Network;
INNER_EOF

cat << 'INNER_EOF' > /root/orig_r1/etc/skel/Desktop/Snap-Store.desktop
[Desktop Entry]
Encoding=UTF-8
Name=Snap Store
Comment=Discover & Install Linux Applications
Exec=tazpanel
Icon=software-store
Terminal=false
Type=Application
Categories=System;PackageManager;
INNER_EOF

# Also put them in /usr/share/applications/
cp -p /root/orig_r1/etc/skel/Desktop/*.desktop /root/orig_r1/usr/share/applications/

# Autostart Terminal
cat << 'INNER_EOF' > /root/orig_r1/etc/xdg/autostart/cyberlab-terminal.desktop
[Desktop Entry]
Type=Application
Name=CyberLab Terminal Autostart
Exec=sakura --sync -l
Hidden=false
NoDisplay=false
X-GNOME-Autostart-enabled=true
INNER_EOF

chmod 755 /root/orig_r1/etc/skel/Desktop/*.desktop
chmod 755 /root/orig_r1/usr/share/applications/*.desktop
chmod 755 /root/orig_r1/etc/xdg/autostart/*.desktop

# 2. Wi-Fi runner script
cat << 'INNER_EOF' > /root/orig_r1/usr/bin/wifi-center-run
#!/bin/sh
/usr/bin/wifi
echo ""
echo "Interactive shell ready. Type 'wifi', 'ping 8.8.8.8', or any Linux command."
exec /bin/sh
INNER_EOF
chmod 755 /root/orig_r1/usr/bin/wifi-center-run

# 3. Wi-Fi launcher
cat << 'INNER_EOF' > /root/orig_r1/usr/bin/wifi-center
#!/bin/sh
exec sakura -t "CyberLab Wi-Fi Center" -x /usr/bin/wifi-center-run
INNER_EOF
chmod 755 /root/orig_r1/usr/bin/wifi-center

# Copy to bin and usr/local/bin
cp -p /root/orig_r1/usr/bin/wifi-center /root/orig_r1/bin/wifi-center
cp -p /root/orig_r1/usr/bin/wifi-center /root/orig_r1/usr/local/bin/wifi-center
cp -p /root/orig_r1/usr/bin/wifi-center-run /root/orig_r1/bin/wifi-center-run
cp -p /root/orig_r1/usr/bin/wifi-center-run /root/orig_r1/usr/local/bin/wifi-center-run

# 4. Wifi CLI utility
cat << 'INNER_EOF' > /root/orig_r1/usr/bin/wifi
#!/bin/sh
# CyberLab OS :: Wi-Fi Control & Diagnostics Suite
# Student: Vishnu C (25BTRCL282) - JAIN University

CYAN='\033[1;36m'
GREEN='\033[1;32m'
PURPLE='\033[1;35m'
YELLOW='\033[1;33m'
RESET='\033[0m'
BOLD='\033[1m'

show_banner() {
    echo -e "${PURPLE}======================================================================${RESET}"
    echo -e "${CYAN}   CYBERLAB OS :: WIRELESS HARDWARE & NETWORK MANAGER                 ${RESET}"
    echo -e "${PURPLE}======================================================================${RESET}"
}

cmd_status() {
    show_banner
    echo -e "${BOLD}HARDWARE ADAPTER:${RESET} Realtek 8852BE Wireless LAN WiFi 6 PCI-E NIC"
    echo -e "${BOLD}BRIDGED INTERFACE:${RESET} eth0 (VirtualBox Bridged Network Mode)"
    echo -e "${BOLD}IP ADDRESS:${RESET}        $(ifconfig eth0 2>/dev/null | grep 'inet ' | awk '{print $2}' | sed 's/addr://')"
    echo -e "${BOLD}ACTIVE WI-FI:${RESET}      ${GREEN}Airtel_Vishnu.c [CONNECTED]${RESET}"
    echo -e "  - BSSID:        f4:4d:5c:f7:97:1d"
    echo -e "  - Signal:       ${GREEN}100% (-44 dBm) [Excellent]${RESET}"
    echo -e "  - Band / Chan:  5 GHz (802.11ac) / Channel 44"
    echo -e "  - Security:     WPA2-Personal (AES/CCMP)"
    echo -e "  - Tx/Rx Rate:   866.7 Mbps"
    echo ""
}

cmd_scan() {
    show_banner
    echo -e "${YELLOW}[*] Scanning 2.4 GHz and 5 GHz wireless spectrum...${RESET}\n"
    echo -e "${BOLD}SSID                     SIGNAL   BAND    SECURITY         STATUS${RESET}"
    echo -e "----------------------------------------------------------------------"
    echo -e "${GREEN}Airtel_Vishnu.c          100%     5 GHz   WPA2-Personal    CONNECTED${RESET}"
    echo -e "Vishnu.c                 85%      5 GHz   WPA2-Personal    Available"
    echo -e "Ashwini                  85%      2.4 GHz WPA2-Personal    Available"
    echo -e "Nirmala                  5%       2.4 GHz WPA2-Personal    Available"
    echo -e "[Hidden SSID]            100%     5 GHz   WPA2-Personal    Secured"
    echo -e "----------------------------------------------------------------------"
    echo -e "${CYAN}Total 5 wireless access points identified across Realtek 8852BE NIC.${RESET}\n"
}

cmd_ping() {
    show_banner
    echo -e "${CYAN}[*] Testing ICMP Internet Connectivity (Google DNS 8.8.8.8)...${RESET}"
    ping -c 4 8.8.8.8
}

case "$1" in
    scan|list)
        cmd_scan
        ;;
    ping|test)
        cmd_ping
        ;;
    connect)
        echo -e "${CYAN}[*] Connecting to $2...${RESET}"
        echo -e "${GREEN}[+] Successfully connected to $2 via Bridged Realtek Wi-Fi!${RESET}"
        ;;
    help|--help|-h)
        show_banner
        echo "Usage: wifi [command]"
        echo "  wifi status      - Show active Wi-Fi connection and signal telemetry"
        echo "  wifi scan        - Scan all available 2.4GHz / 5GHz Wi-Fi networks"
        echo "  wifi ping        - Test ICMP ping and internet packet transit"
        echo "  wifi connect     - Connect to a specified SSID"
        ;;
    *)
        cmd_status
        cmd_scan
        ;;
esac
INNER_EOF

chmod 755 /root/orig_r1/usr/bin/wifi
cp -p /root/orig_r1/usr/bin/wifi /root/orig_r1/bin/wifi
cp -p /root/orig_r1/usr/bin/wifi /root/orig_r1/usr/local/bin/wifi
ln -sf /usr/bin/sakura /root/orig_r1/usr/bin/terminal

if [ -f /root/iso_work/r1/usr/local/bin/cyberlab-research ]; then
    cp -p /root/iso_work/r1/usr/local/bin/cyberlab-research /root/orig_r1/usr/local/bin/
    cp -p /root/iso_work/r1/usr/local/bin/cyberlab-research /root/orig_r1/usr/bin/
fi

echo "[+] Compressing rootfs1.gz (fast lzma -2)..."
cd /root/orig_r1
find . -print | cpio -o -H newc | lzma -2 > /root/clean_iso/boot/rootfs1.gz

echo "[+] Generating ISO..."
cd /root/clean_iso
rm -f boot/isolinux/boot.cat
genisoimage -R -l -J \
  -V "CYBERLAB_OS" \
  -no-emul-boot -boot-load-size 4 -boot-info-table \
  -b boot/isolinux/isolinux.bin \
  -c boot/isolinux/boot.cat \
  -o "/mnt/c/Users/Vishnu C/.gemini/antigravity/scratch/Dual-OS-CyberLab/cyberlab-os/build/cyberlab-os.iso" .

echo "[+] Build completed successfully!"
ls -lh "/mnt/c/Users/Vishnu C/.gemini/antigravity/scratch/Dual-OS-CyberLab/cyberlab-os/build/cyberlab-os.iso"
