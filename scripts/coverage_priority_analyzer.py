#!/usr/bin/env python3
# -*- coding: utf-8 -*-
r"""
==========================================================
Tasqly Coverage Priority Analyzer (Standalone Utility)
==========================================================

📄 Overview:
------------
This standalone script analyzes Cobertura-style `coverage.xml` reports
to produce a **detailed prioritized breakdown** of uncovered source lines.
It provides smart ranking, context-based classification, and intelligent
path resolution to help developers improve test coverage efficiently.

🎯 Purpose:
-----------
To enhance test productivity by generating an **intelligent coverage map**
that identifies critical untested code paths, suggests test types (Unit/Integration),
and provides actionable test recommendations.

⚙️ Core Features:
--------------------------
1- **Smart Path Resolution**:
     - Auto-detects project root and source directories
     - Handles both absolute and relative paths in coverage reports
     - Supports custom source path mapping

2- **Intelligent Analysis**:
     - Extracts function context around uncovered lines
     - Applies heuristic scoring to prioritize test needs
     - Suggests appropriate test types (Unit/Integration)
     - Identifies error handling, database calls, and critical paths

3- **Comprehensive Reporting**:
     - Generates detailed Markdown reports
     - Outputs structured JSON for CI/CD integration
     - Provides compact line range summaries
     - Ranks files by test priority

📊 Output Formats:
------------------
1. **Markdown Report** (`*.md`):
   - Detailed prioritized table with test suggestions
   - Compact line range summaries per file
   - Color-coded priority indicators
   
   Example:
   ```markdown
   | Rank | File | Line(s) | Function | Score | Tags | Suggested Test | Type |
   |------|------|---------|----------|-------|------|----------------|------|
   | 1 | P1_S2_PostgresTaskRepository.cpp | 132 | insertTask | 92 | error-handling, db-call | Simulate DB connection failure | Integration |
   | 2 | P1_S2_PostgresTaskRepository.cpp | 175 | updateTask | 85 | branching, return-error | Cover success/failure paths | Unit |
   ```

2. **JSON Output** (`*.json`):
   - Structured data for CI/CD integration
   - Full context for each uncovered line
   - Compatible with automated tooling

🧠 Heuristic Analysis:
----------------------
Prioritizes test needs using weighted patterns:

| Pattern Match | Tag | Score | Test Type |
|---------------|-----|-------|-----------|
| `throw`, `catch`, `try` | error-handling | +30 | Unit/Integration |
| `query`, `exec`, `transaction` | db-call | +25 | Integration |
| `http`, `api`, `network` | api-call | +25 | Integration |
| `if`, `switch`, `&&` | branching | +10 | Unit |
| `log`, `logger` | logging | +3 | Unit |
| `TODO`, `FIXME` | todo | +10 | Unit |

🧩 Usage:
---------
```bash
# Basic usage
py -3.13 scripts\coverage_priority_analyzer.py coverage.xml output.md

# Example
py -3.13 scripts\coverage_priority_analyzer.py reports\coverage\tests\coverage.xml reports\tests\phase1_extracted\prioritized_uncovered.md

# With options
py -3.13 scripts\coverage_priority_analyzer.py \
    --max-per-file 50 \
    --min-score 20 \
    --output-format json \
    coverage.xml \
    output
```

⚙️ Options:
-----------
- `--max-per-file N`   Limit entries per file (default: 999)
- `--min-score S`      Minimum score to include (default: 0)
- `--output-format FMT` Output format: md, json (default: md)
- `--project-root DIR`  Manual project root directory

📦 Requirements:
---------------
- Python ≥ 3.8
- Cobertura-compatible `coverage.xml`
- Source files accessible at reported paths

📅 Version:
-----------
v1.1 — 2025-11-11
Author: Tasqly QA Automation Team

🔍 Purpose:
-----------
Enhance test coverage quality through intelligent analysis and
automated test recommendations for the Tasqly development pipeline.

==========================================================
"""

import xml.etree.ElementTree as ET
import os
import sys
import re
import json
import argparse
from collections import defaultdict

# -------------------------
# Utilities
# -------------------------

