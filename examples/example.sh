#!/bin/bash

# List available workloads
./benchmark --list

# Run using a preset config file
./benchmark --config examples/config_simd.json

# Run simd workload explicitly and write JSON output
./benchmark --workload simd --duration 10 --threads 4 --output results.json --format json

# Run io workload for 5s and write CSV output
./benchmark --workload io --duration 5 --output io_results.csv --format csv
