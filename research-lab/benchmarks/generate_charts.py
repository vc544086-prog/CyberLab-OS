#!/usr/bin/env python3
"""
CyberLab OS Research Lab: Plot Generator
Recreates Figures 1-7 from Vishnu C's Research Paper:
'Fairness vs Throughput in CPU Scheduling'
"""

import sys
import os
from pathlib import Path
import matplotlib.pyplot as plt

# Add src to sys.path
sys.path.insert(0, str(Path(__file__).parent.parent / "src"))
from cpu_scheduling_fairness_simulator import (
    simulate, mean_ci, run_main_comparison, run_quantum_sensitivity,
    run_overhead_sensitivity
)

def setup_style():
    plt.style.use('seaborn-v0_8-whitegrid' if 'seaborn-v0_8-whitegrid' in plt.style.available else 'default')
    plt.rcParams['font.sans-serif'] = 'DejaVu Sans'
    plt.rcParams['axes.edgecolor'] = '#cccccc'
    plt.rcParams['axes.linewidth'] = 0.8
    plt.rcParams['grid.color'] = '#e5e5e5'
    plt.rcParams['grid.linestyle'] = '--'

def generate_all_plots(fig_dir: Path, screenshots_dir: Path):
    fig_dir.mkdir(parents=True, exist_ok=True)
    screenshots_dir.mkdir(parents=True, exist_ok=True)
    setup_style()

    print("[*] Running simulation to extract plot data...")
    main_res = run_main_comparison()
    q_res = run_quantum_sensitivity()
    c_res = run_overhead_sensitivity()

    policies = ["FCFS", "SJF", "Round Robin (q=5)"]
    key_map = ["FCFS", "SJF", "RR"]

    # Extract Main Comparison Metrics
    throughputs = [mean_ci([r["throughput"] for r in main_res[k]]) for k in key_map]
    jain_vals = [mean_ci([r["jain"] for r in main_res[k]]) for k in key_map]
    min_shares = [mean_ci([r["min_share"] * 100 for r in main_res[k]]) for k in key_map]
    switches = [mean_ci([r["switches"] for r in main_res[k]]) for k in key_map]

    color = "#1f77b4"

    # Figure 1: Mean useful CPU throughput
    fig, ax = plt.subplots(figsize=(7, 4.5), dpi=150)
    bars = ax.bar(policies, [t[0] for t in throughputs], yerr=[t[1] for t in throughputs],
                  capsize=4, color=color, edgecolor='#0f4c81', width=0.6)
    ax.set_ylabel("Useful work / time horizon", fontsize=11)
    ax.set_title("Mean useful CPU throughput (50 seeds)", fontsize=12, fontweight='bold', pad=12)
    ax.set_ylim(0, 1.05)
    for bar in bars:
        height = bar.get_height()
        ax.annotate(f'{height:.4f}', xy=(bar.get_x() + bar.get_width() / 2, height / 2),
                    xytext=(0, 0), textcoords="offset points", ha='center', va='center', color='white', fontweight='bold', fontsize=9)
    plt.tight_layout()
    p1 = fig_dir / "figure1_throughput.png"
    fig.savefig(p1)
    fig.savefig(screenshots_dir / "figure1_throughput.png")
    plt.close(fig)

    # Figure 2: Jain fairness index
    fig, ax = plt.subplots(figsize=(7, 4.5), dpi=150)
    bars = ax.bar(policies, [j[0] for j in jain_vals], yerr=[j[1] for j in jain_vals],
                  capsize=4, color=color, edgecolor='#0f4c81', width=0.6)
    ax.set_ylabel("Jain fairness index (1 = equal shares)", fontsize=11)
    ax.set_title("CPU allocation fairness (50 seeds)", fontsize=12, fontweight='bold', pad=12)
    ax.set_ylim(0, 1.0)
    for bar in bars:
        height = bar.get_height()
        ax.annotate(f'{height:.4f}', xy=(bar.get_x() + bar.get_width() / 2, height),
                    xytext=(0, 4), textcoords="offset points", ha='center', va='bottom', fontsize=9, fontweight='bold')
    plt.tight_layout()
    p2 = fig_dir / "figure2_jain_fairness.png"
    fig.savefig(p2)
    fig.savefig(screenshots_dir / "figure2_jain_fairness.png")
    plt.close(fig)

    # Figure 3: Minimum CPU share received (%)
    fig, ax = plt.subplots(figsize=(7, 4.5), dpi=150)
    bars = ax.bar(policies, [m[0] for m in min_shares], yerr=[m[1] for m in min_shares],
                  capsize=4, color=color, edgecolor='#0f4c81', width=0.6)
    ax.set_ylabel("Minimum process share (%)", fontsize=11)
    ax.set_title("Minimum CPU share received (50 seeds)", fontsize=12, fontweight='bold', pad=12)
    ax.set_ylim(0, 7.5)
    for bar in bars:
        height = bar.get_height()
        ax.annotate(f'{height:.2f}%', xy=(bar.get_x() + bar.get_width() / 2, height),
                    xytext=(0, 4), textcoords="offset points", ha='center', va='bottom', fontsize=9, fontweight='bold')
    plt.tight_layout()
    p3 = fig_dir / "figure3_min_cpu_share.png"
    fig.savefig(p3)
    fig.savefig(screenshots_dir / "figure3_min_cpu_share.png")
    plt.close(fig)

    # Figure 4: Context-switch count
    fig, ax = plt.subplots(figsize=(7, 4.5), dpi=150)
    bars = ax.bar(policies, [s[0] for s in switches], yerr=[s[1] for s in switches],
                  capsize=4, color=color, edgecolor='#0f4c81', width=0.6)
    ax.set_ylabel("Context switches per run", fontsize=11)
    ax.set_title("Context-switch count (50 seeds)", fontsize=12, fontweight='bold', pad=12)
    for bar in bars:
        height = bar.get_height()
        ax.annotate(f'{int(height)}', xy=(bar.get_x() + bar.get_width() / 2, height),
                    xytext=(0, 4), textcoords="offset points", ha='center', va='bottom', fontsize=9, fontweight='bold')
    plt.tight_layout()
    p4 = fig_dir / "figure4_context_switches.png"
    fig.savefig(p4)
    fig.savefig(screenshots_dir / "figure4_context_switches.png")
    plt.close(fig)

    # Figure 5: RR quantum sensitivity - Throughput
    quanta = [1, 2, 5, 10, 20]
    q_thrs = [mean_ci([r["throughput"] for r in q_res[q]])[0] for q in quanta]
    fig, ax = plt.subplots(figsize=(7, 4.5), dpi=150)
    ax.plot(quanta, q_thrs, marker='o', linewidth=2, color=color, markersize=7)
    ax.set_xlabel("Time quantum", fontsize=11)
    ax.set_ylabel("Useful CPU throughput", fontsize=11)
    ax.set_title("Round Robin quantum sensitivity (20 seeds)", fontsize=12, fontweight='bold', pad=12)
    for x, y in zip(quanta, q_thrs):
        ax.annotate(f'{y:.4f}', xy=(x, y), xytext=(0, 7), textcoords="offset points", ha='center', fontsize=8.5)
    plt.tight_layout()
    p5 = fig_dir / "figure5_rr_quantum_throughput.png"
    fig.savefig(p5)
    fig.savefig(screenshots_dir / "figure5_rr_quantum_throughput.png")
    plt.close(fig)

    # Figure 6: RR quantum sensitivity - Fairness
    q_fairs = [mean_ci([r["jain"] for r in q_res[q]])[0] for q in quanta]
    fig, ax = plt.subplots(figsize=(7, 4.5), dpi=150)
    ax.plot(quanta, q_fairs, marker='o', linewidth=2, color=color, markersize=7)
    ax.set_xlabel("Time quantum", fontsize=11)
    ax.set_ylabel("Jain fairness index", fontsize=11)
    ax.set_title("Round Robin quantum sensitivity (20 seeds)", fontsize=12, fontweight='bold', pad=12)
    for x, y in zip(quanta, q_fairs):
        ax.annotate(f'{y:.4f}', xy=(x, y), xytext=(0, 7), textcoords="offset points", ha='center', fontsize=8.5)
    plt.tight_layout()
    p6 = fig_dir / "figure6_rr_quantum_fairness.png"
    fig.savefig(p6)
    fig.savefig(screenshots_dir / "figure6_rr_quantum_fairness.png")
    plt.close(fig)

    # Figure 7: Context-switch overhead sensitivity - Throughput
    costs = [0.0, 0.1, 0.2, 0.5]
    c_thrs = [mean_ci([r["throughput"] for r in c_res[c]])[0] for c in costs]
    fig, ax = plt.subplots(figsize=(7, 4.5), dpi=150)
    ax.plot(costs, c_thrs, marker='o', linewidth=2, color=color, markersize=7)
    ax.set_xlabel("Context-switch cost (time units)", fontsize=11)
    ax.set_ylabel("Useful CPU throughput", fontsize=11)
    ax.set_title("Context-switch overhead sensitivity (20 seeds)", fontsize=12, fontweight='bold', pad=12)
    for x, y in zip(costs, c_thrs):
        ax.annotate(f'{y:.4f}', xy=(x, y), xytext=(0, 7), textcoords="offset points", ha='center', fontsize=8.5)
    plt.tight_layout()
    p7 = fig_dir / "figure7_switch_cost_throughput.png"
    fig.savefig(p7)
    fig.savefig(screenshots_dir / "figure7_switch_cost_throughput.png")
    plt.close(fig)

    print("[+] All 7 Research Paper Figures successfully generated!")

if __name__ == "__main__":
    base_reports = Path(__file__).parent.parent / "reports" / "figures"
    base_screens = Path(__file__).parent.parent.parent / "screenshots"
    generate_all_plots(base_reports, base_screens)
