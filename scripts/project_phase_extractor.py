#!/usr/bin/env python3
# -*- coding: utf-8 -*-
r"""
==========================================================
 Tasqly Project Phase Extractor (Snapshot Analyzer Utility)
==========================================================

📄 Overview:
------------
Analyzes the latest `Project_Tree_*.json` snapshot under 
`reports/project_tree/snapshots/`, filters files by development phase
(e.g., Phase1), and exports categorized results (src, tests, benchmarks)
to both `.json` and `.md` outputs.

🎯 Purpose:
-----------
To provide a clean, phase-specific view of the project’s code structure,
focusing only on files relevant for development and testing workflows.

⚙️ Core Responsibilities:
--------------------------
1- Auto-detect the latest JSON snapshot under the snapshots directory.  
2- Filter files matching the requested phase (via `--phase`).  
3- Include only paths under `src/`, `tests/`, or `benchmarks/`.  
4- Generate dual-format output (.json + .md) with clear counts summary.  
5- Support custom output base path (e.g., `reports/analysis/Phase1_Files`).  

🧠 Intelligent Filtering:
-------------------------
- Phase match based on both `phase` field and tags array.  
- Ignored files: configs, build artifacts, and documentation files.  
- Clean separation between source code, tests, and benchmarks. 

🧩 Usage Example:
-----------------
py -3.13 scripts\project_phase_extractor.py reports\project_tree\snapshots --phase Phase1

# Explanation:
# 1- First argument  → snapshots directory path
# 2- Second argument → output base path (without extension)
# 3- --phase         → required, target phase to extract (e.g., Phase1)

# Output:
# - reports/tests/Phase1_Files.json
# - reports/tests/Phase1_Files.md
📦 Output Example:
reports/tests/Phase1_Files.md
reports/tests/Phase1_Files.json

📅 Version & Changelog:
v1.0 — 2025-11-11 — Initial implementation (phase-based filtering + output system)

👤 Author:
Tasqly Development Automation Team

==========================================================
"""
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import json
import re
import argparse
from datetime import datetime

# -----------------------------------------------------
# Argument parsing
# -----------------------------------------------------

def parse_args():
    p = argparse.ArgumentParser(description="Extract phase/slice-specific project files from latest snapshot JSON")
    p.add_argument("snapshots_dir", help="Path to snapshots directory")
    p.add_argument("--phase", required=True, help="Target phase (e.g., Phase1)")
    p.add_argument("--slice", help="Optional slice filter (e.g., Slice2)")
    return p.parse_args()

# -----------------------------------------------------
# Utility functions
# -----------------------------------------------------

def find_latest_snapshot(snapshots_dir):
    if not os.path.exists(snapshots_dir):
        print(f"[ERROR] Snapshots directory not found: {snapshots_dir}")
        sys.exit(2)
    json_files = [f for f in os.listdir(snapshots_dir) if f.endswith(".json") and f.startswith("Project_Tree_")]
    if not json_files:
        print("[ERROR] No snapshot JSON files found.")
        sys.exit(3)

    def extract_timestamp(fname):
        m = re.search(r"_(\d{8})_(\d{6})", fname)
        if not m:
            return datetime.min
        return datetime.strptime(m.group(1) + m.group(2), "%Y%m%d%H%M%S")

    latest = max(json_files, key=extract_timestamp)
    return os.path.join(snapshots_dir, latest)

def load_json(path):
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)

def detect_snapshot_time(snapshot_name):
    m = re.search(r'_(\d{8})_(\d{6})', snapshot_name)
    if not m:
        return "unknown"
    return datetime.strptime(m.group(1) + m.group(2), "%Y%m%d%H%M%S").strftime("%Y-%m-%d %H:%M:%S")

# -----------------------------------------------------
# Filtering logic
# -----------------------------------------------------

def filter_phase_files(snapshot_data, phase_name, slice_name=None):
    """Filter files belonging to a specific phase/slice and category (src/tests/benchmarks)."""
    src_files, test_files, bench_files = [], [], []

    skip_exts = (".json", ".html", ".md", ".xml", ".log", ".csv")
    for entry in snapshot_data:
        if not isinstance(entry, dict):
            continue
        path = entry.get("file", "")
        phase = entry.get("phase", "")
        tags = entry.get("tags", [])
        slice_field = entry.get("slice", "")

        if not path:
            continue

        # Skip reports and non-source files
        if path.startswith("reports/") or path.endswith(skip_exts):
            continue

        # Filter by phase
        if phase_name.lower() not in (phase or "").lower() and not any(phase_name.lower() in str(t).lower() for t in tags):
            continue

        # Filter by slice (optional)
        if slice_name:
            if slice_name.lower() not in (slice_field or "").lower() and not any(slice_name.lower() in str(t).lower() for t in tags):
                continue

        lower = path.lower()
        if "tests/" in lower:
            test_files.append(path)
        elif "benchmarks/" in lower:
            bench_files.append(path)
        elif lower.startswith("src/"):
            src_files.append(path)
        else:
            continue

    return {
        "src": sorted(set(src_files)),
        "tests": sorted(set(test_files)),
        "benchmarks": sorted(set(bench_files))
    }

