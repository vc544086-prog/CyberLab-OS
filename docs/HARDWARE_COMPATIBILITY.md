# Hardware Compatibility & Hypervisor Support

Detailed compatibility matrix:
- **Physical x86_64 PCs**: Intel / AMD processors, Intel AX200/AX210 Wi-Fi, Realtek RTL8821/RTL8111, NVMe and SATA SSDs.
- **VirtualBox**: VMSVGA graphics adapter, Intel PRO/1000 MT Desktop NAT/Bridged virtual NIC.
- **QEMU / KVM**: `-kernel` direct boot for RustOS, `-vga std`, PS/2 emulation.
