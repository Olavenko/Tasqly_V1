#!/usr/bin/env python3
"""
🧱 File: gen_structure.py
-------------------------
📌 Purpose   : Generate structured PROJECT_STRUCTURE.md with Root Layout + Notes + Build Policy
🧱 Layer     : Utility Script
👤 Author    : Mohamed Ali
🗓️ Created   : 2025-09-04
🗓️ Updated   : 2025-09-05

🧠 Description:
Generates a full documentation-style file with:
1. Header + Table of Contents
2. Root Layout (auto-generated project tree, .gitignore-aware + hard excludes)
3. Notes
4. Build Toolchain Policy (summary)
"""

import os
from pathlib import Path
from datetime import datetime

try:
    from pathspec import PathSpec
except ImportError:
    print("⚠️ Missing dependency: install with `pip install pathspec`")
    exit(1)

# 🔧 Project root
ROOT = Path(__file__).resolve().parent.parent
OUTPUT = ROOT / "Project_Structure.md"

# 📂 Load .gitignore if available
GITIGNORE_FILE = ROOT / ".gitignore"
gitignore_spec = None
if GITIGNORE_FILE.exists():
    with GITIGNORE_FILE.open("r", encoding="utf-8") as f:
        gitignore_spec = PathSpec.from_lines("gitwildmatch", f)

# Hardcoded excludes (always skipped)
HARDCODED_EXCLUDES = {".git", "build", "__pycache__", ".idea", ".vs"}

file_count = 0
dir_count = 0
stats = {"C++": 0, "QML": 0, "Docs": 0, "Build": 0, "Other": 0}


def is_excluded(path: Path) -> bool:
    """Check if a path should be excluded"""
    if path.name in HARDCODED_EXCLUDES:
        return True
    if gitignore_spec and gitignore_spec.match_file(str(path.relative_to(ROOT))):
        return True
    return False


def describe_file(name: str) -> str:
    """Return short description and update stats"""
    lower = name.lower()
    if lower.endswith(".cpp"):
        stats["C++"] += 1
        return "[C++ Source]"
    if lower.endswith(".h"):
        stats["C++"] += 1
        return "[C++ Header]"
    if lower.endswith(".qml"):
        stats["QML"] += 1
        return "[Root QML UI]" if lower == "main.qml" else "[QML UI]"
    if lower.endswith(".md"):
        stats["Docs"] += 1
        return "[Markdown Doc]"
    if lower.endswith(".puml"):
        stats["Docs"] += 1
        return "[UML Diagram]"
    if lower.endswith(".yml") or lower.endswith(".yaml"):
        stats["Build"] += 1
        return "[CI Config]"
    if lower.endswith(".cmake") or name == "CMakeLists.txt":
        stats["Build"] += 1
        return "[Build Config]"
    if "readme" in lower:
        stats["Docs"] += 1
        return "[Readme]"
    if lower == "main.cpp":
        stats["C++"] += 1
        return "[App Entry Point]"
    stats["Other"] += 1
    return ""


def print_tree(start_path: Path, prefix: str = "") -> list[str]:
    """Recursive tree printer (dirs first, then files, both sorted alphabetically)"""
    global file_count, dir_count
    entries = [e for e in start_path.iterdir() if not is_excluded(e)]
    # Sort: dirs first, then files, alphabetical inside each group
    entries = sorted(entries, key=lambda e: (not e.is_dir(), e.name.lower()))

    lines = []
    for idx, entry in enumerate(entries):
        connector = "└── " if idx == len(entries) - 1 else "├── "
        if entry.is_dir():
            dir_count += 1
            lines.append(prefix + connector + entry.name + "/")
            extension = "    " if idx == len(entries) - 1 else "│   "
            lines.extend(print_tree(entry, prefix + extension))
        else:
            file_count += 1
            desc = describe_file(entry.name)
            label = entry.name + ((" " + desc) if desc else "")
            lines.append(prefix + connector + label)
    return lines


def main():
    global file_count, dir_count
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    header = [
        "# 📂 Project Structure — Tasqly_V1 (Auto-generated Snapshot)",
        "",
        f"Generated on **{timestamp}** using `scripts/gen_structure.py`.",
        "",
        "## 📑 Table of Contents",
        "1. [Root Layout](#root-layout)",
        "2. [Notes](#notes)",
        "3. [Build Toolchain Policy (Summary)](#build-toolchain-policy-summary)",
        "",
        "---",
        "",
        "## Root Layout",
        ROOT.name + "/"
    ]

    body = print_tree(ROOT)

    # 📊 Summary with breakdown
    summary = [
        "",
        f"📊 Summary: {dir_count} directories, {file_count} files",
        f"- C++ Files: {stats['C++']}",
        f"- QML Files: {stats['QML']}",
        f"- Docs: {stats['Docs']}",
        f"- Build/Config: {stats['Build']}",
        f"- Other: {stats['Other']}",
        "",
        "---",
        "",
        "## Notes",
        "⚡ This snapshot is auto-generated.",
        "It only reflects the current filesystem and may differ from design docs (e.g., ADRs).",
        "",
        "---",
        "",
        "## Build Toolchain Policy (Summary)",
        "",
        "* **Windows (Baseline):** MSYS2 MinGW-w64 GCC 15.2 + Ninja + Qt-MinGW",
        "* **Linux (Baseline):** GCC / Clang (latest stable) + Qt-Linux (ABI-compatible with MinGW).",
        "* **Optional (Windows-only):** MSVC (Visual Studio 2019/2022) + Qt-MSVC, maintained in the `msvc-only` branch.",
        "* **Qt ABI constraint:**",
        "",
        "  * Qt-MinGW ↔ MinGW/GCC/Clang only (cross-platform baseline)",
        "  * Qt-MSVC ↔ MSVC only (Windows-only branch)",
        "  * ❌ Mixing Qt-MinGW with MSVC is not supported (ABI conflict).",
        "* **Sanitizers:** Enabled on Linux only, disabled on Windows.",
        "",
        "**Build Method (Windows baseline):**",
        "",
        "```powershell",
        "rmdir /S /Q build",
        "cmake --preset=mingw-debug",
        "cmake --build --preset=build-mingw-debug",
        "ctest --preset=test-mingw-debug",
        "```",
        "",
        "**Build Method (Windows optional, MSVC-only):**",
        "",
        "```powershell",
        "rmdir /S /Q build",
        "cmake --preset=msvc-debug",
        "cmake --build --preset=msvc-debug",
        "ctest --preset=msvc-debug",
        "```",
        "",
        "**Build Method (Linux baseline):**",
        "",
        "```bash",
        "rm -rf build",
        "cmake --preset=gcc-debug    # or clang-debug",
        "cmake --build --preset=build-gcc-debug",
        "ctest --preset=test-gcc-debug --output-on-failure",
        "```"
    ]

    content = "\n".join(header + body + summary) + "\n"
    OUTPUT.write_text(content, encoding="utf-8")
    print(f"✅ Project structure written to {OUTPUT}")


if __name__ == "__main__":
    main()
