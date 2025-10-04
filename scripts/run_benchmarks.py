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
#   - Prints friendly messages when no Slow/Medium exist, or all Stable
#   - Backward-compatible with CI/CD pipelines and local runs
#   - Cleans up unnecessary debug logs
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

# ----------------------------------------------------------------------------- 
# CLI Arguments
# -----------------------------------------------------------------------------
parser = argparse.ArgumentParser(description="Tasqly Benchmark Runner (Unified)")
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

default_runners = {
    "mingw": "build/mingw-benchmarks-release/TasqlyBenchmarksRunner.exe",
    "msvc": "build/msvc-benchmarks-release/TasqlyBenchmarksRunner.exe",
    "gcc": "build/gcc-benchmarks-release/TasqlyBenchmarksRunner",
    "clang": "build/clang-benchmarks-release/TasqlyBenchmarksRunner"
}
runner_path = Path(args.runner) if args.runner else project_root / default_runners[compiler]

reports_root = project_root / "reports" / "benchmarks" / f"{PHASE_NAME}_perf_{compiler}_benchmarks_reports"
raw_dir = reports_root / "raw"
dashboard_dir = reports_root / "dashboard"
md_dir = reports_root / "markdown"
raw_dir.mkdir(parents=True, exist_ok=True)
dashboard_dir.mkdir(parents=True, exist_ok=True)
md_dir.mkdir(parents=True, exist_ok=True)

json_path = raw_dir / f"{PHASE_NAME}_{date_str}_bench_results.json"
html_path = dashboard_dir / f"{PHASE_NAME}_{date_str}_dashboard.html"
html_latest = dashboard_dir / "index.html"
md_path = md_dir / f"{PHASE_NAME}_{date_str}_report.md"
summary_json_path = reports_root / f"{PHASE_NAME}_{date_str}_summary.json"

# ----------------------------------------------------------------------------- 
# Run Benchmarks
# -----------------------------------------------------------------------------
print(f"[INFO] Running benchmarks for {PHASE_NAME} ({compiler})... saving to {json_path}")

# 🕒 عرض التاريخ والوقت بتنسيق أنيق
now = datetime.now().astimezone()
print(f"Date: {now.strftime('%Y-%m-%d')} || Time: {now.strftime('%I:%M %p')}")

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
    if unit == "ns":
        return value / 1_000_000
    elif unit == "us":
        return value / 1_000
    elif unit == "ms":
        return value
    elif unit == "s":
        return value * 1000
    return value

def classify_status(real_ms):
    """Classify benchmark by wall-clock time (ms)."""
    if real_ms < 1.0:
        return "Stable"
    elif real_ms < 50.0:
        return "Medium Variance"
    return "Slow"

def classify_type(real_ms, cpu_ms):
    """Heuristic: if real_time >> cpu_time -> I/O-bound, else CPU-bound."""
    try:
        if cpu_ms <= 0 and real_ms > 0:
            return "I/O-bound"
        if cpu_ms > 0 and real_ms > cpu_ms * 2:
            return "I/O-bound"
        return "CPU-bound"
    except Exception:
        return "unknown"

def format_ops(value):
    """Human-friendly formatting for Ops/sec large numbers."""
    try:
        v = float(value)
    except Exception:
        return "-"
    if v <= 0:
        return "-"
    if v >= 1_000_000_000:
        return f"{v/1_000_000_000:.2f}B"
    if v >= 1_000_000:
        return f"{v/1_000_000:.2f}M"
    if v >= 1_000:
        return f"{v/1_000:.2f}K"
    # show integer for small values
    return f"{int(v)}"

def format_markdown_table(rows, headers):
    """Aligned Markdown table with left text / right numbers."""
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
            # numeric detection (allow commas in formatted numbers)
            try:
                float(s.replace(",", "").rstrip("KM B"))
                is_number = True
            except Exception:
                is_number = False
            if is_number:
                formatted_cells.append(s.rjust(col_widths[i]))
            else:
                formatted_cells.append(s.ljust(col_widths[i]))
        lines.append("| " + " | ".join(formatted_cells) + " |")
    return "\n".join(lines)

# ----------------------------------------------------------------------------- 
# Process Results
# -----------------------------------------------------------------------------
rows_md = []
real_times = []
names = []

# Build preliminary rows with computed metrics
for b in benchmarks:
    name = b.get("name")
    iterations = b.get("iterations", "")
    rt_raw = b.get("real_time", 0)
    ct_raw = b.get("cpu_time", 0)
    unit = b.get("time_unit", "ns")
    threads = b.get("threads", 1)

    real_ms = to_ms(rt_raw, unit)
    cpu_ms = to_ms(ct_raw, unit)
    variance = real_ms - cpu_ms

    # ops/sec estimation: iterations per second (real_ms is ms)
    ops_sec = 0.0
    try:
        if real_ms > 0:
            ops_sec = int(int(iterations) / (real_ms / 1000.0))
    except Exception:
        ops_sec = 0.0

    names.append(name)
    real_times.append(real_ms)

    rows_md.append({
        "name": name,
        "iterations": str(iterations),
        "real_ms": real_ms,
        "cpu_ms": cpu_ms,
        "variance": variance,
        "threads": threads,
        "ops_sec": ops_sec
    })

