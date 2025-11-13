#!/usr/bin/env python3
# -*- coding: utf-8 -*-
r"""
===============================================================================
  Coverage Report Generator (Tests Only, Enhanced Clean Version)
===============================================================================

  Features:
   - Parse Cobertura-style coverage.xml for test results only.
   - Generate per-file stats: Coverage %, Covered, Uncovered, Total lines.
   - Produce Markdown report with:
       * Phase Summary table (meta info: commit, branch, phase, OS, compiler).
       * Coverage Summary table (files, coverage %, covered/uncovered counts).
       * Coverage by File table (sorted worst → best).
   - JSON report export (keeps only 2 latest files: baseline + current).
   - Dashboard (Plotly) export:
       * Coverage Summary Pie chart (Covered vs Uncovered).
       * Coverage by File Bar chart (worst → best, colored by High/Medium/Low/None).
       * Phase metadata shown at top.
   - "Change" column in Markdown shows delta % vs previous JSON (if exists).
   - Auto cleanup: never keep more than 2 JSON coverage reports per phase.
   - Comments are in English for clarity.

  Usage examples:
    py -3.13 scripts\coverage_report_updated.py reports\coverage\tests\coverage.xml all --archive Phase1
    py -3.13 scripts\coverage_report_updated.py reports\coverage\tests\coverage.xml markdown
    py -3.13 scripts\coverage_report_updated.py reports\coverage\tests\coverage.xml json
===============================================================================
"""

import xml.etree.ElementTree as ET
import sys
import os
import datetime
import platform
import json
import glob
import plotly.express as px
import plotly.graph_objects as go

# -------------------------
# Helpers
# -------------------------

def ensure_dirs(base_dir):
    os.makedirs(base_dir, exist_ok=True)

def detect_compiler_suffix():
    if "CXX_COMPILER" in os.environ:
        return os.environ["CXX_COMPILER"].lower()
    if "CC" in os.environ:
        return os.environ["CC"].lower()
    return platform.system().lower()

def cleanup_json_reports(directory):
    """Keep only 2 latest *_coverage.json files."""
    pattern = os.path.join(directory, "*_coverage.json")
    files = glob.glob(pattern)
    if len(files) <= 2:
        return
    files.sort(key=lambda p: os.path.getmtime(p), reverse=True)
    for old_file in files[2:]:
        try:
            os.remove(old_file)
            print(f"[CLEANUP] Removed old coverage JSON: {old_file}")
        except Exception as e:
            print(f"[WARN] Could not remove {old_file}: {e}")

def find_previous_json(directory):
    """Return previous JSON file for delta comparison (newest, before current)."""
    pattern = os.path.join(directory, "*_coverage.json")
    cand = glob.glob(pattern)
    if len(cand) < 1:
        return None
    cand.sort(key=lambda p: os.path.getmtime(p), reverse=True)
    return cand[0]  # newest available (after cleanup, max 2 remain)

# -------------------------
# Parse coverage XML
# -------------------------

def analyze_coverage(file_path):
    if not os.path.exists(file_path):
        print(f"[ERROR] Coverage file not found: {file_path}")
        sys.exit(2)

    tree = ET.parse(file_path)
    root = tree.getroot()
    results = []

    for clazz in root.findall(".//class"):
        raw_filename = clazz.get("filename", "")
        filename = os.path.basename(raw_filename)
        try:
            line_rate = float(clazz.get("line-rate", "0"))
        except Exception:
            line_rate = 0.0
        covered_attr = clazz.get("lines-covered")
        valid_attr = clazz.get("lines-valid")
        if covered_attr is not None and valid_attr is not None:
            try:
                covered = int(covered_attr)
                total = int(valid_attr)
            except Exception:
                covered = 0
                total = 0
        else:
            lines = clazz.findall(".//lines/line")
            total = len(lines)
            covered = sum(1 for ln in lines if ln.get("hits") and int(ln.get("hits")) > 0)
        results.append((filename, line_rate, covered, total))

    if not results:
        print(f"[WARNING] No coverage data found in {file_path}")
        sys.exit(0)

    return results

# -------------------------
# Metadata generation
# -------------------------

def generate_metadata(phase=None):
    now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M")
    commit = os.environ.get("GITHUB_SHA", "local")
    branch = os.environ.get("GITHUB_REF_NAME", "main")
    job = os.environ.get("GITHUB_JOB", "manual")
    compiler = detect_compiler_suffix()
    os_name = platform.system().lower()

    meta = {
        "timestamp": now,
        "phase": phase,
        "commit": commit,
        "branch": branch,
        "job": job,
        "compiler": compiler,
        "os": os_name
    }
    return meta

