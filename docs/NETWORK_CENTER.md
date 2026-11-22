# CyberLab Network Center Architecture

The Network Center is a modular Qt6 C++ application communicating directly with the NetworkManager system daemon.
- **Hardware Abstraction**: Queries D-Bus and `nmcli` to identify active adapters.
- **VirtualBox Integration**: Detects hypervisor environments via `/sys/class/dmi/id/product_name` and avoids fabricating Wi-Fi scans.
- **Secure Authentication**: Secrets are passed via NetworkManager API objects rather than raw command-line arguments.
