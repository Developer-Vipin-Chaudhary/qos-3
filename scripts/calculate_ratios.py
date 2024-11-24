#!/usr/bin/env python3

import numpy as np
import pandas as pd


def calculate_buffer_ratios():
    # Read request history
    df = pd.read_csv("request_history.txt", names=["cpu_id", "timestamp"])

    # Calculate request frequency for each CPU
    request_counts = df["cpu_id"].value_counts()

    # Initialize ratios for all 64 CPUs
    ratios = np.zeros(64)

    # Calculate ratios based on request frequency
    total_requests = request_counts.sum()
    for cpu_id in range(64):
        if cpu_id in request_counts:
            ratios[cpu_id] = request_counts[cpu_id] / total_requests
        else:
            ratios[cpu_id] = 0.01  # Minimum allocation for idle CPUs

    # Normalize to ensure sum is 1
    ratios = ratios / ratios.sum()

    # Save ratios
    np.savetxt("buffer_ratios.txt", ratios)


if __name__ == "__main__":
    calculate_buffer_ratios()