def parse_args():
    p = argparse.ArgumentParser(description="Prioritize uncovered lines from cobertura coverage.xml")
    p.add_argument("coverage_xml", help="Path to coverage.xml")
    p.add_argument("out_md", help="Output Markdown file")
    p.add_argument("--max-per-file", type=int, default=999, help="Max entries per file in output")
    p.add_argument("--min-score", type=int, default=0, help="Minimum score to include")
    return p.parse_args()

def load_coverage_xml(path):
    tree = ET.parse(path)
    root = tree.getroot()
    classes = []
    for cls in root.findall(".//class"):
        fname = cls.get("filename", "")
        name = os.path.basename(fname)
        try:
            line_rate = float(cls.get("line-rate", "0"))
        except Exception:
            line_rate = 0.0
        # collect uncovered line numbers
        uncovered = []
        for line in cls.findall(".//line"):
            hits = line.attrib.get("hits")
            if hits is None:
                continue
            try:
                if int(hits) == 0:
                    uncovered.append(int(line.attrib.get("number")))
            except Exception:
                continue
        classes.append({
            "filename": fname,
            "name": name,
            "line_rate": line_rate,
            "uncovered": sorted(set(uncovered))
        })
    return classes

def read_source(src_path):
    try:
        with open(src_path, "r", encoding="utf-8", errors="ignore") as f:
            return f.read().splitlines()
    except Exception:
        return None

def detect_project_root(start_path="."):
    """
    Automatically detect project source root by scanning for known directories.
    Priority:
      1. src/
      2. source/
      3. app/
      4. tasqly/ (fallback)
    """
    candidates = ["src", "source", "app", "Tasqly", "tasqly"]
    for root, dirs, files in os.walk(start_path):
        for d in candidates:
            if d in dirs:
                return os.path.join(root, d)
    return start_path

def find_src_path_from_map(path_map, basename, search_root=None):
    """
    Try to resolve the real path for a file:
    1. Use the path from XML if it exists.
    2. Otherwise search recursively in `search_root`.
    """
    # Auto-detect project root if not provided
    if search_root is None or not os.path.exists(search_root):
        search_root = detect_project_root(".")
    
    # 1️-Try the path directly from XML map
    for k, v in path_map.items():
        if k == basename and os.path.exists(v):
            return v
        if v.endswith(basename) and os.path.exists(v):
            return v

    # 2️- Try local cwd
    if os.path.exists(basename):
        return os.path.abspath(basename)

    # 3️- Search recursively under project src/
    for root, dirs, files in os.walk(search_root):
        if basename in files:
            return os.path.join(root, basename)

    # 4️- Not found
    return None


def collapse_ranges(sorted_numbers):
    """Convert [1,2,3,5,7,8] -> ['1–3','5','7–8']"""
    if not sorted_numbers:
        return []
    ranges = []
    start = prev = sorted_numbers[0]
    for n in sorted_numbers[1:]:
        if n == prev + 1:
            prev = n
            continue
        # flush
        if start == prev:
            ranges.append(str(start))
        else:
            ranges.append(f"{start}–{prev}")
        start = prev = n
    if start == prev:
        ranges.append(str(start))
    else:
        ranges.append(f"{start}–{prev}")
    return ranges

# -------------------------
# Context extraction / heuristics
# -------------------------

FUNC_SIG_RE = re.compile(r'([A-Za-z_][\w:\<\>\s\*&]+)\s+([A-Za-z_]\w*)\s*\([^;{]*\)\s*\{', re.M)

def extract_function_name(lines, line_number, back_search_limit=300, forward_limit=5):
    """
    Try to find nearest function signature above the line.
    lines: list of source lines (0-based indexing)
    line_number: 1-based line number
    """
    if not lines:
        return "unknown"
    idx = max(0, line_number - 1)
    start = max(0, idx - back_search_limit)
    snippet = "\n".join(lines[start: idx + forward_limit])
    m = FUNC_SIG_RE.search(snippet)
    if m:
        return m.group(2)
    # fallback: look upwards line-by-line for '::name(' or 'name(' patterns
    for i in range(idx, start-1, -1):
        line = lines[i].strip()
        # skip comments and braces
        if line.endswith(");"):
            continue
        simple = re.match(r'([A-Za-z_]\w*)\s*\([^;{]*\)\s*(const)?\s*(->\s*\w+)?\s*\{?', line)
        if simple:
            return simple.group(1)
    return "unknown"

