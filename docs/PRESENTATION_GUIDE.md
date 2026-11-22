# Presentation & Viva Defense Guide

## Slide Structure
1. **Title Slide**: CyberLab OS - Dual Architecture Engineering & CPU Scheduling Fairness vs Throughput.
2. **Motivation**: Why dual architecture? Distinguishing bare-metal kernel development from distribution engineering.
3. **Research Question**: Does maximizing throughput necessarily produce fair CPU allocation?
4. **Methodology**: 8 exponential burst processes, 50 paired seeds, Jain's fairness index, context-switch penalty.
5. **Experimental Findings**: Trade-off graphs (Figures 1-7). SJF starves long jobs; RR guarantees turn-taking.
6. **CyberLab Linux Engineering**: Custom KDE desktop, Qt6 Network Center, System Monitor, ByteRunner.
7. **RustOS Bare-Metal Kernel**: Ring 0, GDT, IDT, 8259 PIC remapping, PIT, VGA driver, interactive shell.
8. **Conclusion & Viva Defense**: Systems trade-offs, engineering limitations, and future work.
