# CyberLab OS: Academic Research Experiment Lab

**Research Topic**: Fairness vs Throughput in CPU Scheduling  
**Author**: Vishnu C (25BTRCL282) - 3-AIML E  
**Institution**: JAIN (Deemed-to-be University), Faculty of Engineering and Technology (FET)  
**Course**: 25PCCS201: Operating Systems  

---

## 1. Overview
The **CyberLab OS Research Lab** is an empirical systems evaluation framework designed to investigate the fundamental trade-off between **throughput-oriented** and **fairness-oriented** CPU scheduling algorithms under competitive workloads.

It evaluates three canonical policies:
1. **First-Come-First-Served (FCFS)**: Non-preemptive baseline.
2. **Shortest-Job-First (SJF)**: Throughput-maximizing, greedy policy.
3. **Round Robin (RR)**: Preemptive, time-sliced fairness policy with varying time quanta $q \in \{1, 2, 5, 10, 20\}$ and context-switch overheads $c \in \{0.0, 0.1, 0.2, 0.5\}$.

---

## 2. Methodology & Metrics
- **Controlled Workload**: 8 continuously runnable processes with exponential burst distributions:  
  $\mu \in [2, 3, 4, 6, 8, 12, 18, 25]$ time units.
- **Observation Horizon**: 10,000 continuous time units.
- **Statistical Rigor**: 50 independent seeds ($2026 \dots 2075$) paired across policies with two-sided 95% Student's $t$ confidence intervals ($df=49$, $t_{crit}=2.0096$).
- **Key Metrics**:
  - **Useful CPU Throughput**: $\frac{\sum \text{service}}{T_{\text{horizon}}}$ (excluding context-switch overhead).
  - **Jain's Fairness Index**:
    $$J = \frac{\left(\sum_{i=1}^{n} x_i\right)^2}{n \sum_{i=1}^{n} x_i^2}$$
    where $x_i$ represents the CPU time slice allocated to process $i$.
  - **Minimum Process CPU Share (%)**: $\min_i \left(\frac{x_i}{\sum x}\right) \times 100$
  - **Context-Switch Overhead Count**.

---

## 3. Experimental Results Summary

| Policy | Useful Throughput (mean ± 95% CI) | Jain Fairness Index (mean ± 95% CI) | Minimum CPU Share % (mean ± 95% CI) | Context Switches (mean ± 95% CI) |
| :--- | :--- | :--- | :--- | :--- |
| **FCFS** | $0.9798 \pm 0.0002$ | $0.6222 \pm 0.0071$ | $2.5781 \pm 0.0703\%$ | $1008.2 \pm 10.4$ |
| **SJF** | **$0.9996 \pm 0.0000$** | $0.1799 \pm 0.0135$ | $0.0135 \pm 0.0090\%$ | $21.0 \pm 1.6$ |
| **Round Robin ($q=5$)** | $0.9446 \pm 0.0001$ | **$0.9340 \pm 0.0010$** | **$6.7001 \pm 0.0873\%$** | $2770.0 \pm 6.4$ |

### Sensitivity to Time Quantum ($q$)
- As $q$ decreases from 20 to 1, Jain's fairness increases from $0.7614 \to 0.9953$, while throughput drops from $0.9731 \to 0.8202$ due to context-switch thrashing ($1347 \to 8989$ switches).

---

## 4. Execution Commands

### In CyberLab Linux Distribution:
```bash
# Run full paired comparison suite
cyberlab-research --all

# Run specific experiment
cyberlab-research --policy RR -q 5 -c 0.2

# Regenerate publication figures
cyberlab-research --export-charts
```

### From Python Source:
```bash
cd research-lab
python src/cpu_scheduling_fairness_simulator.py
python benchmarks/run_all_experiments.py
python benchmarks/generate_charts.py
```
