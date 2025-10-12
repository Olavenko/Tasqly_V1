from pathlib import Path
from .utils import is_excluded, process_file
from .classify import classify_file_with_context


def walk_tree(start_path: Path, phase: str, slice_name: str, run_ts: str, old_data: list, stats: dict, prefix: str = ""):
    """
    Recursively walk the project directory and collect file metadata.
    
    Responsibilities:
    - Skip excluded files/directories
    - Classify files by type
    - Process files for metadata and tags
    - Build Markdown tree lines
    - Update stats for summary
    - Build phase/slice breakdown for analytics
    """
    json_store, lines = [], []
    entries = [e for e in start_path.iterdir() if not is_excluded(e)]
    entries = sorted(entries, key=lambda e: (not e.is_dir(), e.name.lower()))

    if not entries:
        lines.append(prefix + "(empty)")
        return json_store, lines

    for idx, entry in enumerate(entries):
        connector = "└── " if idx == len(entries) - 1 else "├── "

        if entry.is_dir():
            sub_entries = [e for e in entry.iterdir() if not is_excluded(e)]
            if not sub_entries:
                all_entries = list(entry.iterdir())
                if all_entries:
                    # Directory exists but all contents are ignored
                    lines.append(prefix + connector + entry.name + f"/ (ignored {len(all_entries)} files)")
                else:
                    # Directory is completely empty
                    lines.append(prefix + connector + entry.name + "/ (empty)")
                stats["dirs"] += 1
                continue

            # Non-empty directory: recurse into it
            lines.append(prefix + connector + entry.name + "/")
            extension = "    " if idx == len(entries) - 1 else "│   "
            sub_json, sub_lines = walk_tree(entry, phase, slice_name, run_ts, old_data, stats, prefix + extension)
            json_store.extend(sub_json)
            lines.extend(sub_lines)

        else:
            # Handle regular file
            rel_path = str(entry.relative_to(Path.cwd())).replace("\\", "/")
            ftype = classify_file_with_context(rel_path, entry.name)
            record, _, label = process_file(entry, rel_path, ftype, phase, slice_name, run_ts, old_data)

            if record:
                # Add record to output
                json_store.append(record)

                # Update counters
                stats["files"] += 1
                stats["types"][ftype] = stats["types"].get(ftype, 0) + 1

                # ✅ Build phase breakdown for summary
                if "phase_breakdown" not in stats:
                    stats["phase_breakdown"] = []

                stats["phase_breakdown"].append({
                    "file": rel_path,
                    "phase": record["phase"],
                    "slice": record["slice"],
                    "status": record["status"],
                })

                # Add Markdown label
                lines.append(prefix + connector + label)

    stats["dirs"] += 1
    return json_store, lines
