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
    content = "\n".join(header + body + _build_summary(stats, phase, slice_name, json_store)) + "\n"

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
    """Keep only the last `keep` snapshots (MD + JSON) in SNAPSHOT_DIR."""
    all_files = sorted(SNAPSHOT_DIR.glob("Project_Tree_*.md"), key=lambda f: f.stat().st_mtime, reverse=True)
    old_files = all_files[keep:]
    for f in old_files:
        json_file = f.with_suffix(".json")
        if f.exists():
            f.unlink()
        if json_file.exists():
            json_file.unlink()
        print(Fore.RED + f"❌ Deleted old snapshot: {f.name} (+ JSON)" + Style.RESET_ALL)


def _build_summary(stats: dict, current_phase: str, current_slice: str, json_store: list) -> list[str]:
    """
    Build summary section of the report based on file statistics.
    Includes:
    - Category overview (filtered by current phase)
    - Historical Phase summary (timeline) with formatted date/time and status
    """

    from datetime import datetime

    # 🔹 Filter records for current phase only
    current_phase_records = [r for r in json_store if r["phase"] == current_phase]

    CATEGORIES = {
        "📝 Documentation": ["ADR Doc", "Policies Doc", "Template Doc", "Release Doc", "Testing Doc", "Markdown Doc"],
        "⚙️ Build & Config": ["Build Config", "CI Config"],
        "💻 Code (C++)": ["App Entry Point", "C++ Source", "C++ Header", "C++ Test", "C++ Benchmark"],
        "🎨 UI (QML)": ["Root QML UI", "QML UI"],
        "🔧 Scripts": ["Python Script", "Other"],
        "📐 Diagrams": ["UML Diagram"],
    }

    # --- Base summary section ---
    summary_lines = [
        "",
        f"## 📊 Summary of {current_phase}",
        "",
        f"- Directories: {stats['dirs']}",
        f"- Files: {len(current_phase_records)}",
    ]

    # === Overall Category Summary (filtered by current phase) ===
    type_counts = {}
    for rec in current_phase_records:
        t = rec.get("type")
        if t:
            type_counts[t] = type_counts.get(t, 0) + 1

    for cat, types in CATEGORIES.items():
        total = sum(type_counts.get(t, 0) for t in types)
        if total > 0:
            summary_lines.append(f"\n### {cat}")
            for t in types:
                if t in type_counts:
                    summary_lines.append(f"- {t}: {type_counts[t]}")
            summary_lines.append(f"**Total {cat}: {total}**")

    # --- Historical Phase Summary ---
    summary_lines.append("\n## 🧭 Phase History Summary\n")

    def fmt_time(ts: str | None) -> tuple[str, str]:
        """Convert timestamp string to ('YYYY-MM-DD', 'HH:MM AM/PM') supporting both HH:MM and HH:MM:SS formats."""
        if not ts:
            return ("N/A", "N/A")
        from datetime import datetime
        for fmt in ("%Y-%m-%d %H:%M:%S", "%Y-%m-%d %H:%M"):
            try:
                dt = datetime.strptime(ts, fmt)
                date_part = dt.strftime("%Y-%m-%d")
                time_part = dt.strftime("%I:%M %p")
                return (date_part, time_part)
            except ValueError:
                continue
        return (ts, "N/A")


    # Build phase timelines
    phase_timeline = {}
    for record in json_store:
        ph = record["phase"]
        ts = record.get("timestamp")
        phase_timeline.setdefault(ph, {"start": ts, "end": ts, "files": 0, "new": 0, "changed": 0})
        phase_timeline[ph]["files"] += 1
        if record["status"] == "new":
            phase_timeline[ph]["new"] += 1
        if record["status"] == "changed":
            phase_timeline[ph]["changed"] += 1
        if ts:
            if phase_timeline[ph]["start"] is None or ts < phase_timeline[ph]["start"]:
                phase_timeline[ph]["start"] = ts
            if phase_timeline[ph]["end"] is None or ts > phase_timeline[ph]["end"]:
                phase_timeline[ph]["end"] = ts

    # Generate formatted summary lines
    for ph, data in sorted(phase_timeline.items()):
        files = data["files"]
        start_date, start_time = fmt_time(data["start"])
        end_date, end_time = fmt_time(data["end"])
        star = "⭐" if ph == current_phase else ""

        if ph == current_phase:
            # 🔧 In-progress phase
            summary_lines.append(
                f"- **{ph}** {star} → Files: {files} | "
                f"📅 Started Date: {start_date} - 🕒 Time: {start_time} "
                f"| ⏳ Status : 🛠️ In Progress"
            )
        else:
            # ✅ Completed phase
            summary_lines.append(
                f"- **{ph}**  → Files: {files} | "
                f"📅 From Date: {start_date} - 🕒 Time: {start_time} → "
                f"📅 To Date: {end_date} - 🕒 Time: {end_time} | ⏳ Status : ✅ Done"
            )

    return summary_lines


