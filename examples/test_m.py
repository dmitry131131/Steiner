#!/usr/bin/env python3
import subprocess
import json
import time
import os
import sys
from pathlib import Path

PROGRAM = "../build/Release/bin/steiner"        
TEST_DIR = "."              
START_NUM = 5
END_NUM = 30
INPUT_SUFFIX = "_0000.json"
OUTPUT_SUFFIX = "_out.json"

def mht_distance(p1, p2):
    return abs(p1['x'] - p2['x']) + abs(p1['y'] - p2['y'])

def compute_total_length(nodes, edges):
    coords = {node['id']: node for node in nodes}
    total = 0
    for edge in edges:
        u = coords[edge['vertices'][0]]
        v = coords[edge['vertices'][1]]
        total += mht_distance(u, v)
    return total

def process_file(filepath):
    print(f"Processing {filepath.name}...", end=' ', flush=True)
    start_time = time.perf_counter()
    
    try:
        result = subprocess.run(
            [PROGRAM, "-m", str(filepath)],
            capture_output=True,
            text=True,
            timeout=300 
        )
        elapsed = time.perf_counter() - start_time
        
        if result.returncode != 0:
            print(f"FAILED (exit {result.returncode})")
            return {"file": filepath.name, "status": "error", "time": elapsed, "error": result.stderr.strip()}
        
        out_path = filepath.with_name(filepath.stem + OUTPUT_SUFFIX)
        if not out_path.exists():
            print("FAILED (no output file)")
            return {"file": filepath.name, "status": "error", "time": elapsed, "error": "Output file not found"}
        
        with open(out_path, 'r') as f:
            data = json.load(f)
        
        nodes = data.get('node', [])
        edges = data.get('edge', [])
        
        num_terminals = sum(1 for n in nodes if n.get('type') == 't')
        num_steiners = sum(1 for n in nodes if n.get('type') == 's')
        total_length = compute_total_length(nodes, edges)
        num_edges = len(edges)
        
        print(f"OK ({elapsed:.3f}s) length={total_length} edges={num_edges} steiners={num_steiners}")
        return {
            "file": filepath.name,
            "status": "ok",
            "time": elapsed,
            "length": total_length,
            "edges": num_edges,
            "terminals": num_terminals,
            "steiners": num_steiners
        }
    except subprocess.TimeoutExpired:
        print("TIMEOUT")
        return {"file": filepath.name, "status": "timeout", "time": 300.0}
    except Exception as e:
        print(f"ERROR: {e}")
        return {"file": filepath.name, "status": "error", "time": time.perf_counter()-start_time, "error": str(e)}

def main():
    if not os.path.isfile(PROGRAM):
        print(f"Error: Program '{PROGRAM}' not found.")
        sys.exit(1)
    
    results = []
    for num in range(START_NUM, END_NUM + 1):
        filename = f"{num:04d}{INPUT_SUFFIX}"
        filepath = Path(TEST_DIR) / filename
        if not filepath.exists():
            print(f"Warning: {filename} not found, skipping.")
            continue
        res = process_file(filepath)
        results.append(res)
    
    print("\n" + "="*60)
    print("Summary:")
    print(f"{'File':<15} {'Status':<8} {'Time(s)':<8} {'Length':<8} {'Edges':<6} {'Steiners':<8}")
    print("-"*60)
    for r in results:
        if r['status'] == 'ok':
            print(f"{r['file']:<15} {'OK':<8} {r['time']:<8.3f} {r['length']:<8} {r['edges']:<6} {r['steiners']:<8}")
        else:
            print(f"{r['file']:<15} {r['status']:<8} {r.get('time', 0):<8.3f} {'-':<8} {'-':<6} {'-':<8}")
    
    import csv
    with open('test_results.csv', 'w', newline='') as csvfile:
        fieldnames = ['file', 'status', 'time', 'length', 'edges', 'terminals', 'steiners', 'error']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for r in results:
            writer.writerow({k: r.get(k, '') for k in fieldnames})
    print(f"\nDetailed results saved to test_results.csv")

if __name__ == "__main__":
    main()