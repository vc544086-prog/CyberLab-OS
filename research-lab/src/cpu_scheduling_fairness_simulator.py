#!/usr/bin/env python3
"""
CyberLab OS - Academic Research Experiment Simulator
Topic: Fairness vs Throughput in CPU Scheduling
Author: Vishnu C (25BTRCL282), 3-AIML E, JAIN (Deemed-to-be University)
Course: 25PCCS201: Operating Systems

Evaluates FCFS, SJF, and Round Robin across 50 paired seeds (2026-2075)
and sensitivity parameters for Round Robin quantum and context-switch costs.
"""

import random
import statistics
import math
from collections import deque

MEANS = [2, 3, 4, 6, 8, 12, 18, 25]
HORIZON = 10_000.0
CONTEXT_SWITCH_COST = 0.2
RR_QUANTUM = 5.0
SEEDS = range(2026, 2076)          # 50 independent seeds
T_CRIT_49 = 2.0096                 # two-sided 95% t critical value, df=49
T_CRIT_19 = 2.093                  # two-sided 95% t critical value, df=19
SENSITIVITY_SEEDS = range(3026, 3046)  # 20 paired seeds per setting

def simulate(policy, seed, horizon=HORIZON, overhead=CONTEXT_SWITCH_COST, quantum=RR_QUANTUM):
    rng = random.Random(seed)
    n = len(MEANS)
    remaining = [max(1e-6, rng.expovariate(1.0 / m)) for m in MEANS]
    service = [0.0] * n
    ready = deque(range(n))
    t, last, switches = 0.0, None, 0

    while t < horizon:
        if policy == "SJF":
            i = min(range(n), key=lambda j: remaining[j])
        else:
            i = ready.popleft()

        if last is not None and i != last:
            t += overhead
            switches += 1
            if t >= horizon:
                break

        run = min(remaining[i], quantum, horizon - t) if policy == "RR" else min(remaining[i], horizon - t)
        t += run
        service[i] += run
        remaining[i] -= run

        if remaining[i] <= 1e-9:
            remaining[i] = max(1e-6, rng.expovariate(1.0 / MEANS[i]))
            if policy != "SJF":
                ready.append(i)
        elif policy == "RR":
            ready.append(i)
        elif policy == "FCFS":
            ready.appendleft(i)  # non-preemptive continuation

        last = i

    total = sum(service)
    shares = [x / total for x in service] if total else [0.0] * n
    jain = total**2 / (n * sum(x*x for x in service)) if total else 0.0

    return {
        "policy": policy,
        "seed": seed,
        "throughput": total / horizon,
        "jain": jain,
        "min_share": min(shares),
        "switches": switches,
        "shares": shares,
        "total_service": total
    }

def mean_ci(values, tcrit=None):
    if tcrit is None:
        tcrit = T_CRIT_49 if len(values) == 50 else T_CRIT_19
    mean = statistics.mean(values)
    half = tcrit * statistics.stdev(values) / math.sqrt(len(values)) if len(values) > 1 else 0.0
    return mean, half

def run_main_comparison():
    results = {}
    for policy in ("FCFS", "SJF", "RR"):
        rows = [simulate(policy, seed) for seed in SEEDS]
        results[policy] = rows
    return results

def run_quantum_sensitivity():
    results = {}
    for q in (1, 2, 5, 10, 20):
        rows = [simulate("RR", seed, quantum=float(q), overhead=0.2) for seed in SENSITIVITY_SEEDS]
        results[q] = rows
    return results

def run_overhead_sensitivity():
    results = {}
    for overhead in (0.0, 0.1, 0.2, 0.5):
        rows = [simulate("RR", seed, quantum=5.0, overhead=overhead) for seed in SENSITIVITY_SEEDS]
        results[overhead] = rows
    return results

def main():
    print("=" * 80)
    print("CYBERLAB OS RESEARCH LAB: FAIRNESS VS THROUGHPUT IN CPU SCHEDULING")
    print(f"Horizon: {HORIZON:g} | Processes: {len(MEANS)} | Switch Cost: {CONTEXT_SWITCH_COST} | RR Quantum: {RR_QUANTUM}")
    print("=" * 80)

    main_res = run_main_comparison()
    print("\n--- MAIN POLICY COMPARISON (50 Seeds: 2026-2075, df=49) ---")
    for policy in ("FCFS", "SJF", "RR"):
        rows = main_res[policy]
        print(f"\nPolicy: {policy}")
        for metric in ("throughput", "jain", "min_share", "switches"):
            vals = [r[metric] for r in rows]
            if metric == "min_share":
                vals = [v * 100 for v in vals]
            m, ci = mean_ci(vals)
            unit = "%" if metric == "min_share" else ""
            print(f"  {metric:14s}: {m:.5f} +/- {ci:.5f}{unit}")

    print("\n--- ROUND ROBIN QUANTUM SENSITIVITY (20 Seeds: 3026-3046) ---")
    q_res = run_quantum_sensitivity()
    for q, rows in q_res.items():
        thr = mean_ci([r["throughput"] for r in rows])
        fair = mean_ci([r["jain"] for r in rows])
        mins = mean_ci([r["min_share"] * 100 for r in rows])
        switches = mean_ci([r["switches"] for r in rows])
        print(f"  q={q:>2}: Thr={thr[0]:.5f}+/-{thr[1]:.5f} | Jain={fair[0]:.5f}+/-{fair[1]:.5f} | MinShare={mins[0]:.3f}%+/-{mins[1]:.3f}% | Switches={switches[0]:.1f}+/-{switches[1]:.1f}")

    print("\n--- CONTEXT SWITCH COST SENSITIVITY (20 Seeds: 3026-3046) ---")
    c_res = run_overhead_sensitivity()
    for cost, rows in c_res.items():
        thr = mean_ci([r["throughput"] for r in rows])
        fair = mean_ci([r["jain"] for r in rows])
        mins = mean_ci([r["min_share"] * 100 for r in rows])
        switches = mean_ci([r["switches"] for r in rows])
        print(f"  cost={cost:.1f}: Thr={thr[0]:.5f}+/-{thr[1]:.5f} | Jain={fair[0]:.5f}+/-{fair[1]:.5f} | MinShare={mins[0]:.3f}%+/-{mins[1]:.3f}% | Switches={switches[0]:.1f}+/-{switches[1]:.1f}")

if __name__ == "__main__":
    main()