# compute relative factor using fastest (smallest real_ms > 0)
nonzero_times = [r["real_ms"] for r in rows_md if r["real_ms"] > 0]
min_time = min(nonzero_times) if nonzero_times else None

# enrich rows: formatted fields, type, status, relative factor
final_rows = []
for r in rows_md:
    real_ms = r["real_ms"]
    cpu_ms = r["cpu_ms"]
    ops = r["ops_sec"]
    if min_time and min_time > 0 and real_ms > 0:
        relative_factor = f"{(real_ms / min_time):.1f}x"
    else:
        relative_factor = "-"  # cannot compute
    status = classify_status(real_ms)
    type_ = classify_type(real_ms, cpu_ms)

    final_rows.append([
        r["name"],
        r["iterations"],
        f"{real_ms:.3f}",
        f"{cpu_ms:.3f}",
        f"{r['variance']:.3f}",
        str(r["threads"]),
        format_ops(ops),
        relative_factor,
        type_,
        status  # keep status last for grouping convenience
    ])

# ----------------------------------------------------------------------------- 
# Generate Reports
# -----------------------------------------------------------------------------
def generate_markdown():
    headers = ["Benchmark", "Iterations", "Real Time (ms)", "CPU Time (ms)",
               "Variance (ms)", "Threads", "Ops/sec", "Relative", "Type"]

    # Group by status
    groups = {"Slow": [], "Medium Variance": [], "Stable": []}
    for row in final_rows:
        status = row[9]
        groups.setdefault(status, []).append(row[:-1])  # append without status column

    # Sort each group: slowest first (highest real_ms)
    for k in groups:
        groups[k].sort(key=lambda x: float(x[2]) if x and x[2] not in ("-", "") else 0.0, reverse=True)

    # Summary
    best = min(final_rows, key=lambda x: float(x[2])) if final_rows else None
    worst = max(final_rows, key=lambda x: float(x[2])) if final_rows else None

    # Local time in AM/PM with separator
    local_time = datetime.now().astimezone()
    formatted_time = local_time.strftime("%I:%M %p — %Y-%m-%d")

    with open(md_path, "w", encoding="utf-8") as f:
        f.write(f"# Benchmark Report ({compiler.upper()})\n")
        f.write(f"_Phase: {PHASE_NAME} — {date_str}_\n\n")
        f.write(f"**Commit**: {os.environ.get('GITHUB_SHA', 'local')}  \n")
        f.write(f"**Job**: {os.environ.get('GITHUB_JOB', 'manual')}  \n")
        f.write(f"**Host**: {context.get('host_name', 'unknown')}  \n")
        f.write(f"**CPU**: {context.get('num_cpus', '?')} cores @ {context.get('mhz_per_cpu', '?')} MHz  \n")
        f.write(f"**Runner**: {runner_path}  \n")
        f.write(f"**Execution Time**: {elapsed_str}  \n")
        f.write(f"**Generated On**: {formatted_time}\n\n")
        f.write("---\n\n")

        if best and worst:
            f.write(f"**Best Benchmark**: {best[0]} ({best[2]} ms)\n\n")
            f.write(f"**Worst Benchmark**: {worst[0]} ({worst[2]} ms)\n\n")
            f.write("---\n\n")

        # Produce groups in order Slow -> Medium -> Stable
        order = ["Slow", "Medium Variance", "Stable"]
        for status in order:
            rows = groups.get(status, [])
            if rows:
                f.write(f"## {status} Benchmarks\n\n")
                table = format_markdown_table(rows, headers)
                f.write(table + "\n\n")
            else:
                if status == "Slow":
                    f.write("## Slow Benchmarks\n\nNo slow benchmarks found. Performance is stable for this run.\n\n")
                elif status == "Medium Variance":
                    f.write("## Medium Variance Benchmarks\n\nNo medium-variance benchmarks found.\n\n")

        if not groups["Slow"] and not groups["Medium Variance"] and groups["Stable"]:
            f.write("All benchmarks are stable and fast.\n\n")

    print(f"[OK] Markdown report generated: {md_path}")

