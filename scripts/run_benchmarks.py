#!/usr/bin/env python3
# =============================================================================
# Tasqly Benchmark Runner (Unified Cross-Platform Script)
# =============================================================================
# 📌 Purpose:
#   Run Google Benchmark suite for the Tasqly project, normalize results,
#   and generate reports in Markdown, HTML, and JSON formats.
#
#   This script replaces:
#     - run_bench_mingw_release.py
#     - run_bench_msvc_release.py
#
# -----------------------------------------------------------------------------
# ⚙️ Usage (Local):
#   py -3.13 scripts/run_benchmarks.py --phase Phase0 --compiler msvc
#   py -3.13 scripts/run_benchmarks.py --phase Phase0 --compiler mingw
#   py -3.13 scripts/run_benchmarks.py --phase Phase0 --compiler gcc
#   py -3.13 scripts/run_benchmarks.py --phase Phase0 --compiler clang
#
# ⚙️ Usage (CI/CD):
#   - Executed after build stage, using the compiled benchmark runner.
#   - Example GitHub Actions step:
#       - name: Run Benchmarks
#         run: |
#           python scripts/run_benchmarks.py \
#             --phase Phase0 \
#             --compiler ${{ matrix.compiler }} \
#             --quiet \
#             --timeout 600 \
#             --format all
#
# -----------------------------------------------------------------------------
# 📂 Output Structure:
#   reports/benchmarks/{phase}_perf_{compiler}_benchmarks_reports/
#       raw/        → JSON (raw benchmark results)
#       markdown/   → Markdown report(s)
#       dashboard/  → HTML interactive dashboards
#
# Example:
#   reports/benchmarks/phase0_perf_msvc_benchmarks_reports/
#       raw/Phase0_2025-09-28_bench_results.json
#       markdown/Phase0_2025-09-28_report.md
#       dashboard/Phase0_2025-09-28_dashboard.html
#
# -----------------------------------------------------------------------------
# Features:
#   - Cross-platform: Windows (MSVC/MinGW), Linux (GCC/Clang)
#   - Normalizes Google Benchmark units to milliseconds
#   - Generates detailed reports in:
#       * Markdown → aligned tables, grouped sections (Slow / Medium / Stable)
#       * HTML → summary box + color-coded tables (red/yellow/green)
#       * JSON → enriched entries with all computed metrics
#   - Adds extended metrics:
#       * Variance (difference between Real and CPU time)
#       * Ops/sec (throughput, human-readable: K / M / B)
#       * Relative Factor (X slower than fastest benchmark)
#       * Type classification (CPU-bound vs I/O-bound)
#   - Provides automatic grouping:
#       * Slow benchmarks (≥ 50ms)
#       * Medium Variance (1–50ms)
#       * Stable (< 1ms)
#   - Adds baseline comparison:
#       * Automatically detects the earliest (baseline) benchmark report.
#       * Compares current results with that baseline.
#       * Adds:
#           - Time (ms): absolute difference (current - baseline)
#           - Change (%): ((baseline - current) / baseline) × 100
#           - Trend: text status (Faster / Slower / Same / New)
#   - Prints friendly messages when no Slow/Medium exist, or all Stable
#   - Backward-compatible with CI/CD pipelines and local runs
#
# Updated Features:
#   - Added Baseline Comparison (with first benchmark file)
#   - Added Time (ms), Change (%), Trend, Baseline (ms), Speedup (x)
#   - Added Summary Section (Faster / Slower / Same / New / Overall %)
#   - Improved Variance alignment with fixed precision
# -----------------------------------------------------------------------------

import subprocess
import json
from datetime import datetime
from pathlib import Path
import argparse
import shutil
import os
import platform
import sys
import time
import statistics

# ----------------------------------------------------------------------------- 
# CLI Arguments
# -----------------------------------------------------------------------------
parser = argparse.ArgumentParser(description="Tasqly Benchmark Runner (Enhanced Baseline Mode)")
parser.add_argument("--phase", required=True, help="Benchmark phase name (e.g., Phase0, Phase1)")
parser.add_argument("--compiler", required=True, choices=["msvc", "mingw", "gcc", "clang"],
                    help="Compiler used to build benchmarks (msvc/mingw/gcc/clang)")
parser.add_argument("--runner", default=None, help="Path to benchmark runner executable (optional override)")
parser.add_argument("--quiet", action="store_true", help="Suppress benchmark runner stdout output")
parser.add_argument("--timeout", type=int, default=900, help="Timeout in seconds for benchmark execution (default=900)")
parser.add_argument("--format", choices=["markdown", "html", "json", "all"], default="all",
                    help="Output report format (default=all)")
