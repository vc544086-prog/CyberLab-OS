#!/usr/bin/env python3
"""
CyberLab OS Benchmark Suite: Automated Experiment Runner
Generates CSV and JSON reports for 'Fairness vs Throughput in CPU Scheduling'
"""

import sys
import os
import json
import csv
import argparse
from pathlib import Path

# Add src to sys.path
sys.path.insert(0, str(Path(__file__).parent.parent / "src"))
from cpu_scheduling_fairness_simulator import (
    simulate, mean_ci, run_main_comparison, run_quantum_sensitivity,
    run_overhead_sensitivity, MEANS, HORIZON, CONTEXT_SWITCH_COST, RR_QUANTUM,
    SEEDS, SENSITIVITY_SEEDS
)

def run_and_export(output_dir: Path):
    output_dir.mkdir(parents=True, exist_ok=True)
    print(f"[*] Executing benchmarks... Output directory: {output_dir}")

    # 1. Main comparison
    main_res = run_main_comparison()
    summary = {"parameters": {
        "horizon": HORIZON,
        "processes": len(MEANS),
        "burst_means": MEANS,
        "context_switch_cost": CONTEXT_SWITCH_COST,
        "default_quantum": RR_QUANTUM,
        "seeds_count": len(SEEDS),
        "sensitivity_seeds_count": len(SENSITIVITY_SEEDS)
    }, "policies": {}}

    # Write raw data CSV
    csv_raw = output_dir / "fairness_vs_throughput_raw.csv"
    with open(csv_raw, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["policy", "seed", "useful_throughput", "jain_fairness", "min_cpu_share_pct", "context_switches", "total_service"])
        for policy, rows in main_res.items():
            for r in rows:
                writer.writerow([r["policy"], r["seed"], f"{r['throughput']:.6f}", f"{r['jain']:.6f}", f"{r['min_share']*100:.6f}", r["switches"], f"{r['total_service']:.4f}"])

    # Compute summary statistics
    for policy, rows in main_res.items():
        thr_m, thr_ci = mean_ci([r["throughput"] for r in rows])
        jain_m, jain_ci = mean_ci([r["jain"] for r in rows])
        min_m, min_ci = mean_ci([r["min_share"] * 100 for r in rows])
        sw_m, sw_ci = mean_ci([r["switches"] for r in rows])
        summary["policies"][policy] = {
            "useful_throughput": {"mean": thr_m, "ci95": thr_ci},
            "jain_fairness": {"mean": jain_m, "ci95": jain_ci},
            "min_cpu_share_pct": {"mean": min_m, "ci95": min_ci},
            "context_switches": {"mean": sw_m, "ci95": sw_ci}
        }

    # Write summary CSV
    csv_summary = output_dir / "fairness_vs_throughput_summary.csv"
    with open(csv_summary, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["Policy", "Useful Throughput (mean)", "Throughput 95% CI", "Jain Fairness (mean)", "Jain 95% CI", "Min CPU Share % (mean)", "Min Share 95% CI", "Context Switches (mean)", "Switches 95% CI"])
        for p, data in summary["policies"].items():
            writer.writerow([
                p,
                f"{data['useful_throughput']['mean']:.4f}", f"{data['useful_throughput']['ci95']:.4f}",
                f"{data['jain_fairness']['mean']:.4f}", f"{data['jain_fairness']['ci95']:.4f}",
                f"{data['min_cpu_share_pct']['mean']:.4f}", f"{data['min_cpu_share_pct']['ci95']:.4f}",
                f"{data['context_switches']['mean']:.1f}", f"{data['context_switches']['ci95']:.1f}"
            ])

    # 2. Sensitivity - Quantum
    q_res = run_quantum_sensitivity()
    q_summary = {}
    csv_q = output_dir / "sensitivity_quantum.csv"
    with open(csv_q, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["Quantum", "Useful Throughput (mean)", "Throughput CI", "Jain Fairness (mean)", "Jain CI", "Min Share % (mean)", "Min Share CI", "Context Switches (mean)", "Switches CI"])
        for q, rows in q_res.items():
            thr_m, thr_ci = mean_ci([r["throughput"] for r in rows])
            jain_m, jain_ci = mean_ci([r["jain"] for r in rows])
            min_m, min_ci = mean_ci([r["min_share"] * 100 for r in rows])
            sw_m, sw_ci = mean_ci([r["switches"] for r in rows])
            q_summary[str(q)] = {
                "throughput": [thr_m, thr_ci], "jain": [jain_m, jain_ci],
                "min_share": [min_m, min_ci], "switches": [sw_m, sw_ci]
            }
            writer.writerow([q, f"{thr_m:.4f}", f"{thr_ci:.4f}", f"{jain_m:.4f}", f"{jain_ci:.4f}", f"{min_m:.2f}", f"{min_ci:.2f}", f"{sw_m:.1f}", f"{sw_ci:.1f}"])
    summary["quantum_sensitivity"] = q_summary

    # 3. Sensitivity - Switch Cost
    c_res = run_overhead_sensitivity()
    c_summary = {}
    csv_c = output_dir / "sensitivity_switch_cost.csv"
    with open(csv_c, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["Switch Cost", "Useful Throughput (mean)", "Throughput CI", "Jain Fairness (mean)", "Jain CI", "Min Share % (mean)", "Min Share CI", "Context Switches (mean)", "Switches CI"])
        for cost, rows in c_res.items():
            thr_m, thr_ci = mean_ci([r["throughput"] for r in rows])
            jain_m, jain_ci = mean_ci([r["jain"] for r in rows])
            min_m, min_ci = mean_ci([r["min_share"] * 100 for r in rows])
            sw_m, sw_ci = mean_ci([r["switches"] for r in rows])
            c_summary[str(cost)] = {
                "throughput": [thr_m, thr_ci], "jain": [jain_m, jain_ci],
                "min_share": [min_m, min_ci], "switches": [sw_m, sw_ci]
            }
            writer.writerow([cost, f"{thr_m:.4f}", f"{thr_ci:.4f}", f"{jain_m:.4f}", f"{jain_ci:.4f}", f"{min_m:.2f}", f"{min_ci:.2f}", f"{sw_m:.1f}", f"{sw_ci:.1f}"])
    summary["switch_cost_sensitivity"] = c_summary

    # Write combined JSON
    json_path = output_dir / "fairness_vs_throughput_results.json"
    with open(json_path, "w", encoding="utf-8") as f:
        json.dump(summary, f, indent=2)

    print(f"[+] Successfully wrote reports to:\n  - {csv_summary}\n  - {csv_raw}\n  - {csv_q}\n  - {csv_c}\n  - {json_path}")
    return summary

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="CyberLab OS Research Lab Benchmarks")
    parser.add_argument("--outdir", default=str(Path(__file__).parent.parent / "reports"), help="Output directory")
    args = parser.parse_args()
    run_and_export(Path(args.outdir))