def generate_html():
    # الحصول على الوقت المحلي بصيغة AM/PM
    local_time = datetime.now().astimezone()
    formatted_time = local_time.strftime("%I:%M %p — %Y-%m-%d")

    # meta
    meta_html = f"<div><strong>Date:</strong> {formatted_time} | <strong>Host:</strong> {context.get('host_name','unknown')} | <strong>CPU:</strong> {context.get('num_cpus','?')} cores @ {context.get('mhz_per_cpu','?')} MHz | <strong>Runner:</strong> {runner_path}</div><hr/>"

    # summary box
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

    # Build table rows grouped and colored by status
    def row_color(status):
        if status == "Slow":
            return "background: rgba(200, 80, 80, 0.8);"   # dark red
        if status == "Medium Variance":
            return "background: rgba(200, 180, 80, 0.8);"  # dark yellow/golden
        if status == "Stable":
            return "background: rgba(80, 160, 80, 0.8);"   # dark green
        return ""

    # group rows same as markdown
    groups = {"Slow": [], "Medium Variance": [], "Stable": []}
    for r in final_rows:
        groups.setdefault(r[9], []).append(r)

    # sort groups: slowest first
    for k in groups:
        groups[k].sort(key=lambda x: float(x[2]) if x and x[2] not in ("-", "") else 0.0, reverse=True)

    # headers for HTML table
    table_header = "<tr><th>Name</th><th>Iterations</th><th>Real (ms)</th><th>CPU (ms)</th><th>Variance</th><th>Threads</th><th>Ops/sec</th><th>Relative</th><th>Type</th></tr>"

    # build HTML content
    tables_html = ""
    for status in ["Slow", "Medium Variance", "Stable"]:
        rows = groups.get(status, [])
        if rows:
            tables_html += f"<h2>{status} Benchmarks</h2>\n"
            tables_html += "<table>\n" + table_header + "\n"
            for r in rows:
                color = row_color(status)
                tables_html += f"<tr style='{color}'>"
                tables_html += "<td>{}</td><td>{}</td><td>{}</td><td>{}</td><td>{}</td><td>{}</td><td>{}</td><td>{}</td><td>{}</td>".format(
                    r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8]
                )
                tables_html += "</tr>\n"
            tables_html += "</table>\n"
        else:
            if status == "Slow":
                tables_html += "<h2>Slow Benchmarks</h2>\n<p>No slow benchmarks found. Performance is stable for this run.</p>\n"
            elif status == "Medium Variance":
                tables_html += "<h2>Medium Variance Benchmarks</h2>\n<p>No medium-variance benchmarks found.</p>\n"

    if not groups.get("Slow") and not groups.get("Medium Variance") and groups.get("Stable"):
        tables_html += "<p>All benchmarks are stable and fast.</p>\n"

    html_content = f"""
    <html>
    <head>
      <title>Tasqly Benchmark Dashboard - {PHASE_NAME} - {formatted_time}</title>
      <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
      <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; background: #111; color: #eee; }}
        table {{ border-collapse: collapse; width: 100%; margin-top: 12px; background: #222; color: #eee; }}
        th, td {{ border: 1px solid #444; padding: 8px; text-align: left; font-size: 13px; }}
        th {{ background: #333; color: #fff; }}
        h1, h2, h3 {{ margin: 8px 0; color: #fff; }}
      </style>
    </head>
    <body>
      <h1>Tasqly Benchmark Results - {PHASE_NAME} ({formatted_time}) [{compiler.upper()}]</h1>
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
        "meta": {
            "phase": PHASE_NAME,
            "compiler": compiler,
            "commit": os.environ.get("GITHUB_SHA", "local"),
            "job": os.environ.get("GITHUB_JOB", "manual"),
            "date": date_str,
            "os": platform.system().lower(),
            "execution_time_sec": round(elapsed_sec, 2)
        },
        "benchmarks": [
            {
                "name": r[0],
                "iterations": r[1],
                "real_ms": float(r[2]),
                "cpu_ms": float(r[3]),
                "variance_ms": float(r[4]),
                "threads": int(r[5]),
                "ops_sec": r[6],
                "relative_factor": r[7],
                "type": r[8],
                "status": r[9]
            }
            for r in final_rows
        ]
    }
    with open(summary_json_path, "w", encoding="utf-8") as f:
        json.dump(summary, f, indent=2)
    print(f"[OK] JSON summary saved: {summary_json_path}")

# ----------------------------------------------------------------------------- 
# Execution Time
# -----------------------------------------------------------------------------
elapsed_sec = time.time() - start_time
minutes = int(elapsed_sec // 60)
seconds = int(elapsed_sec % 60)
elapsed_str = f"{minutes} min {seconds} sec"
print(f"[INFO] Total Execution Time: {elapsed_str}")

# ----------------------------------------------------------------------------- 
# Output Selection
# -----------------------------------------------------------------------------
if args.format in ("markdown", "all"):
    generate_markdown()
if args.format in ("html", "all"):
    generate_html()
if args.format in ("json", "all"):
    generate_json_summary()

# ----------------------------------------------------------------------------- 
# Cleanup
# -----------------------------------------------------------------------------
logs_dir = project_root / "reports" / "benchmarks" / "logs"
if logs_dir.exists():
    shutil.rmtree(logs_dir, ignore_errors=True)
    print(f"[CLEANUP] Removed benchmark logs: {logs_dir}")