args = parser.parse_args()

# ----------------------------------------------------------------------------- 
# Paths & Setup
# -----------------------------------------------------------------------------
PHASE_NAME = args.phase
compiler = args.compiler
date_str = datetime.now().strftime("%Y-%m-%d")

project_root = Path(__file__).resolve().parent.parent
reports_root = project_root / "reports" / "benchmarks" / f"{PHASE_NAME}_perf_{compiler}_benchmarks_reports"
raw_dir = reports_root / "raw"
dashboard_dir = reports_root / "dashboard"
md_dir = reports_root / "markdown"
raw_dir.mkdir(parents=True, exist_ok=True)
dashboard_dir.mkdir(parents=True, exist_ok=True)
md_dir.mkdir(parents=True, exist_ok=True)

default_runners = {
    "mingw": "build/mingw-benchmarks-release/TasqlyBenchmarksRunner.exe",
    "msvc": "build/msvc-benchmarks-release/TasqlyBenchmarksRunner.exe",
    "gcc": "build/gcc-benchmarks-release/TasqlyBenchmarksRunner",
    "clang": "build/clang-benchmarks-release/TasqlyBenchmarksRunner"
}
runner_path = Path(args.runner) if args.runner else project_root / default_runners[compiler]

json_path = raw_dir / f"{PHASE_NAME}_{date_str}_bench_results.json"
md_path = md_dir / f"{PHASE_NAME}_{date_str}_report.md"
html_path = dashboard_dir / f"{PHASE_NAME}_{date_str}_dashboard.html"
html_latest = dashboard_dir / "index.html"
summary_json_path = reports_root / f"{PHASE_NAME}_{date_str}_summary.json"

# ----------------------------------------------------------------------------- 
# Run Benchmarks
# -----------------------------------------------------------------------------
print(f"[INFO] Running benchmarks for {PHASE_NAME} ({compiler})... saving to {json_path}")
now = datetime.now().astimezone()
print(f"[INFO] Date: {now.strftime('%Y-%m-%d')} || Time: {now.strftime('%I:%M %p')}")
print(f"[INFO] Running {runner_path}")
start_time = time.time()

try:
    cmd = [str(runner_path), f"--benchmark_out={json_path}"]
    if args.quiet:
        subprocess.run(cmd, check=True, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT, timeout=args.timeout)
    else:
        subprocess.run(cmd, check=True, timeout=args.timeout)
except subprocess.CalledProcessError as e:
    print("[ERROR] Benchmark runner failed:", e)
    sys.exit(1)
except subprocess.TimeoutExpired:
    print("[ERROR] Benchmark runner timed out")
    sys.exit(1)

with open(json_path, "r", encoding="utf-8") as f:
    data = json.load(f)

benchmarks = [b for b in data.get("benchmarks", []) if b.get("run_type") == "iteration"]
context = data.get("context", {})

# ----------------------------------------------------------------------------- 
# Helpers
# -----------------------------------------------------------------------------
def to_ms(value, unit):
    if unit == "ns": return value / 1_000_000
    elif unit == "us": return value / 1_000
    elif unit == "ms": return value
    elif unit == "s": return value * 1000
    return value

def classify_status(real_ms):
    if real_ms < 1.0: return "Stable"
    elif real_ms < 50.0: return "Medium Variance"
    return "Slow"

def classify_type(real_ms, cpu_ms):
    if cpu_ms <= 0 and real_ms > 0: return "I/O-bound"
    if cpu_ms > 0 and real_ms > cpu_ms * 2: return "I/O-bound"
    return "CPU-bound"

def format_ops(value):
    if value <= 0: return "-"
    if value >= 1_000_000_000: return f"{value/1_000_000_000:.2f}B"
    if value >= 1_000_000: return f"{value/1_000_000:.2f}M"
    if value >= 1_000: return f"{value/1_000:.2f}K"
    return f"{int(value)}"

def format_markdown_table(rows, headers):
    col_widths = [len(h) for h in headers]
    for row in rows:
        for i, cell in enumerate(row):
            col_widths[i] = max(col_widths[i], len(str(cell)))
    header_line = "| " + " | ".join(headers[i].ljust(col_widths[i]) for i in range(len(headers))) + " |"
    sep_line = "|-" + "-|-".join("-" * col_widths[i] for i in range(len(headers))) + "-|"
    lines = [header_line, sep_line]
    for row in rows:
        formatted_cells = []
        for i, cell in enumerate(row):
            s = str(cell)
            formatted_cells.append(s.rjust(col_widths[i]) if s.replace('.', '', 1).isdigit() else s.ljust(col_widths[i]))
        lines.append("| " + " | ".join(formatted_cells) + " |")
    return "\n".join(lines)