def score_and_tag(line_text, context_text):
    """
    Return (score:int, tags:list[str], suggestion:str, test_type:str)
    Heuristics are lightweight and additive.
    """
    score = 0
    tags = []
    suggestion = "General behavior verification"
    text = (line_text or "") + "\n" + (context_text or "")
    lower = text.lower()

    # high-priority error returns / failure paths
    if re.search(r'\breturn\b.*\b(error|failure|err|false|nullptr)\b', lower) or 'result::failure' in lower or 'make_error' in lower:
        tags.append("error-handling")
        score += 30
        suggestion = "Simulate failing condition; expect error result"

    # db related
    if re.search(r'\b(query|exec|prepare|transaction|commit|rollback|pqexec|sqlite3_)\b', lower):
        tags.append("db-call")
        score += 25
        suggestion = "Mock DB (or use test DB) to exercise failing/successful DB path"

    # exception/catch
    if 'catch' in lower or 'exception' in lower:
        tags.append("catch")
        score += 15
        suggestion = "Trigger exception and verify recovery/fallback"

    # default/switch branch
    if re.search(r'\bdefault\b|\bswitch\b', lower):
        tags.append("default-branch")
        score += 15
        suggestion = "Cover default path with unexpected input"

    # branching / complex condition
    if re.search(r'&&|\|\||\bif\b|\belse\b', lower):
        tags.append("branching")
        score += 10
        suggestion = "Cover both sides of the conditional"

    # logging only
    if 'log(' in lower or 'logger' in lower or 'spdlog' in lower:
        tags.append("logging")
        score += 3
        if suggestion == "General behavior verification":
            suggestion = "Verify log invoked (or focus on logic around logging)"

    # TODO or FIXME
    if 'todo' in lower or 'fixme' in lower:
        tags.append("todo")
        score += 10
        suggestion = "Address TODO / add targeted test"

    # assert / unreachable
    if 'assert(' in lower or 'std::terminate' in lower or 'std::abort' in lower:
        tags.append("assert")
        score -= 10
        suggestion = "Usually not covered by unit tests; deprioritize"

    # determine test type
    integration_tags = {"db-call", "network", "filesystem", "api", "transaction"}
    test_type = "Integration" if any(t in tags for t in integration_tags) else "Unit"

    # floor score
    if score < 0:
        score = 0

    return score, tags, suggestion, test_type

# -------------------------
# Main analyzer
# -------------------------

def build_path_map(classes):
    # map basename -> full path (last occurrence wins)
    pm = {}
    for cls in classes:
        fname = cls.get("filename") if isinstance(cls, dict) else cls.get("filename", "")
        if not fname:
            continue
        b = os.path.basename(fname)
        pm[b] = fname
    return pm

