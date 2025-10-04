#!/usr/bin/env python3
import os
import re
from pathlib import Path
from datetime import datetime

ROOT = Path(__file__).resolve().parent.parent
UML_DIR = ROOT / "docs" / "uml"
README = UML_DIR / "Diagrams-Readme.md"


def build_tree(start_path, prefix=""):
    """Build a folder tree with file type labels."""
    lines = []
    entries = sorted(os.listdir(start_path))
    entries = [e for e in entries if not e.startswith(".")]

    for idx, entry in enumerate(entries):
        full_path = os.path.join(start_path, entry)
        connector = "└── " if idx == len(entries) - 1 else "├── "

        label = entry
        if os.path.isfile(full_path):
            if entry.endswith(".puml"):
                label += " [UML Diagram]"
            elif entry.endswith(".sh"):
                label += " [Script]"
            elif entry.endswith(".jar"):
                label += " [Tool]"
            elif entry.lower().endswith(".md"):
                label += " [Markdown Doc]"
            elif entry.endswith(".py"):
                label += " [Python Script]"

        lines.append(f"{prefix}{connector}{label}")

        if os.path.isdir(full_path):
            extension = "    " if idx == len(entries) - 1 else "│   "
            lines.extend(build_tree(full_path, prefix + extension))

    return lines


def collect_stats(start_path):
    """Count files, directories and return with timestamp."""
    total_files, total_dirs = 0, 0
    for _, dirs, files in os.walk(start_path):
        total_dirs += len(dirs)
        total_files += len(files)
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    return total_files, total_dirs, timestamp


def update_readme():
    with open(README, "r", encoding="utf-8") as f:
        content = f.read()

    # Capture block from header to next section
    pattern = r"## 📂 Folder Structure[\s\S]*?(?=(\n## |\Z))"
    match = re.search(pattern, content)
    if not match:
        raise RuntimeError("Folder Structure block not found in Diagrams-Readme.md")

    # Build new tree
    tree_lines = build_tree(str(UML_DIR))
    tree_str = "\n".join(tree_lines)

    total_files, total_dirs, timestamp = collect_stats(UML_DIR)
    stats = (
        f"📊 Summary\n"
        f"- Directories: {total_dirs}\n"
        f"- Files: {total_files}\n"
        f"⏱️ Updated: {timestamp}\n"
    )

    # Wrap between --- markers
    new_block = (
        "## 📂 Folder Structure\n\n"
        "---\n\n"
        + tree_str + "\n\n"
        "---\n\n"
        + stats + "\n---\n"
    )

    updated = re.sub(pattern, new_block, content)

    with open(README, "w", encoding="utf-8") as f:
        f.write(updated)

    print("✅ Updated Diagrams-Readme.md with new folder tree and stats.")


if __name__ == "__main__":
    update_readme()
