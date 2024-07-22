#!/bin/bash

mkdir -p monitor_output

# Output file for storing GPU power usage data
GPU_OUTPUT="monitor_output/gpu_power_usage.csv"

# Function to monitor GPU power usage
monitor_gpu() {
    echo "timestamp,gpu0_power_w,gpu1_power_w" > $GPU_OUTPUT
    while true; do
        TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S')
        GPU0_POWER=$(nvidia-smi -i 0 --query-gpu=power.draw --format=csv,noheader,nounits)
        GPU1_POWER=$(nvidia-smi -i 1 --query-gpu=power.draw --format=csv,noheader,nounits)
        echo "$TIMESTAMP,$GPU0_POWER,$GPU1_POWER" >> $GPU_OUTPUT
        sleep 0.1
    done
}

# Start monitoring GPU power usage
monitor_gpu
