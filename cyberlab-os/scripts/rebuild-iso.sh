#!/bin/bash
set -e
echo "[*] Preparing files in orig_r1 for CyberLab OS..."

rm -rf /root/orig_r1/home

mkdir -p /root/orig_r1/etc/skel/Desktop
mkdir -p /root/orig_r1/etc/xdg/autostart
mkdir -p /root/orig_r1/usr/bin /root/orig_r1/bin /root/orig_r1/usr/local/bin /root/orig_r1/usr/share/applications
mkdir -p /root/orig_r1/etc/init.d

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
echo "Interactive shell ready. Type 'wifi', 'ping 8.8.8.8', 'ping google.com' or any command."
exec /bin/sh
INNER_EOF
chmod 755 /root/orig_r1/usr/bin/wifi-center-run

# 3. Wi-Fi launcher
cat << 'INNER_EOF' > /root/orig_r1/usr/bin/wifi-center
#!/bin/sh
exec sakura -t "CyberLab Wi-Fi Center" -x /usr/bin/wifi-center-run
INNER_EOF
chmod 755 /root/orig_r1/usr/bin/wifi-center

cp -p /root/orig_r1/usr/bin/wifi-center /root/orig_r1/bin/wifi-center
cp -p /root/orig_r1/usr/bin/wifi-center /root/orig_r1/usr/local/bin/wifi-center
cp -p /root/orig_r1/usr/bin/wifi-center-run /root/orig_r1/bin/wifi-center-run
cp -p /root/orig_r1/usr/bin/wifi-center-run /root/orig_r1/usr/local/bin/wifi-center-run

# 4. Copy updated Wi-Fi CLI utility
cp -p "/mnt/c/Users/Vishnu C/.gemini/antigravity/scratch/Dual-OS-CyberLab/cyberlab-os/scripts/wifi" /root/orig_r1/usr/bin/wifi
chmod 755 /root/orig_r1/usr/bin/wifi
cp -p /root/orig_r1/usr/bin/wifi /root/orig_r1/bin/wifi
cp -p /root/orig_r1/usr/bin/wifi /root/orig_r1/usr/local/bin/wifi

# 5. Copy Universal Smart Ping utility (handles ICMP + NAT packet transit)
cp -p "/mnt/c/Users/Vishnu C/.gemini/antigravity/scratch/Dual-OS-CyberLab/cyberlab-os/scripts/ping-smart.sh" /root/orig_r1/bin/ping
chmod 755 /root/orig_r1/bin/ping
cp -p /root/orig_r1/bin/ping /root/orig_r1/usr/bin/ping
cp -p /root/orig_r1/bin/ping /root/orig_r1/usr/local/bin/ping

# 6. Pre-configure resilient DNS in /etc/resolv.conf
cat << 'INNER_EOF' > /root/orig_r1/etc/resolv.conf
nameserver 8.8.8.8
nameserver 1.1.1.1
nameserver 10.0.2.3
INNER_EOF
chmod 644 /root/orig_r1/etc/resolv.conf

# 7. Pre-configure kernel sysctl permissions for ICMP
cat << 'INNER_EOF' > /root/orig_r1/etc/sysctl.conf
net.ipv4.ping_group_range = 0 2147483647
net.ipv4.icmp_echo_ignore_broadcasts = 1
INNER_EOF
chmod 644 /root/orig_r1/etc/sysctl.conf

# 8. Add network & DNS initialization to local.sh startup
cat << 'INNER_EOF' > /root/orig_r1/etc/init.d/local.sh
#!/bin/sh
# CyberLab OS :: Network & System Init
[ -d /sys/firmware/efi/efivars ] && mount -t efivarfs none /sys/firmware/efi/efivars 2>/dev/null

ifconfig eth0 up 2>/dev/null
if ! ifconfig eth0 2>/dev/null | grep -q 'inet '; then
    udhcpc -i eth0 -n -q -t 4 2>/dev/null
fi
if ! ifconfig eth0 2>/dev/null | grep -q 'inet '; then
    ifconfig eth0 10.0.2.15 netmask 255.255.255.0 up 2>/dev/null
    route add default gw 10.0.2.2 dev eth0 2>/dev/null
fi
if ! route -n 2>/dev/null | grep -q '^0.0.0.0'; then
    route add default gw 10.0.2.2 dev eth0 2>/dev/null
fi
if [ ! -s /etc/resolv.conf ]; then
    echo "nameserver 8.8.8.8" > /etc/resolv.conf
    echo "nameserver 1.1.1.1" >> /etc/resolv.conf
    echo "nameserver 10.0.2.3" >> /etc/resolv.conf
fi
sysctl -w net.ipv4.ping_group_range="0 2147483647" 2>/dev/null
chmod 4755 /bin/busybox 2>/dev/null
chmod 755 /bin/ping /usr/bin/ping 2>/dev/null
INNER_EOF
chmod 755 /root/orig_r1/etc/init.d/local.sh

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
