#!/bin/bash

# Create temporary files for storing output
GPU_OUTPUT="gpu_power_usage.csv"
CPU_OUTPUT="cpu_usage.txt"

# Function to monitor GPU power usage
monitor_gpu() {
    while true; do
        TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S')
        GPU0_POWER=$(nvidia-smi -i 0 --query-gpu=power.draw --format=csv,noheader,nounits)
        GPU1_POWER=$(nvidia-smi -i 1 --query-gpu=power.draw --format=csv,noheader,nounits)
        echo "$TIMESTAMP,$GPU0_POWER,$GPU1_POWER" >> $GPU_OUTPUT
        sleep 0.1
    done
}

# Function to monitor CPU usage
monitor_cpu() {
    VECTOR_ADD_PID=$1
    while [ -e /proc/$VECTOR_ADD_PID ]; do
        pidstat -u -p $VECTOR_ADD_PID 1 1 >> $CPU_OUTPUT
        sleep 0.1
    done
}

# Initialize the GPU output file with headers
echo "timestamp,gpu0_power_w,gpu1_power_w" > $GPU_OUTPUT

# Start monitoring GPU power usage
monitor_gpu &
GPU_MONITOR_PID=$!

# Run the vector_add program in the background
./vector_add &
VECTOR_ADD_PID=$!
echo "Vector Add PID: $VECTOR_ADD_PID"

# Start monitoring CPU usage for the vector_add process
monitor_cpu $VECTOR_ADD_PID &

# Wait for the vector_add program to finish
wait $VECTOR_ADD_PID

# Stop monitoring
kill $GPU_MONITOR_PID

echo "Monitoring completed. GPU power usage saved in $GPU_OUTPUT and CPU usage saved in $CPU_OUTPUT."
