# CyberLab OS Troubleshooting Manual

Common operational issues and remedies:
1. **No Wi-Fi visible in VirtualBox**: Expected behavior. VirtualBox exposes virtual Ethernet, not host Wi-Fi cards.
2. **Permission Denied in build.sh**: Ensure build is executed with `sudo`.
3. **QEMU executable not found**: Add QEMU installation directory to system PATH.