def analyze(classes, coverage_xml_path, max_per_file=999, min_score=0):
    path_map = {}
    # build path map from classes list
    for c in classes:
        path_map[c['name']] = c['filename']
    results = []
    # sort by ascending coverage rate (so worst files first)
    classes_sorted = sorted(classes, key=lambda x: x.get("line_rate", 0.0))
    for rank_idx, cls in enumerate(classes_sorted, start=1):
        name = cls['name']
        full_path = find_src_path_from_map(path_map, name, search_root="src")
        uncovered = cls.get("uncovered", [])
        if not uncovered:
            continue
        # read source if possible
        lines = None
        if full_path and os.path.exists(full_path):
            lines = read_source(full_path)
        entries = []
        #===============================================

        for ln in uncovered:
            # --------------------------------------------
            # Smart Context Extraction (structural awareness)
            # --------------------------------------------
            line_text = ""
            context_text = ""
            if lines:
                total_lines = len(lines)
                if 1 <= ln <= total_lines:
                    line_text = lines[ln - 1].strip()
                
                # dynamic context search
                start = max(0, ln - 1)
                end = min(total_lines, ln + 1)
                
                # expand upwards to find start of block
                open_braces = 0
                for i in range(ln - 1, -1, -1):
                    if "{" in lines[i]:
                        open_braces += lines[i].count("{")
                    if "}" in lines[i]:
                        open_braces -= lines[i].count("}")
                    if open_braces > 0:
                        start = i
                        break
                    if re.search(r'\b(if|for|while|try|switch|catch)\b', lines[i]):
                        start = max(0, i - 1)
                        break
                
                # expand downwards to find block end
                close_braces = 0
                for i in range(ln - 1, total_lines):
                    if "{" in lines[i]:
                        close_braces += lines[i].count("{")
                    if "}" in lines[i]:
                        close_braces -= lines[i].count("}")
                    if close_braces < 0 or re.search(r'\b(return|throw|break|continue)\b', lines[i]):
                        end = min(total_lines, i + 1)
                        break

                # final context window
                context_text = "\n".join(lines[start:end])
        #===============================================
            # find function name
            func = extract_function_name(lines, ln) if lines else "unknown"
            score, tags, suggestion, test_type = score_and_tag(line_text, context_text)
            # -------------------------------
            # 🔹 Determine Risk Level
            if score >= 80:
                risk = "Critical"
            elif score >= 50:
                risk = "High"
            elif score >= 20:
                risk = "Medium"
            else:
                risk = "Low"
        
            # -------------------------------
            # 🔹 Determine Complexity Type
            lower = (line_text + " " + context_text).lower()
            complexity_parts = []
            if any(x in lower for x in ["query", "exec", "transaction", "commit", "rollback", "db"]):
                complexity_parts.append("DB call")
            if any(x in lower for x in ["if", "else", "switch", "case", "while", "for", "&&", "||"]):
                complexity_parts.append("Branching")
            if any(x in lower for x in ["throw", "catch", "exception", "error"]):
                complexity_parts.append("Error handling")
            if any(x in lower for x in ["log", "logger", "spdlog"]):
                complexity_parts.append("Logging")
            if not complexity_parts:
                complexity_parts.append("Simple logic")
            complexity = " + ".join(complexity_parts)
        
            # -------------------------------
            # 🔹 Confidence Evaluation
            if not lines:
                confidence = "Low"
            elif func == "unknown":
                confidence = "Medium"
            else:
                confidence = "High"
            if score < min_score:
                continue
            entries.append({
                "rank": rank_idx,
                "file": name,
                "full_path": full_path,
                "line": ln,
                "func": func,
                "score": score,
                "risk": risk,
                "complexity": complexity,
                "confidence": confidence,
                "tags": tags,
                "suggestion": suggestion,
                "type": test_type,
                "line_text": line_text
            })

        # sort entries within file by score desc, then line asc
        entries = sorted(entries, key=lambda e: (-e['score'], e['line']))
        if entries:
            # limit
            entries = entries[:max_per_file]
            results.extend(entries)
    # Sort by: file coverage rank → score (desc) → line (asc)
    results = sorted(results, key=lambda e: (e['rank'], -e['score'], e['line']))
    return results

# -------------------------
# Output formatting
# -------------------------

def make_md_table(rows, headers):
    return format_table_md(headers, rows)

def format_table_md(headers, rows):
    # compute widths
    col_widths = [len(h) for h in headers]
    for r in rows:
        for i, c in enumerate(r):
            col_widths[i] = max(col_widths[i], len(str(c)))
    hdr = "| " + " | ".join(headers[i].ljust(col_widths[i]) for i in range(len(headers))) + " |"
    sep = "|-" + "-|-".join("-" * col_widths[i] for i in range(len(headers))) + "-|"
    lines = [hdr, sep]
    for r in rows:
        line = "| " + " | ".join(str(r[i]).ljust(col_widths[i]) for i in range(len(headers))) + " |"
        lines.append(line)
    return "\n".join(lines)

