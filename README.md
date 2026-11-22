# CyberLab OS – Dual Architecture Operating Systems Engineering & Research Platform

**Author**: Vishnu C (25BTRCL282) - 3-AIML E  
**Institution**: JAIN (Deemed-to-be University), Faculty of Engineering and Technology  
**Course**: 25PCCS201: Operating Systems  

---

## 🌟 Executive Summary
**CyberLab OS** is an operating systems engineering platform featuring:
1. **Debian 12 Live Linux Distribution**: Custom KDE Plasma desktop with a floating bottom dock, top panel, CyberLab Network Center (Qt6/D-Bus), System Monitor (/proc), and ByteRunner retro arcade game (SDL2).
2. **RustOS Bare-Metal Kernel**: 32-bit freestanding x86 kernel (`no_std`, `no_main`) running in Ring 0 with custom GDT, IDT, 8259 PIC, PIT, and VGA driver.
3. **Academic Research Experiment Lab**: Direct empirical evaluation of **Fairness vs Throughput in CPU Scheduling** with 50 paired seeds, statistical tables, and publication graphs.

---

## 🚀 Quick Start

### 1. Launch Research Lab Benchmarks
```bash
python research-lab/benchmarks/run_all_experiments.py
python research-lab/benchmarks/generate_charts.py
```

### 2. Launch RustOS in QEMU
- On Windows:
  ```cmd
  cd rustos
  run-qemu.bat
  ```
- On Linux:
  ```bash
  cd rustos
  ./run-qemu.sh
  ```

### 3. Launch CyberLab OS in VirtualBox
- On Windows:
  ```cmd
  Start-CyberLab.bat
  ```

---

## 📚 Complete Documentation
- [Project Overview](docs/PROJECT_OVERVIEW.md)
- [Systems Architecture](docs/ARCHITECTURE.md)
- [Research Paper Analysis](docs/RESEARCH_PAPER_ANALYSIS.md)
- [Viva Presentation Guide](docs/PRESENTATION_GUIDE.md)
- [RustOS Kernel Guide](docs/RUSTOS_KERNEL.md)
- [Network Center Manual](docs/NETWORK_CENTER.md)
- [Viva Questions & Answers](docs/VIVA_QUESTIONS.md)