# -----------------------------------------------------
# Output generation helpers
# -----------------------------------------------------

def make_table(headers, rows):
    widths = [max(len(str(cell)) for cell in col) for col in zip(headers, *rows)]
    hdr = "| " + " | ".join(headers[i].ljust(widths[i]) for i in range(len(headers))) + " |"
    sep = "|-" + "-|-".join("-" * widths[i] for i in range(len(headers))) + "-|"
    lines = [hdr, sep]
    for row in rows:
        line = "| " + " | ".join(str(row[i]).ljust(widths[i]) for i in range(len(headers))) + " |"
        lines.append(line)
    return "\n".join(lines)

# -----------------------------------------------------
# Output generation
# -----------------------------------------------------

def write_json(out_path, data):
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    with open(out_path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=4, ensure_ascii=False)

def write_markdown(out_path, data):
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    with open(out_path, "w", encoding="utf-8") as f:
        f.write(f"# 🧩 Project Files — {data['metadata']['phase']}\n\n")
        f.write(f"**Snapshot:** {data['metadata']['snapshot']}\n\n")
        f.write(f"**Snapshot time:** {data['metadata']['snapshot_time']}\n\n")
        if data['metadata'].get('slice'):
            f.write(f"**Slice:** {data['metadata']['slice']}\n\n")
        f.write(f"**Generated:** {data['metadata']['generated_time']}\n\n")
        f.write("---\n\n")

        # Summary table
        f.write("### 📊 Summary\n\n")
        headers = ["Category", "Count"]
        rows = [
            ["Src", data["counts"]["src"]],
            ["Tests", data["counts"]["tests"]],
            ["Benchmarks", data["counts"]["benchmarks"]],
            ["Total", data["counts"]["total"]],
        ]
        f.write(make_table(headers, rows))
        f.write("\n\n---\n\n")

        # Detailed tables
        for category, files in data["files"].items():
            if not files:
                continue
            icon = {"src": "🧱", "tests": "🧪", "benchmarks": "🚀"}.get(category, "📁")
            f.write(f"### {icon} {category.upper()} ({len(files)})\n\n")
            headers = ["File", "Path"]
            rows = []
            for fp in files:
                filename = os.path.basename(fp)
                rows.append([filename, fp])
            f.write(make_table(headers, rows))
            f.write("\n\n")

# -----------------------------------------------------
# Main execution
# -----------------------------------------------------

def main():
    args = parse_args()
    snapshots_dir = args.snapshots_dir
    phase_name = args.phase
    slice_name = args.slice

    latest_snapshot_path = find_latest_snapshot(snapshots_dir)
    snapshot_name = os.path.basename(latest_snapshot_path)
    snapshot_time = detect_snapshot_time(snapshot_name)

    print(f"[✓] Snapshot detected: {snapshot_name}")
    print(f"[✓] Phase filter: {phase_name}")
    if slice_name:
        print(f"[✓] Slice filter: {slice_name}")

    snapshot_data = load_json(latest_snapshot_path)
    filtered = filter_phase_files(snapshot_data, phase_name, slice_name)

    total_src = len(filtered["src"])
    total_tests = len(filtered["tests"])
    total_bench = len(filtered["benchmarks"])
    total = total_src + total_tests + total_bench

    report = {
        "metadata": {
            "phase": phase_name,
            "slice": slice_name or None,
            "snapshot": snapshot_name,
            "snapshot_time": snapshot_time,
            "generated_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        },
        "counts": {
            "src": total_src,
            "tests": total_tests,
            "benchmarks": total_bench,
            "total": total
        },
        "files": filtered
    }

    suffix = f"{phase_name}_Files" if not slice_name else f"{phase_name}_{slice_name}_Files"
    output_dir = os.path.join("reports", "tests", f"{phase_name.lower()}_extracted")
    os.makedirs(output_dir, exist_ok=True)

    out_json = os.path.join(output_dir, f"{suffix}.json")
    out_md = os.path.join(output_dir, f"{suffix}.md")

    write_json(out_json, report)
    write_markdown(out_md, report)

    print("\n[✓] Output generated successfully!")
    print(f"    Folder : {output_dir}")
    print(f"    Files  : {os.path.basename(out_json)}, {os.path.basename(out_md)}")
    print(f"    Totals : {total_src} src, {total_tests} tests, {total_bench} benchmarks → {total}\n")

if __name__ == "__main__":
    main()
