#!/usr/bin/env python3
import json
from pathlib import Path
from .cli import parse_args
from .walker import walk_tree
from .console import print_status
from .exporter import export_outputs
from .utils import load_gitignore


def main():
    args = parse_args()

    run_ts = args.run_ts
    run_stamp = args.run_stamp

    ROOT = Path(__file__).resolve().parent.parent.parent
    SNAPSHOT_DIR = ROOT / "reports" / "project_tree" / "snapshots"
    SNAPSHOT_DIR.mkdir(parents=True, exist_ok=True)

    load_gitignore()

    old_data = []
    latest_snapshot = None

    snapshots = sorted(
        SNAPSHOT_DIR.glob("Project_Tree_*.json"),
        key=lambda f: f.stat().st_mtime,
        reverse=True,
    )
    if snapshots:
        latest_snapshot = snapshots[0]
        try:
            old_data = json.loads(latest_snapshot.read_text(encoding="utf-8"))
            print(f"📂 Comparing against last snapshot: {latest_snapshot.name}")
        except Exception:
            print("⚠️ Warning: Could not read last snapshot, starting fresh.")

    stats = {"files": 0, "dirs": 0, "types": {}}

    header = [
        "# 📂 Project Tree (Raw Snapshot)",
        "",
        f"Generated on **{run_ts}** using `scripts/treegen/gen_tree.py`.",
        "",
        "## Root Layout",
        ROOT.name + "/",
    ]

    json_store, body = walk_tree(ROOT, args.phase, args.slice, run_ts, old_data, stats)

    print_status(json_store, old_data, args.phase)

    export_outputs(
        run_ts, run_stamp, header, body, stats,
        json_store, args.phase, args.slice, latest_snapshot
    )


if __name__ == "__main__":
    main()
