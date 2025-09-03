#!/usr/bin/env python3
"""
🧱 File: gen_adr_index.py
-------------------------
📌 Purpose   : Auto-generate docs/adr/index-adr.md from ADR files + phase/slice folders
🧱 Layer     : Utility Script
👤 Author    : Mohamed Ali
🗓️ Created   : 2025-09-05
🗓️ Updated   : 2025-09-05

🧠 Description:
Scans docs/adr/ for ADR files and phase/slice subfolders.
Generates grouped Markdown tables per Slice with ID, Title, Status, Last Modified, and File.
"""

from pathlib import Path
from datetime import datetime
import re

ROOT = Path(__file__).resolve().parent.parent
ADR_DIR = ROOT / "docs" / "adr"
OUTPUT = ADR_DIR / "index-adr.md"


def extract_title_and_status(file: Path):
    """Extract ADR title + status from file content"""
    title = file.stem
    status = "Draft"
    try:
        with file.open("r", encoding="utf-8") as f:
            for line in f:
                if line.strip().startswith("# "):  # first heading = title
                    title = line.strip("# ").strip()
                if "Status:" in line:
                    raw_status = line.split(":", 1)[1].strip()
                    # 🧹 Remove markdown bold/italic markers like **Accepted**, *Planned*
                    status = re.sub(r"[*_]+", "", raw_status).strip()
    except Exception:
        pass
    return title, status


def collect_adrs():
    """Return dict: {group_name: [rows]}"""
    groups = {}

    # ADR root files (ungrouped)
    root_group = "root"
    groups[root_group] = []
    for f in sorted(ADR_DIR.glob("*.md")):
        if f.name == "index-adr.md":
            continue
        if f.name[0:4].isdigit():
            adr_id = f.name.split("-")[0]
            title, status = extract_title_and_status(f)
            last_modified = datetime.fromtimestamp(f.stat().st_mtime).strftime("%Y-%m-%d")
            groups[root_group].append([adr_id, title, status, last_modified,
                                       f"[{f.name}]({f.name})"])

    # Phase/Slice groups
    for phase_dir in sorted(ADR_DIR.glob("phase*")):
        if not phase_dir.is_dir():
            continue
        for slice_dir in sorted(phase_dir.glob("slice*")):
            if not slice_dir.is_dir():
                continue
            group_name = f"{phase_dir.name}/{slice_dir.name}"
            groups[group_name] = []
            for f in sorted(slice_dir.glob("*.md")):
                adr_id = f.name.split("-")[0]
                title, status = extract_title_and_status(f)
                last_modified = datetime.fromtimestamp(f.stat().st_mtime).strftime("%Y-%m-%d")
                relpath = f.relative_to(ADR_DIR)
                groups[group_name].append([adr_id, title, status, last_modified,
                                           f"[{f.name}]({relpath.as_posix()})"])
    return groups


def format_table(rows):
    headers = ["ID", "Title", "Status", "Last Modified", "File"]

    # Find max width for each column
    col_widths = [len(h) for h in headers]
    for row in rows:
        for i, cell in enumerate(row):
            col_widths[i] = max(col_widths[i], len(cell))

    header_line = "| " + " | ".join(h.ljust(col_widths[i]) for i, h in enumerate(headers)) + " |"
    separator = "|-" + "-|-".join("-" * col_widths[i] for i in range(len(headers))) + "-|"

    formatted_rows = []
    for row in rows:
        formatted_row = "| " + " | ".join(str(cell).ljust(col_widths[i]) for i, cell in enumerate(row)) + " |"
        formatted_rows.append(formatted_row)

    return [header_line, separator] + formatted_rows


def main():
    groups = collect_adrs()

    header = [
        "# ADR Index — Tasqly Project",
        "",
        "This directory contains Architecture Decision Records (ADRs) for the Tasqly project.",
        "Each ADR documents a significant architectural or tooling decision.",
        "",
        "## ADR List",
        "",
    ]

    sections = []
    # root ADRs (if any)
    if groups.get("root"):
        sections.append("### Root ADRs\n")
        sections.extend(format_table(groups["root"]))
        sections.append("")

    # phases/slices
    for group, rows in groups.items():
        if group == "root":
            continue
        sections.append(f"### {group}\n")
        sections.extend(format_table(rows))
        sections.append("")

    legend = [
        "---",
        "",
        "## Status Legend",
        "- **Accepted** → Implemented and in use.",
        "- **Planned** → Approved but not yet implemented fully.",
        "- **Proposed** → Under discussion.",
        "- **Draft** → Created but not yet reviewed."
    ]

    content = "\n".join(header + sections + legend) + "\n"
    OUTPUT.write_text(content, encoding="utf-8")
    print(f"✅ ADR index written to {OUTPUT}")


if __name__ == "__main__":
    main()