# ----------------------------------------------------------------------------- 
# Baseline Comparison Logic
# -----------------------------------------------------------------------------
def find_baseline(current_file: Path):
    files = list(raw_dir.glob(f"{PHASE_NAME}_*_bench_results.json"))
    candidates = [p for p in files if p.resolve() != current_file.resolve()]
    if not candidates: return None
    candidates.sort(key=lambda p: p.stat().st_mtime)  # oldest first
    return candidates[0]

baseline_file = find_baseline(json_path)
if baseline_file:
    print(f"[INFO] Found baseline report for comparison: {baseline_file.name}")
    with open(baseline_file, "r", encoding="utf-8") as f:
        base_data = json.load(f)
    base_map = {
        b.get("name"): to_ms(b.get("real_time", 0), b.get("time_unit", "ns"))
        for b in base_data.get("benchmarks", [])
        if b.get("run_type") == "iteration"
    }
else:
    base_map = {}

# ----------------------------------------------------------------------------- 
# Process Results
# -----------------------------------------------------------------------------
rows_md, real_times, names = [], [], []
for b in benchmarks:
    name = b.get("name")
    rt_raw, ct_raw, unit = b.get("real_time", 0), b.get("cpu_time", 0), b.get("time_unit", "ns")
    iterations, threads = b.get("iterations", ""), b.get("threads", 1)
    real_ms, cpu_ms = to_ms(rt_raw, unit), to_ms(ct_raw, unit)
    variance = real_ms - cpu_ms
    ops_sec = int(int(iterations) / (real_ms / 1000.0)) if real_ms > 0 else 0
    rows_md.append({
        "name": name,
        "iterations": str(iterations),
        "real_ms": real_ms,
        "cpu_ms": cpu_ms,
        "variance": variance,
        "threads": threads,
        "ops_sec": ops_sec
    })
    real_times.append(real_ms)
    names.append(name)

min_time = min((r["real_ms"] for r in rows_md if r["real_ms"] > 0), default=None)

final_rows, perf_changes = [], []
for r in rows_md:
    name, real_ms, cpu_ms, ops = r["name"], r["real_ms"], r["cpu_ms"], r["ops_sec"]
    relative = f"{(real_ms / min_time):.1f}x" if min_time and real_ms > 0 else "-"
    status, type_ = classify_status(real_ms), classify_type(real_ms, cpu_ms)
    base_val = base_map.get(name)

    if base_val is None:
        delta_str, change_str, trend, speedup, baseline_str = "-", "NEW", "New", "-", "-"
    elif base_val == 0:
        delta_str, change_str, trend, speedup, baseline_str = "-", "-", "Same", "-", f"{real_ms:.3f}"
    else:
        delta = real_ms - base_val
        pct = ((base_val - real_ms) / base_val) * 100
        speedup = base_val / real_ms if real_ms > 0 else 0
        delta_str = f"{delta:+.3f}"
        change_str = f"{abs(pct):.2f}%"
        baseline_str = f"{base_val:.3f}"
        if pct > 0.5:
            trend = "Faster"
        elif pct < -0.5:
            trend = "Slower"
        else:
            trend = "Same"
        perf_changes.append(pct)

    final_rows.append([
        name, r["iterations"], f"{real_ms:.3f}", f"{cpu_ms:.3f}",
        f"{r['variance']:8.3f}", str(r["threads"]), format_ops(ops),
        relative, type_, baseline_str, f"{speedup:.2f}x" if speedup != "-" else "-",
        delta_str, change_str, trend, status
    ])