# -------------------------
# Markdown Output
# -------------------------
def generate_markdown(out_md_path, prioritized_entries):
    with open(out_md_path, "w", encoding="utf-8") as f:
        f.write("# Files Below 100% — Prioritized Uncovered Lines\n\n")
        if not prioritized_entries:
            f.write("_All files fully covered or nothing met the threshold._\n")
            return
        headers = ["Rank", "File", "Line", "Func", "Score", "Risk", "Complexity", "Tags", "Suggested Test", "Type", "Confidence"]
        rows = []
        # group by file for better visual
        grouped = defaultdict(list)
        for e in prioritized_entries:
            grouped[e['file']].append(e)
        # we want to preserve the ordering in prioritized_entries (already sorted)
        for e in prioritized_entries:
            tag_str = ", ".join(e['tags']) if e['tags'] else "-"
            # collapse to range string (but here we output single line number per row)
            rows.append([
                str(e['rank']),
                e['file'],
                str(e['line']),
                e['func'],
                str(e['score']),
                e['risk'],
                e['complexity'],
                tag_str,
                e['suggestion'],
                e['type'],
                e['confidence']
            ])

        f.write(make_md_table(rows, headers))
        f.write("\n\n")
        # Also add a compact grouped view: file -> collapsed ranges
        f.write("## Compact view (collapsed ranges)\n\n")
        headers2 = ["Rank", "File", "Uncovered Lines (ranges)"]
        rows2 = []
        # compute uncovered lists per file from prioritized entries
        per_file = defaultdict(list)
        for e in prioritized_entries:
            per_file[e['file']].append(e['line'])
        # keep file ordering by first appearance in prioritized_entries
        seen = []
        for e in prioritized_entries:
            if e['file'] not in seen:
                seen.append(e['file'])
        for idx, fname in enumerate(seen, 1):
            ls = sorted(set(per_file[fname]))
            ranges = collapse_ranges(ls)
            rows2.append([str(idx), fname, ", ".join(ranges)])
        f.write(make_md_table(rows2, headers2))
        f.write("\n")
        # --------------------------------------------
        # Summary Section
        # --------------------------------------------
        total_files = len(set(e['file'] for e in prioritized_entries))
        total_lines = len(prioritized_entries)
        high_risk = sum(1 for e in prioritized_entries if e['risk'] in ("Critical", "High"))
        avg_score = round(sum(e['score'] for e in prioritized_entries) / total_lines, 2) if total_lines > 0 else 0.0

        # احسب أكثر نوع تعقيد مكرر (الأكثر شيوعًا)
        complexity_counts = defaultdict(int)
        for e in prioritized_entries:
            complexity_counts[e['complexity']] += 1
        if complexity_counts:
            top_complexity = max(complexity_counts, key=complexity_counts.get)
        else:
            top_complexity = "N/A"

        f.write("\n## Summary\n\n")
        f.write(f"Analyzed files: {total_files}\n")
        f.write(f"Uncovered lines analyzed: {total_lines}\n")
        f.write(f"High-risk lines (Score ≥ 50): {high_risk}\n")
        f.write(f"Average Score: {avg_score}\n")
        f.write(f"Most common complexity: {top_complexity}\n")
        # --------------------------------------------

# -------------------------
# JSON Output
# -------------------------
def generate_json(out_json_path, prioritized_entries):
    data = {
        "summary": {},
        "entries": prioritized_entries
    }

    # compute summary like in generate_markdown
    total_files = len(set(e['file'] for e in prioritized_entries))
    total_lines = len(prioritized_entries)
    high_risk = sum(1 for e in prioritized_entries if e['risk'] in ("Critical", "High"))
    avg_score = round(sum(e['score'] for e in prioritized_entries) / total_lines, 2) if total_lines > 0 else 0.0
    complexity_counts = defaultdict(int)
    for e in prioritized_entries:
        complexity_counts[e['complexity']] += 1
    top_complexity = max(complexity_counts, key=complexity_counts.get) if complexity_counts else "N/A"

    data["summary"] = {
        "analyzed_files": total_files,
        "uncovered_lines_analyzed": total_lines,
        "high_risk_lines": high_risk,
        "average_score": avg_score,
        "most_common_complexity": top_complexity
    }

    # write out
    with open(out_json_path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=4, ensure_ascii=False)

# -------------------------
# Entrypoint
# -------------------------

def main():
    args = parse_args()
    coverage_xml = args.coverage_xml
    out_md = args.out_md

    if not os.path.exists(coverage_xml):
        print(f"[ERROR] coverage.xml not found: {coverage_xml}")
        sys.exit(2)

    classes = load_coverage_xml(coverage_xml)
    # build path map for robust discovery
    path_map = {c['name']: c['filename'] for c in classes}
    prioritized = analyze(classes, coverage_xml, max_per_file=args.max_per_file, min_score=args.min_score)
    generate_markdown(out_md, prioritized)

    # generate parallel JSON file (same base name)
    out_json = os.path.splitext(out_md)[0] + ".json"
    generate_json(out_json, prioritized)
    
    print(f"[OK] Prioritized report written to: {out_md}")
    print(f"[OK] JSON report written to: {out_json}")
    print(f"[INFO] {len(prioritized)} prioritized uncovered entries generated.")


if __name__ == "__main__":
    main()
