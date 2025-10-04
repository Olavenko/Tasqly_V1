import json
from pathlib import Path
from colorama import Fore, Style

# Base output directories
ROOT = Path(__file__).resolve().parent.parent.parent
OUTPUT_DIR = ROOT / "reports" / "project_tree"
SNAPSHOT_DIR = OUTPUT_DIR / "snapshots"
OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
SNAPSHOT_DIR.mkdir(parents=True, exist_ok=True)

OUTPUT_MD = OUTPUT_DIR / "Project_Tree.md"

def export_outputs(run_ts, run_stamp, header, body, stats, json_store, phase, slice_name, compared_snapshot=None):
    """
    Export project tree snapshot:
    - Markdown (latest in root)
    - Snapshot (MD + JSON in snapshots/, keep only last 2)
    """

    # Build markdown content
    content = "\n".join(header + body + _build_summary(stats)) + "\n"

    # --- Write latest Markdown in root ---
    OUTPUT_MD.write_text(content, encoding="utf-8")

    # --- Prepare snapshot filenames ---
    phase_clean = phase if phase else "PhaseX"
    slice_clean = slice_name if slice_name else "SliceX"

    SNAPSHOT_MD = SNAPSHOT_DIR / f"Project_Tree_{phase_clean}_{slice_clean}_{run_stamp}.md"
    SNAPSHOT_JSON = SNAPSHOT_DIR / f"Project_Tree_{phase_clean}_{slice_clean}_{run_stamp}.json"

    # --- Write snapshots ---
    SNAPSHOT_MD.write_text(content, encoding="utf-8")
    SNAPSHOT_JSON.write_text(json.dumps(json_store, indent=2), encoding="utf-8")

    # --- Cleanup: keep only last 2 snapshots ---
    _cleanup_old_snapshots(keep=2)

    # --- Console messages ---
    if compared_snapshot:
        print(Fore.YELLOW + f"📊 Compared against: {compared_snapshot.name}" + Style.RESET_ALL)

    print(Fore.GREEN + f"✅ Latest Project tree written to {OUTPUT_MD}" + Style.RESET_ALL)
    print(Fore.GREEN + f"✅ Snapshot saved: {SNAPSHOT_MD.name}, {SNAPSHOT_JSON.name}" + Style.RESET_ALL)


def _cleanup_old_snapshots(keep=2):
    """
    Keep only the last `keep` snapshots (MD + JSON) in SNAPSHOT_DIR.
    Older files are deleted.
    """
    all_files = sorted(SNAPSHOT_DIR.glob("Project_Tree_*.md"), key=lambda f: f.stat().st_mtime, reverse=True)
    old_files = all_files[keep:]  # keep only the most recent N
    for f in old_files:
        json_file = f.with_suffix(".json")
        if f.exists():
            f.unlink()
        if json_file.exists():
            json_file.unlink()
        print(Fore.RED + f"❌ Deleted old snapshot: {f.name} (+ JSON)" + Style.RESET_ALL)


def _build_summary(stats: dict) -> list[str]:
    """
    Build summary section of the report based on file statistics.
    """
    CATEGORIES = {
        "📝 Documentation": ["ADR Doc", "Policies Doc", "Template Doc", "Release Doc", "Testing Doc", "Markdown Doc"],
        "⚙️ Build & Config": ["Build Config", "CI Config"],
        "💻 Code (C++)": ["App Entry Point", "C++ Source", "C++ Header", "C++ Test", "C++ Benchmark"],
        "🎨 UI (QML)": ["Root QML UI", "QML UI"],
        "🔧 Scripts": ["Python Script", "Other"],
        "📐 Diagrams": ["UML Diagram"],
    }

    summary_lines = [
        "",
        "## 📊 Summary",
        "",
        f"- Directories: {stats['dirs']}",
        f"- Files: {stats['files']}",
    ]

    for cat, types in CATEGORIES.items():
        total = sum(stats["types"].get(t, 0) for t in types)
        summary_lines.append(f"\n### {cat}")
        for t in types:
            if t in stats["types"]:
                summary_lines.append(f"- {t}: {stats['types'][t]}")
        if total > 0:
            summary_lines.append(f"**Total {cat}: {total}**")

    return summary_lines