# ----------------------------------------------------------------------------- 
# Markdown Report Generation
# -----------------------------------------------------------------------------
def generate_markdown():
    headers = [
        "Benchmark", "Iterations", "Real Time (ms)", "CPU Time (ms)",
        "Variance (ms)", "Threads", "Ops/sec", "Relative", "Type",
        "Baseline (ms)", "Speedup (x)", "Time (ms)", "Change (%)", "Trend"
    ]

    groups = {"Slow": [], "Medium Variance": [], "Stable": []}
    for row in final_rows:
        groups[row[-1]].append(row[:-1])
    for k in groups:
        groups[k].sort(key=lambda x: float(x[2]) if x[2] not in ("-", "") else 0.0, reverse=True)

    best = min(final_rows, key=lambda x: float(x[2])) if final_rows else None
    worst = max(final_rows, key=lambda x: float(x[2])) if final_rows else None

    local_time = datetime.now().astimezone().strftime("%I:%M %p — %Y-%m-%d")
    with open(md_path, "w", encoding="utf-8") as f:
        f.write(f"# Benchmark Report ({compiler.upper()})\n_Phase: {PHASE_NAME} — {date_str}_\n\n")
        f.write(f"**Commit**: {os.environ.get('GITHUB_SHA', 'local')}  \n")
        f.write(f"**Job**: {os.environ.get('GITHUB_JOB', 'manual')}  \n")
        f.write(f"**Host**: {context.get('host_name', 'unknown')}  \n")
        f.write(f"**CPU**: {context.get('num_cpus', '?')} cores @ {context.get('mhz_per_cpu', '?')} MHz  \n")
        f.write(f"**Runner**: {runner_path}  \n")
        f.write(f"**Execution Time**: {elapsed_str}  \n")
        f.write(f"**Generated On**: {local_time}\n\n---\n\n")
        if best and worst:
            f.write(f"**Best Benchmark**: {best[0]} ({best[2]} ms)\n\n")
            f.write(f"**Worst Benchmark**: {worst[0]} ({worst[2]} ms)\n\n---\n\n")

        for status in ["Slow", "Medium Variance", "Stable"]:
            rows = groups.get(status, [])
            if rows:
                f.write(f"## {status} Benchmarks\n\n{format_markdown_table(rows, headers)}\n\n")
            elif status == "Slow":
                f.write("## Slow Benchmarks\n\nNo slow benchmarks found.\n\n")
            elif status == "Medium Variance":
                f.write("## Medium Variance Benchmarks\n\nNo medium-variance benchmarks found.\n\n")

        # Comparison Summary
        faster = sum(1 for r in final_rows if r[-2] == "Faster")
        slower = sum(1 for r in final_rows if r[-2] == "Slower")
        same = sum(1 for r in final_rows if r[-2] == "Same")
        newb = sum(1 for r in final_rows if r[-2] == "New")
        avg_gain = statistics.mean(perf_changes) if perf_changes else 0.0

        f.write("---\n\n## Comparison Summary\n\n")
        f.write(f"- **Faster:** {faster}\n")
        f.write(f"- **Slower:** {slower}\n")
        f.write(f"- **Same:** {same}\n")
        f.write(f"- **New:** {newb}\n")
        f.write(f"- **Overall Improvement:** {avg_gain:+.2f}%\n\n")

    print(f"[OK] Markdown report generated: {md_path}")