# -------------------------
# Markdown table formatter
# -------------------------

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
            is_number = False
            if s.endswith("%"):
                is_number = True
            else:
                try:
                    _ = float(s)
                    is_number = True
                except Exception:
                    is_number = False
            if is_number:
                formatted_cells.append(s.rjust(col_widths[i]))
            else:
                formatted_cells.append(s.ljust(col_widths[i]))
        lines.append("| " + " | ".join(formatted_cells) + " |")
    return "\n".join(lines)

# -------------------------
# Markdown exporter
# -------------------------

def export_markdown(results, out_file, meta, prev_json_path=None):
    prev_map = {}
    if prev_json_path and os.path.exists(prev_json_path):
        try:
            with open(prev_json_path, "r", encoding="utf-8") as f:
                old = json.load(f)
                for entry in old.get("files", []):
                    prev_map[os.path.basename(entry.get("file", ""))] = float(entry.get("coverage", 0.0))
        except Exception:
            prev_map = {}

    total_files = len(results)
    total_covered = sum(c for _, _, c, _ in results)
    total_lines = sum(t for _, _, _, t in results)
    total_uncovered = total_lines - total_covered
    avg_coverage = (sum(r for _, r, _, _ in results) / total_files * 100) if total_files else 0.0
    files_below_80 = sum(1 for _, r, _, _ in results if r*100 < 80)

    with open(out_file, "w", encoding="utf-8", errors="ignore") as f:
        # Phase summary
        phase_headers = ["Generated", "Commit", "Branch", "Job", "Phase", "OS", "Compiler"]

        # Format timestamp as: Date: YYYY-MM-DD || Time: HH:MM AM/PM
        try:
            from datetime import datetime
            ts = datetime.strptime(meta['timestamp'], "%Y-%m-%d %H:%M")
            formatted_ts = f"Date: {ts.strftime('%Y-%m-%d')} || Time: {ts.strftime('%I:%M %p')}"
        except Exception:
            # fallback in case of unexpected format
            formatted_ts = meta['timestamp']

        phase_rows = [[formatted_ts, meta['commit'], meta['branch'], meta['job'], meta['phase'] or "-", meta['os'], meta['compiler']]]

        f.write("# Coverage Report\n\n## Phase Summary\n\n")
        f.write(format_markdown_table(phase_rows, phase_headers) + "\n\n")

        # Coverage summary
        cov_headers = ["Total Files", "Avg Coverage", "Covered Lines", "Uncovered Lines", "Files < 80%"]
        cov_rows = [[str(total_files), f"{avg_coverage:.2f}%", str(total_covered), str(total_uncovered), str(files_below_80)]]
        f.write("## Coverage Summary\n\n")
        f.write(format_markdown_table(cov_rows, cov_headers) + "\n\n")

        # Coverage by file
        headers = ["Rank", "File", "Coverage", "Covered", "Uncovered", "Total", "Level", "Change"]
        rows = []
        sorted_results = sorted(results, key=lambda x: x[1])  # worst → best
        for idx, (fname, rate, covered, total) in enumerate(sorted_results, 1):
            bname = os.path.basename(fname)
            pct = rate * 100
            pct_str = f"{pct:.2f}%"
            uncovered = total - covered
            if pct >= 80:
                level = "High"
            elif pct >= 50:
                level = "Medium"
            elif pct > 0:
                level = "Low"
            else:
                level = "None"
            prev_cov = prev_map.get(bname)
            if prev_cov is not None:
                delta = pct - prev_cov
                delta_str = f"{delta:+.1f}%"
            else:
                delta_str = "0.0%"
            rows.append([str(idx), bname, pct_str, str(covered), str(uncovered), str(total), level, delta_str])
        f.write("## Coverage by File\n\n")
        f.write(format_markdown_table(rows, headers) + "\n")

    print(f"[OK] Markdown report generated: {out_file}")

# -------------------------
# JSON exporter
# -------------------------

