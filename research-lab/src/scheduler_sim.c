/*
 * CyberLab OS - Academic Research Experiment Engine (Native C Implementation)
 * Topic: Fairness vs Throughput in CPU Scheduling
 * Author: Vishnu C (25BTRCL282) - JAIN (Deemed-to-be University)
 *
 * Implements FCFS, SJF, and Round Robin scheduler models with exact exponential
 * burst generation, context switch penalties, and statistical aggregation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>

#define PROCESS_COUNT 8
#define DEFAULT_HORIZON 10000.0
#define DEFAULT_OVERHEAD 0.2
#define DEFAULT_QUANTUM 5.0
#define NUM_SEEDS 50

static const double BURST_MEANS[PROCESS_COUNT] = {2.0, 3.0, 4.0, 6.0, 8.0, 12.0, 18.0, 25.0};

/* Simple LCG or standard rand wrapper seeded per run */
static unsigned long g_seed = 1;
static void sim_srand(unsigned long s) { g_seed = s; }
static double sim_rand_uniform(void) {
    /* 64-bit LCG */
    g_seed = g_seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return (double)(g_seed >> 11) * (1.0 / 9007199254740992.0);
}

static double sim_expovariate(double lambd) {
    double u = sim_rand_uniform();
    while (u <= 1e-12) u = sim_rand_uniform();
    return -log(1.0 - u) / lambd;
}

typedef struct {
    double throughput;
    double jain_fairness;
    double min_share_pct;
    int switches;
    double total_service;
} SimResult;

SimResult run_simulation(const char* policy, unsigned long seed, double horizon, double overhead, double quantum) {
    sim_srand(seed);

    double remaining[PROCESS_COUNT];
    double service[PROCESS_COUNT];
    int ready_queue[PROCESS_COUNT * 4];
    int q_head = 0, q_tail = 0;

    for (int i = 0; i < PROCESS_COUNT; i++) {
        double b = sim_expovariate(1.0 / BURST_MEANS[i]);
        remaining[i] = (b < 1e-6) ? 1e-6 : b;
        service[i] = 0.0;
        ready_queue[q_tail++] = i;
    }

    double t = 0.0;
    int last = -1;
    int switches = 0;

    while (t < horizon) {
        int i;
        if (strcmp(policy, "SJF") == 0) {
            i = 0;
            double min_rem = remaining[0];
            for (int j = 1; j < PROCESS_COUNT; j++) {
                if (remaining[j] < min_rem) {
                    min_rem = remaining[j];
                    i = j;
                }
            }
        } else {
            /* Queue pop */
            if (q_head == q_tail) {
                for (int j = 0; j < PROCESS_COUNT; j++) ready_queue[q_tail++] = j;
            }
            i = ready_queue[q_head++];
            if (q_head > PROCESS_COUNT * 2) {
                int count = q_tail - q_head;
                for (int k = 0; k < count; k++) ready_queue[k] = ready_queue[q_head + k];
                q_head = 0;
                q_tail = count;
            }
        }

        if (last != -1 && i != last) {
            t += overhead;
            switches++;
            if (t >= horizon) break;
        }

        double rem_time = horizon - t;
        double run = remaining[i];
        if (strcmp(policy, "RR") == 0) {
            if (quantum < run) run = quantum;
        }
        if (run > rem_time) run = rem_time;

        t += run;
        service[i] += run;
        remaining[i] -= run;

        if (remaining[i] <= 1e-9) {
            double b = sim_expovariate(1.0 / BURST_MEANS[i]);
            remaining[i] = (b < 1e-6) ? 1e-6 : b;
            if (strcmp(policy, "SJF") != 0) {
                ready_queue[q_tail++] = i;
            }
        } else if (strcmp(policy, "RR") == 0) {
            ready_queue[q_tail++] = i;
        } else if (strcmp(policy, "FCFS") == 0) {
            /* non-preemptive continuation */
            ready_queue[--q_head] = i;
        }

        last = i;
    }

    double total = 0.0;
    for (int i = 0; i < PROCESS_COUNT; i++) total += service[i];

    double sum_sq = 0.0;
    double min_share = 1.0;
    for (int i = 0; i < PROCESS_COUNT; i++) {
        double share = (total > 0.0) ? (service[i] / total) : 0.0;
        sum_sq += share * share;
        if (share < min_share) min_share = share;
    }

    double jain = 0.0;
    if (sum_sq > 0.0) {
        jain = 1.0 / (PROCESS_COUNT * sum_sq);
    }

    SimResult res;
    res.throughput = total / horizon;
    res.jain_fairness = jain;
    res.min_share_pct = min_share * 100.0;
    res.switches = switches;
    res.total_service = total;
    return res;
}

int main(int argc, char** argv) {
    const char* policy = "RR";
    double quantum = DEFAULT_QUANTUM;
    double overhead = DEFAULT_OVERHEAD;
    double horizon = DEFAULT_HORIZON;

    if (argc > 1) policy = argv[1];
    if (argc > 2) quantum = atof(argv[2]);
    if (argc > 3) overhead = atof(argv[3]);
    if (argc > 4) horizon = atof(argv[4]);

    printf("CyberLab OS Research Simulator (C Native)\n");
    printf("Evaluating Policy: %s | Quantum: %.2f | Overhead: %.2f | Horizon: %.0f\n",
           policy, quantum, overhead, horizon);

    double sum_thr = 0.0, sum_jain = 0.0, sum_min = 0.0, sum_sw = 0.0;
    for (int s = 0; s < NUM_SEEDS; s++) {
        SimResult r = run_simulation(policy, 2026 + s, horizon, overhead, quantum);
        sum_thr += r.throughput;
        sum_jain += r.jain_fairness;
        sum_min += r.min_share_pct;
        sum_sw += r.switches;
    }

    printf("Results over %d independent runs:\n", NUM_SEEDS);
    printf("  Mean Useful Throughput: %.5f\n", sum_thr / NUM_SEEDS);
    printf("  Mean Jain Fairness:    %.5f\n", sum_jain / NUM_SEEDS);
    printf("  Mean Min CPU Share:    %.3f%%\n", sum_min / NUM_SEEDS);
    printf("  Mean Context Switches:  %.1f\n", sum_sw / NUM_SEEDS);

    return 0;
}