# ----------------------------------------------------------------------------- 
# HTML + JSON + Execution Time + Cleanup
# -----------------------------------------------------------------------------
def generate_html():
    local_time = datetime.now().astimezone()
    formatted_time = local_time.strftime("%I:%M %p — %Y-%m-%d")

    # 🧩 Meta info
    meta_html = f"""
    <div style='font-family:Segoe UI, sans-serif; font-size:14px; margin-bottom:10px;'>
        <strong>Date:</strong> {formatted_time} |
        <strong>Host:</strong> {context.get('host_name','unknown')} |
        <strong>CPU:</strong> {context.get('num_cpus','?')} cores @ {context.get('mhz_per_cpu','?')} MHz |
        <strong>Runner:</strong> {runner_path}
    </div>
    <hr/>
    """

    # 📊 Summary box
    if final_rows:
        best = min(final_rows, key=lambda x: float(x[2]))
        worst = max(final_rows, key=lambda x: float(x[2]))
        avg = sum(float(r[2]) for r in final_rows) / len(final_rows)
        summary_html = f"""
        <div style='padding:12px; background:#111; color:#eee; font-family:Segoe UI, sans-serif; margin:15px 0; border-radius:8px;'>
            <h3 style='margin:0 0 6px 0;'>Benchmark Summary</h3>
            <p style='margin:4px 0;'>Total Benchmarks: <strong>{len(final_rows)}</strong></p>
            <p style='margin:4px 0;'>Best: <strong>{best[0]}</strong> — {best[2]} ms</p>
            <p style='margin:4px 0;'>Worst: <strong>{worst[0]}</strong> — {worst[2]} ms</p>
            <p style='margin:4px 0;'>Average Real Time: <strong>{avg:.3f} ms</strong></p>
            <p style='margin:4px 0;'>Generated On: <strong>{formatted_time}</strong></p>
            <p style='margin:4px 0;'>Execution Time: <strong>{elapsed_str}</strong></p>
        </div>
        """
    else:
        summary_html = ""

    def row_color(status):
        if status == "Slow":
            return "background: rgba(200, 80, 80, 0.8);"
        elif status == "Medium Variance":
            return "background: rgba(200, 180, 80, 0.8);"
        elif status == "Stable":
            return "background: rgba(80, 160, 80, 0.8);"
        return ""

    groups = {"Slow": [], "Medium Variance": [], "Stable": []}
    for r in final_rows:
        groups.setdefault(r[-1], []).append(r)

    for k in groups:
        groups[k].sort(key=lambda x: float(x[2]) if x and x[2] not in ("-", "") else 0.0, reverse=True)

    table_header = """
    <tr>
        <th>Benchmark</th>
        <th>Iterations</th>
        <th>Real (ms)</th>
        <th>CPU (ms)</th>
        <th>Variance</th>
        <th>Threads</th>
        <th>Ops/sec</th>
        <th>Relative</th>
        <th>Type</th>
        <th>Baseline (ms)</th>
        <th>Speedup (x)</th>
        <th>Time (ms)</th>
        <th>Change (%)</th>
        <th>Trend</th>
    </tr>
    """

    tables_html = ""
    for status in ["Slow", "Medium Variance", "Stable"]:
        rows = groups.get(status, [])
        if rows:
            tables_html += f"<h2>{status} Benchmarks</h2>\n<table>\n{table_header}\n"
            for r in rows:
                color = row_color(status)
                tables_html += f"<tr style='{color}'>"
                for cell in r[:-1]:
                    tables_html += f"<td>{cell}</td>"
                tables_html += "</tr>\n"
            tables_html += "</table>\n"
        else:
            if status == "Slow":
                tables_html += "<h2>Slow Benchmarks</h2><p>No slow benchmarks found.</p>\n"
            elif status == "Medium Variance":
                tables_html += "<h2>Medium Variance Benchmarks</h2><p>No medium-variance benchmarks found.</p>\n"

    if not groups["Slow"] and not groups["Medium Variance"] and groups["Stable"]:
        tables_html += "<p>All benchmarks are stable and fast.</p>\n"

    html_content = f"""
    <html>
    <head>
        <title>Tasqly Benchmark Dashboard - {PHASE_NAME} - {formatted_time}</title>
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
        <style>
            body {{
                font-family: Arial, sans-serif;
                margin: 20px;
                background: #111;
                color: #eee;
            }}
            table {{
                border-collapse: collapse;
                width: 100%;
                margin-top: 12px;
                background: #222;
                color: #eee;
            }}
            th, td {{
                border: 1px solid #444;
                padding: 6px 8px;
                text-align: left;
                font-size: 13px;
            }}
            th {{
                background: #333;
                color: #fff;
            }}
            h1, h2, h3 {{
                color: #fff;
                margin: 8px 0;
            }}
        </style>
    </head>
    <body>
        <h1>Tasqly Benchmark Results - {PHASE_NAME} ({compiler.upper()})</h1>
        {meta_html}
        {summary_html}
        {tables_html}
    </body>
    </html>
    """

    with open(html_path, "w", encoding="utf-8") as f:
        f.write(html_content)
    with open(html_latest, "w", encoding="utf-8") as f:
        f.write(html_content)

    print(f"[OK] HTML dashboard generated: {html_path}")

def generate_json_summary():
    summary = {
        "meta": {"phase": PHASE_NAME,"compiler": compiler,"baseline_file": baseline_file.name if baseline_file else None},
        "benchmarks": [{
            "name": r[0],"iterations": r[1],"real_ms": float(r[2]),"cpu_ms": float(r[3]),
            "variance_ms": float(r[4]),"threads": int(r[5]),"ops_sec": r[6],"relative_factor": r[7],
            "type": r[8],"baseline_ms": r[9],"speedup": r[10],"delta_ms": r[11],"change_percent": r[12],"trend": r[13],"status": r[14]
        } for r in final_rows]
    }
    with open(summary_json_path,"w",encoding="utf-8") as f: json.dump(summary,f,indent=2)
    print(f"[OK] JSON summary saved: {summary_json_path}")

elapsed_sec = time.time() - start_time
minutes, seconds = int(elapsed_sec // 60), int(elapsed_sec % 60)
elapsed_str = f"{minutes} min {seconds} sec"
print(f"[INFO] Total Execution Time: {elapsed_str}")

if args.format in ("markdown","all"): generate_markdown()
if args.format in ("html","all"): generate_html()
if args.format in ("json","all"): generate_json_summary()

logs_dir = project_root / "reports" / "benchmarks" / "logs"
if logs_dir.exists():
    shutil.rmtree(logs_dir, ignore_errors=True)
    print(f"[CLEANUP] Removed benchmark logs: {logs_dir}")
    