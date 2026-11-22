# Comprehensive Viva Voce Questions & Answers

### Q1: Why does Shortest Job First (SJF) achieve higher throughput than Round Robin?
**Answer**: SJF executes the task with the smallest CPU burst first, completing bursts rapidly and minimizing context switches (21 switches vs 2770 in RR). By avoiding context-switch overhead penalties (0.2 time units per switch), more wall-clock time is dedicated to useful CPU computation.

### Q2: If SJF has higher throughput, why isn't it universally used in general-purpose OS kernels?
**Answer**: SJF suffers from severe starvation of long-running tasks. In our empirical test, the minimum process CPU share was only $0.0135\%$ and Jain's fairness dropped to $0.1799$. General-purpose operating systems like Linux need interactive responsiveness and fairness, which Round Robin or Completely Fair Scheduler (CFS/EEVDF) provide.

### Q3: Why is 8259 PIC remapping mandatory in x86 Protected Mode?
**Answer**: IBM PC BIOS maps IRQ 0..7 to interrupt vectors 8..15. However, in 32-bit protected mode, vectors 8..15 are reserved by Intel for CPU exceptions (e.g., Vector 8 is Double Fault, Vector 13 is General Protection Fault). Remapping the PIC to vectors 32..47 prevents hardware interrupts from being misinterpreted as fatal CPU exceptions.

### Q4: What is the architectural difference between CyberLab Linux and RustOS?
**Answer**: CyberLab Linux is a hosted distribution running user-space processes on top of a monolithic Linux kernel with virtual memory and multi-tier paging. RustOS is a freestanding, bare-metal operating system running directly on the CPU in Ring 0 supervisor mode with flat segmentation, direct VGA memory access, and custom interrupt handling.
