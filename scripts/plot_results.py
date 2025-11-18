#!/usr/bin/env python3
"""
Simple plotting script for PulseBench JSON/CSV outputs.
Usage:
  python3 scripts/plot_results.py results.json
  python3 scripts/plot_results.py results.csv
"""
import sys
import os
import json
import csv
import argparse
import numpy as np
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='Plot PulseBench results')
parser.add_argument('file', help='JSON or CSV result file')
args = parser.parse_args()

path = args.file
if not os.path.exists(path):
    print('File not found:', path)
    sys.exit(1)

ext = os.path.splitext(path)[1].lower()

if ext == '.json':
    with open(path,'r') as f:
        j = json.load(f)
    stats = j.get('stats', {})
    percentiles = stats.get('percentiles', {})
    hist = j.get('histogram_bins', [])

    print('Workload:', j.get('workload'))
    print('Threads:', j.get('threads'))
    print('Duration (s):', j.get('duration_seconds'))
    print('Throughput (batches/s):', j.get('throughput_batches_per_s'))


    if hist:
        plt.figure()
        plt.bar(range(len(hist)), hist)
        plt.title('Sample histogram bins')
        plt.xlabel('Bin')
        plt.ylabel('Count')


    if percentiles:
        keys = sorted([float(k) for k in percentiles.keys()])
        vals = [percentiles[str(int(k))] for k in keys]
        plt.figure()
        plt.plot(keys, vals, marker='o')
        plt.title('Percentiles (ms)')
        plt.xlabel('Percentile')
        plt.ylabel('Value (ms)')

    plt.show()

elif ext == '.csv':
    with open(path,'r') as f:
        reader = csv.DictReader(f)
        rows = list(reader)
    if not rows:
        print('No rows in CSV')
        sys.exit(1)
    row = rows[0]
    print('Workload:', row.get('workload'))
    print('Threads:', row.get('threads'))
    print('Duration:', row.get('duration_seconds'))
    print('Throughput (batches/s):', row.get('throughput_bps'))

    sys.exit(0)

else:
    print('Unsupported file type:', ext)
    sys.exit(1)