def export_json(results, out_file, meta, prev_json_path=None):
    """Export full JSON report with same analytical depth as Markdown."""
    prev_map = {}
    if prev_json_path and os.path.exists(prev_json_path):
        try:
            with open(prev_json_path, "r", encoding="utf-8") as f:
                old = json.load(f)
                for entry in old.get("files", []):
                    prev_map[os.path.basename(entry.get("file", ""))] = float(entry.get("coverage", 0.0))
        except Exception:
            prev_map = {}

    total_files = len(results)
    total_covered = sum(c for _, _, c, _ in results)
    total_lines = sum(t for _, _, _, t in results)
    total_uncovered = total_lines - total_covered
    avg_coverage = (sum(r for _, r, _, _ in results) / total_files * 100) if total_files else 0.0
    files_below_80 = sum(1 for _, r, _, _ in results if r * 100 < 80)

    # Detailed per-file data (sorted worst → best)
    files_data = []
    sorted_results = sorted(results, key=lambda x: x[1])
    for idx, (fname, rate, covered, total) in enumerate(sorted_results, 1):
        pct = rate * 100
        uncovered = total - covered
        if pct >= 80:
            level = "High"
        elif pct >= 50:
            level = "Medium"
        elif pct > 0:
            level = "Low"
        else:
            level = "None"

        prev_cov = prev_map.get(os.path.basename(fname))
        delta = pct - prev_cov if prev_cov is not None else 0.0

        files_data.append({
            "rank": idx,
            "file": os.path.basename(fname),
            "coverage": round(pct, 2),
            "covered": covered,
            "uncovered": uncovered,
            "total": total,
            "level": level,
            "change": round(delta, 1)
        })

    data = {
        "meta": meta,
        "phase_summary": {
            "Generated": meta["timestamp"],
            "Commit": meta["commit"],
            "Branch": meta["branch"],
            "Job": meta["job"],
            "Phase": meta.get("phase", "-"),
            "OS": meta["os"],
            "Compiler": meta["compiler"]
        },
        "coverage_summary": {
            "Total Files": total_files,
            "Avg Coverage": f"{avg_coverage:.2f}%",
            "Covered Lines": total_covered,
            "Uncovered Lines": total_uncovered,
            "Files < 80%": files_below_80
        },
        "files": files_data
    }

    with open(out_file, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2)

    print(f"[OK] Enhanced JSON report generated: {out_file}")

# -------------------------
# Dashboard exporter
# -------------------------

def export_dashboard(results, out_file, meta, prev_json_path=None):
    prev_map = {}
    if prev_json_path and os.path.exists(prev_json_path):
        try:
            with open(prev_json_path, "r", encoding="utf-8") as f:
                old = json.load(f)
                for entry in old.get("files", []):
                    prev_map[os.path.basename(entry.get("file", ""))] = float(entry.get("coverage", 0.0))
        except Exception:
            prev_map = {}

    # ✅ Format timestamp before using meta
    try:
        from datetime import datetime
        ts = datetime.strptime(meta['timestamp'], "%Y-%m-%d %H:%M")
        meta['timestamp'] = f"Date: {ts.strftime('%Y-%m-%d')} || Time: {ts.strftime('%I:%M %p')}"
    except Exception:
        pass  # fallback silently if already formatted

    # Build data for plotting
    files, coverages, levels = [], [], []
    for fname, rate, covered, total in sorted(results, key=lambda x: x[1]):
        pct = rate * 100
        files.append(os.path.basename(fname))
        coverages.append(pct)
        if pct >= 80:
            levels.append("High")
        elif pct >= 50:
            levels.append("Medium")
        elif pct > 0:
            levels.append("Low")
        else:
            levels.append("None")

    # Coverage by file bar chart (dark theme)
    fig_bar = px.bar(
        x=coverages,
        y=files,
        orientation="h",
        color=levels,
        color_discrete_map={"High": "green", "Medium": "orange", "Low": "red", "None": "gray"},
        text=[f"{c:.2f}%" for c in coverages],
        title="Coverage by File (sorted worst → best)",
        template="plotly_dark"
    )
    fig_bar.update_layout(
        yaxis={'categoryorder': 'array', 'categoryarray': files, 'autorange': "reversed"},
        plot_bgcolor="#121212",
        paper_bgcolor="#121212",
        font=dict(color="white"),
        margin=dict(l=100, r=20, t=40, b=40),
        height=600 + len(files) * 15
    )

    # Pie chart (dark theme)
    total_covered = sum(c for _, _, c, _ in results)
    total_lines = sum(t for _, _, _, t in results)
    total_uncovered = total_lines - total_covered
    fig_pie = px.pie(
        names=["Covered", "Uncovered"],
        values=[total_covered, total_uncovered],
        template="plotly_dark",
        color_discrete_sequence=["green", "red"]
    )
    fig_pie.update_layout(
        plot_bgcolor="#121212",
        paper_bgcolor="#121212",
        font=dict(color="white"),
        width=500,
        height=400,
        margin=dict(l=20, r=20, t=40, b=20),
        title=None
    )

    # Dark theme CSS + layout
    css = """
    <style>
      body {
          background-color: #121212;
          color: white;
          font-family: Arial, sans-serif;
          margin: 20px;
      }
      h1, h2 {
          color: white;
      }
      .row {
          display: flex;
          justify-content: space-between;
          align-items: flex-start;
      }
      .column {
          flex: 1;
          margin: 10px;
      }
      ul {
          color: white;
          font-size: 20px;
          line-height: 2;
      }
      .chart-title {
          text-align: center;
          font-size: 22px;
          margin-bottom: 5px;
          margin-top: 0;
      }
    </style>
    """

    # Combine HTML
    html_parts = []
    html_parts.append("<html><head>" + css + "</head><body>")
    html_parts.append("<h1>Coverage Dashboard</h1>")

    html_parts.append("<div class='row'>")

    # Left column: Phase Summary
    html_parts.append("<div class='column'>")
    html_parts.append("<h2>Phase Summary</h2>")
    html_parts.append("<ul>")
    for k, v in meta.items():
        html_parts.append(f"<li><b>{k}</b>: {v}</li>")
    html_parts.append("</ul>")
    html_parts.append("</div>")

    # Right column: Coverage Summary Pie
    html_parts.append("<div class='column'>")
    html_parts.append("<h2 class='chart-title'>Coverage Summary</h2>")
    html_parts.append(fig_pie.to_html(full_html=False, include_plotlyjs='cdn'))
    html_parts.append("</div>")

    html_parts.append("</div>")  # close row

    html_parts.append("<h2>Coverage by File</h2>")
    html_parts.append(fig_bar.to_html(full_html=False, include_plotlyjs=False))

    html_parts.append("</body></html>")

    with open(out_file, "w", encoding="utf-8") as f:
        f.write("\n".join(html_parts))

    print(f"[OK] Dashboard generated (Full Dark Mode): {out_file}")

