#!/bin/bash
# ==============================================================================
# CYBERLAB OS :: DEBIAN 12 BOOKWORM LIVE ISO BUILD SCRIPT
# Author: Vishnu C (25BTRCL282) - JAIN University
# Architecture: x86_64 Dual Bootable (BIOS + UEFI)
# ==============================================================================

set -euo pipefail

WORK_DIR="$(pwd)/build_temp"
OUT_DIR="$(pwd)/build"
ISO_NAME="cyberlab-os.iso"
DEBIAN_VERSION="bookworm"
MIRROR="http://deb.debian.org/debian"

echo "================================================================================"
echo "          CYBERLAB OS :: LIVE LINUX DISTRIBUTION BUILD SYSTEM                   "
echo "================================================================================"

# 1. Root check
if [ "$(id -u)" -ne 0 ]; then
    echo "[ERROR] This build script requires root permissions. Run with: sudo ./build.sh"
    exit 1
fi

# 2. Dependency check
REQUIRED_PKGS="debootstrap squashfs-tools xorriso grub-pc-bin grub-efi-amd64-bin mtools dosfstools"
echo "[*] Verifying host dependencies..."
for pkg in $REQUIRED_PKGS; do
    if ! dpkg -s "$pkg" &>/dev/null; then
        echo "[!] Missing build package: $pkg. Installing..."
        apt-get update && apt-get install -y "$pkg"
    fi
done

# Cleanup Trap
cleanup() {
    echo "[*] Executing cleanup traps..."
    set +e
    umount -lf "$WORK_DIR/chroot/proc" 2>/dev/null || true
    umount -lf "$WORK_DIR/chroot/sys" 2>/dev/null || true
    umount -lf "$WORK_DIR/chroot/dev/pts" 2>/dev/null || true
    umount -lf "$WORK_DIR/chroot/dev" 2>/dev/null || true
}
trap cleanup EXIT INT TERM

# Prepare directory tree
rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR/chroot" "$WORK_DIR/image/live" "$WORK_DIR/image/boot/grub" "$OUT_DIR"

echo "[*] Step 1: Bootstrapping Debian 12 Bookworm root filesystem..."
debootstrap --arch=amd64 "$DEBIAN_VERSION" "$WORK_DIR/chroot" "$MIRROR"

echo "[*] Step 2: Configuring package sources & non-free wireless firmware..."
cat << 'EOF' > "$WORK_DIR/chroot/etc/apt/sources.list"
deb http://deb.debian.org/debian bookworm main contrib non-free non-free-firmware
deb http://deb.debian.org/debian-security bookworm-security main contrib non-free non-free-firmware
deb http://deb.debian.org/debian bookworm-updates main contrib non-free non-free-firmware
EOF

mount --bind /dev "$WORK_DIR/chroot/dev"
mount --bind /dev/pts "$WORK_DIR/chroot/dev/pts"
mount -t proc proc "$WORK_DIR/chroot/proc"
mount -t sysfs sysfs "$WORK_DIR/chroot/sys"

echo "[*] Step 3: Installing Linux Monolithic Kernel, KDE Plasma, SDDM, & Drivers..."
chroot "$WORK_DIR/chroot" /bin/bash << 'EOF'
export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install -y --no-install-recommends \
    linux-image-amd64 live-boot systemd-sysv \
    kde-plasma-desktop sddm dolphin konsole kate \
    network-manager network-manager-gnome wireless-tools wpasupplicant rfkill iw \
    firmware-linux firmware-iwlwifi firmware-realtek firmware-atheros \
    build-essential cmake qt6-base-dev libsdl2-dev python3 python3-pip python3-matplotlib \
    curl git vim htop pciutils usbutils sudo

# Create user 'cyberlab'
useradd -m -s /bin/bash -G sudo cyberlab
echo "cyberlab:cyberlab" | chpasswd
echo "cyberlab ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/cyberlab

# Configure NetworkManager
cat << 'NMCONF' > /etc/NetworkManager/NetworkManager.conf
[main]
plugins=ifupdown,keyfile

[ifupdown]
managed=true
NMCONF

systemctl enable sddm
systemctl enable NetworkManager

apt-get clean
rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
EOF

echo "[*] Step 4: Compiling and Installing CyberLab Custom Applications..."
# Copy custom apps into chroot
mkdir -p "$WORK_DIR/chroot/opt/cyberlab"
cp -r ../cyberlab-os "$WORK_DIR/chroot/opt/cyberlab/"
cp -r ../research-lab "$WORK_DIR/chroot/opt/cyberlab/"

chroot "$WORK_DIR/chroot" /bin/bash << 'EOF'
# Build Network Center
cd /opt/cyberlab/cyberlab-os/network-center
mkdir -p build && cd build && cmake .. && make -j$(nproc) && make install

# Build System Monitor
cd /opt/cyberlab/cyberlab-os/system-monitor
mkdir -p build && cd build && cmake .. && make -j$(nproc) && make install

# Build ByteRunner
cd /opt/cyberlab/cyberlab-os/byterunner
make && cp byterunner /usr/local/bin/ && cp byterunner.desktop /usr/share/applications/

# Install Research Lab CLI
cp /opt/cyberlab/research-lab/cyberlab-research /usr/local/bin/
chmod +x /usr/local/bin/cyberlab-research

# Apply desktop configuration
/opt/cyberlab/cyberlab-os/scripts/setup-desktop.sh
EOF

echo "[*] Step 5: Extracting kernel & initramfs..."
cp "$WORK_DIR/chroot/boot/vmlinuz-"* "$WORK_DIR/image/live/vmlinuz"
cp "$WORK_DIR/chroot/boot/initrd.img-"* "$WORK_DIR/image/live/initrd"

echo "[*] Step 6: Creating Compressed SquashFS Filesystem..."
mksquashfs "$WORK_DIR/chroot" "$WORK_DIR/image/live/filesystem.squashfs" -comp xz -e boot

echo "[*] Step 7: Generating GRUB Hybrid Bootloader Configuration..."
cat << 'EOF' > "$WORK_DIR/image/boot/grub/grub.cfg"
set default="0"
set timeout=5

insmod font
insmod all_video

menuentry "CyberLab OS - Live Desktop (KDE Plasma 64-bit)" {
    linux /live/vmlinuz boot=live quiet splash
    initrd /live/initrd
}

menuentry "CyberLab OS - Failsafe Mode" {
    linux /live/vmlinuz boot=live nomodeset
    initrd /live/initrd
}
EOF

echo "[*] Step 8: Generating Bootable Hybrid ISO image..."
grub-mkrescue -o "$OUT_DIR/$ISO_NAME" "$WORK_DIR/image"

FINAL_SIZE=$(du -h "$OUT_DIR/$ISO_NAME" | cut -f1)
echo "================================================================================"
echo " [SUCCESS] CYBERLAB OS ISO BUILT SUCCESSFULLY!"
echo " Location: $OUT_DIR/$ISO_NAME"
echo " Final Size: $FINAL_SIZE"
echo "================================================================================"
