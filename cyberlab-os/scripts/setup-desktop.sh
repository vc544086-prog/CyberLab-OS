#!/bin/bash
set -e
echo "[*] Configuring CyberLab OS KDE Plasma Desktop Environment..."

# Install theme and colors
mkdir -p /etc/skel/.config
cp /opt/cyberlab/desktop/kdeglobals /etc/skel/.config/kdeglobals
cp /opt/cyberlab/desktop/plasma-org.kde.plasma.desktop-appletsrc /etc/skel/.config/plasma-org.kde.plasma.desktop-appletsrc

# Set default 4 virtual workspaces (Research, Development, Networking, System Monitoring)
kwriteconfig5 --file /etc/skel/.config/kwinrc --group Desktops --key Number 4
kwriteconfig5 --file /etc/skel/.config/kwinrc --group Desktops --key Rows 1
kwriteconfig5 --file /etc/skel/.config/kwinrc --group Desktops --key Name_1 "Research"
kwriteconfig5 --file /etc/skel/.config/kwinrc --group Desktops --key Name_2 "Development"
kwriteconfig5 --file /etc/skel/.config/kwinrc --group Desktops --key Name_3 "Networking"
kwriteconfig5 --file /etc/skel/.config/kwinrc --group Desktops --key Name_4 "System Monitoring"

echo "[+] CyberLab Desktop configured successfully."