# -------------------------
# Main
# -------------------------

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python coverage_report_updated.py <coverage.xml> [markdown|json|all] [--archive Phase]")
        sys.exit(1)

    coverage_file = sys.argv[1]
    if "tests" not in coverage_file.lower():
        print("[ERROR] Only test coverage reports are supported (path must include 'tests').")
        sys.exit(1)

    BASE_DIR = os.path.join("reports", "tests")
    ensure_dirs(BASE_DIR)

    mode = sys.argv[2]
    results = analyze_coverage(coverage_file)

    compiler = detect_compiler_suffix()
    suffix = f"_coverage_{compiler}_tests_reports"
    out_file = os.path.join(BASE_DIR, f"latest_report.{mode}")

    phase = None
    if len(sys.argv) >= 5 and sys.argv[3] == "--archive":
        now_str = datetime.datetime.now().strftime("%Y-%m-%d")
        phase = sys.argv[4]
        phase_dir = os.path.join(BASE_DIR, f"{phase.lower()}{suffix}")
        ensure_dirs(phase_dir)
        out_file = os.path.join(phase_dir, f"{phase}_{now_str}.{mode}")

    meta = generate_metadata(phase)
    search_dir = os.path.dirname(out_file)
    prev_json = find_previous_json(search_dir)
    cleanup_json_reports(search_dir)

    if mode == "markdown":
        export_markdown(results, out_file, meta, prev_json_path=prev_json)
    elif mode == "json":
        json_file = out_file if out_file.endswith("_coverage.json") else out_file.replace(f".{mode}", "_coverage.json")
        export_json(results, json_file, meta)
    elif mode == "all":
        md_file = out_file if out_file.endswith(".md") else out_file.replace(".all", ".md")
        export_markdown(results, md_file, meta, prev_json_path=prev_json)
        json_file = out_file if out_file.endswith("_coverage.json") else out_file.replace(".all", "_coverage.json")
        export_json(results, json_file, meta)
        html_file = out_file if out_file.endswith(".html") else out_file.replace(".all", "_dashboard.html")
        export_dashboard(results, html_file, meta, prev_json_path=prev_json)
    else:
        print("Unknown mode. Use: markdown | json | all")
        sys.exit(1)
