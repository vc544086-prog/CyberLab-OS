# Research Paper Analysis: Fairness vs Throughput in CPU Scheduling

**Author**: Vishnu C (25BTRCL282) - 3-AIML E  
**Institution**: JAIN, FET CAMPUS  
**Course**: 25PCCS201: Operating Systems  

## Abstract
This empirical research investigates whether throughput-oriented CPU scheduling policies necessarily provide fair CPU allocation among competing tasks. Using a trace-free simulation with 8 continuously runnable processes having exponential burst distributions, we evaluate First-Come-First-Served (FCFS), Shortest-Job-First (SJF), and Round Robin (RR). Results over 50 paired seeds demonstrate that while SJF maximizes useful throughput ($0.9996$), it degrades Jain's fairness index to $0.1799$ and produces starvation (minimum share $0.0135\%$). Conversely, Round Robin achieves high fairness ($0.9340$) and balanced minimum share ($6.70\%$) at the expense of useful throughput ($0.9446$) caused by context-switch overhead ($2770$ switches).

## Key Equations
1. **Useful Throughput**:
   $$T = \frac{\sum_{i=1}^{n} S_i}{H}$$
2. **Jain's Fairness Index**:
   $$J = \frac{\left(\sum_{i=1}^{n} x_i\right)^2}{n \sum_{i=1}^{n} x_i^2}$$
   where $J \in [1/n, 1.0]$. For $n=8$, minimum possible fairness is $0.125$.

## Experimental Results (50 Seeds, 95% CI)
| Policy | Useful Throughput | Jain's Fairness | Min Share % | Context Switches |
| :--- | :--- | :--- | :--- | :--- |
| **FCFS** | $0.9798 \pm 0.0002$ | $0.6222 \pm 0.0071$ | $2.5781 \pm 0.0703\%$ | $1008.2 \pm 10.4$ |
| **SJF** | $0.9996 \pm 0.0000$ | $0.1799 \pm 0.0135$ | $0.0135 \pm 0.0090\%$ | $21.0 \pm 1.6$ |
| **RR ($q=5$)** | $0.9446 \pm 0.0001$ | $0.9340 \pm 0.0010$ | $6.7001 \pm 0.0873\%$ | $2770.0 \pm 6.4$ |
