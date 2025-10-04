#!/usr/bin/env python3
# ------------------------------------------------------------
# 🧱 File: uml-export.py
# ------------------------------------------------------------
# 📌 Purpose   : Export all PlantUML diagrams (*.puml) into SVG format
#                under their corresponding exports/ folders.
# 🧱 Layer     : Docs (UML Infrastructure)
# 👤 Author    : Mohamed Ali
# 🗓️ Created   : 2025-09-07
#
# 🧠 Description:
# Traverses all UML slice folders (docs/uml/phaseX/sY), finds *.puml
# diagrams, and invokes plantuml.jar to generate .svg files into
# the local exports/ directory. Designed to be reusable in local dev
# workflow and in CI (uml.yml + uml-pages.yml).
#
# ✅ CI-Safe Enhancements:
#  - Robust path resolution (no fragile ../../)
#  - Validate JAR (detect LFS pointer / corrupt files) and auto-download if missing
#  - Java presence check with clear error
#  - Auto workers = min(#.puml files, CPU cores - 1)
#
# Example usage:
#   python docs/uml/uml-export.py
# ------------------------------------------------------------
#
# ------------------------------------------------------------
# 📌 TODO (Future Optimizations - Phase 1+)
# ------------------------------------------------------------
# 1️⃣ Parallel Export:
#    Run multiple PlantUML processes in parallel (xargs -P or GNU parallel)
#    → Faster export for hundreds of diagrams.
#
# 2️⃣ Hash-Based Caching:
#    Compute MD5/SHA1 for each .puml and store in cache.json.
#    Rebuild only if file content changes, not just timestamp.
#
# 3️⃣ Dependency Graph:
#    Parse !include directives to detect indirect dependencies.
#    If style.puml or any included file changes → rebuild affected diagrams only.
#
# Note: Current Phase 0 solution (timestamp + style.puml awareness) is sufficient.
# ------------------------------------------------------------
import os
import shutil
import subprocess
import requests
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed

BASE_DIR = Path(__file__).resolve().parent
TOOLS_DIR = BASE_DIR / "tools"
PLANTUML_JAR = TOOLS_DIR / "plantuml.jar"

PLANTUML_VERSION = os.environ.get("PLANTUML_VERSION", "1.2024.7")
PLANTUML_URL = f"https://github.com/plantuml/plantuml/releases/download/v{PLANTUML_VERSION}/plantuml-{PLANTUML_VERSION}.jar"

# 📝 Global counters
stats = {"exported": 0, "skipped": 0, "errors": 0}

def log(msg: str):
    print(msg, flush=True, end="\n")

# ---------------------------
# ☕ Java & JAR guards
# ---------------------------
def ensure_java():
    if shutil.which("java") is None:
        log("❌ Java is not installed on PATH")
        exit(1)

def ensure_jar():
    TOOLS_DIR.mkdir(parents=True, exist_ok=True)
    if not PLANTUML_JAR.exists() or PLANTUML_JAR.stat().st_size == 0:
        log(f"⬇️  Downloading PlantUML {PLANTUML_VERSION} → {PLANTUML_JAR}")
        r = requests.get(PLANTUML_URL)
        r.raise_for_status()
        with open(PLANTUML_JAR, "wb") as f:
            f.write(r.content)

def validate_jar():
    try:
        subprocess.run(["java", "-jar", str(PLANTUML_JAR), "-version"],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
    except subprocess.CalledProcessError:
        log("⚠️  plantuml.jar appears invalid. Re-downloading…")
        PLANTUML_JAR.unlink(missing_ok=True)
        ensure_jar()
        try:
            subprocess.run(["java", "-jar", str(PLANTUML_JAR), "-version"],
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
        except subprocess.CalledProcessError:
            log("❌ PlantUML jar is invalid after re-download")
            exit(2)

# ---------------------------
# 🖼️ Export routine
# ---------------------------
def export_file(puml_file: Path, svg_file: Path, dir_path: Path):
    log(f"🖼️  Exporting {puml_file.name} → {svg_file}")
    subprocess.run(
        ["java", "-jar", str(PLANTUML_JAR), "-tsvg", str(puml_file), "-o", "exports"],
        cwd=dir_path,
        check=True
    )
    return puml_file.name

def export_slice_dir(dir_path: Path):
    if not dir_path.is_dir():
        return
    export_dir = dir_path / "exports"
    export_dir.mkdir(exist_ok=True)

    puml_files = list(dir_path.glob("*.puml"))
    if not puml_files:
        return

    # 🧠 Auto workers: min(#files, CPU cores - 1)
    cpu_max = max(1, (os.cpu_count() or 2) - 1)
    workers = min(len(puml_files), cpu_max)

    log(f"⚙️ Using {workers} workers for {len(puml_files)} file(s) in {dir_path.name}")

    tasks = []
    with ThreadPoolExecutor(max_workers=workers) as executor:
        for puml_file in puml_files:
            svg_file = export_dir / f"{puml_file.stem}.svg"
            if not svg_file.exists() or puml_file.stat().st_mtime > svg_file.stat().st_mtime:
                tasks.append(executor.submit(export_file, puml_file, svg_file, dir_path))
            else:
                log(f"⏩ Skipping {puml_file.name} (up to date)")
                stats["skipped"] += 1

        for future in as_completed(tasks):
            try:
                _ = future.result()
                stats["exported"] += 1
                log(f"✅ Finished {_}")
            except Exception as e:
                stats["errors"] += 1
                log(f"❌ Error: {e}")

# ---------------------------
# 🚀 Run
# ---------------------------
def main():
    log(f"🚀 Starting UML export using {PLANTUML_JAR}")
    ensure_java()
    ensure_jar()
    validate_jar()

    # Traverse all phase folders and export diagrams
    for phase_dir in BASE_DIR.glob("phase*/*"):
        if phase_dir.is_dir():
            export_slice_dir(phase_dir)

    log("✅ UML export completed successfully.")

    # ------------------------------------------------------------
    # 🌐 Generate index.html for GitHub Pages browsing
    # ------------------------------------------------------------
    index_file = BASE_DIR / "exports" / "index.html"
    index_file.parent.mkdir(parents=True, exist_ok=True)

    log(f"📝 Generating index.html at {index_file}")

    svg_files = sorted(BASE_DIR.rglob("*.svg"))

    with open(index_file, "w", encoding="utf-8") as f:
        f.write("""<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Tasqly UML Diagrams</title>
  <style>
    body { background-color: #0D1117; color: #E6EDF3; font-family: "Segoe UI", Arial, sans-serif; margin: 40px; }
    h1 { color: #58A6FF; margin-bottom: 20px; }
    ul { list-style-type: none; padding: 0; }
    li { margin: 8px 0; }
    a { color: #A371F7; text-decoration: none; font-weight: 600; }
    a:hover { color: #3FB950; }
    footer { margin-top: 40px; font-size: 12px; color: #8B949E; }
  </style>
</head>
<body>
  <h1>Tasqly UML Diagrams</h1>
  <ul>
""")
        for svg in svg_files:
            rel = os.path.relpath(svg, start=index_file.parent)
            f.write(f"    <li><a href='{rel}' target='_blank'>{svg.name}</a></li>\n")
        f.write("""  </ul>
  <footer>Generated automatically with PlantUML • Corporate Dark Theme</footer>
</body>
</html>""")

    log("✅ index.html generated.")

    # ------------------------------------------------------------
    # 📊 Summary
    # ------------------------------------------------------------
    total = stats["exported"] + stats["skipped"]
    log("\n📊 Summary")
    log(f"   🖼️ Exported : {stats['exported']}")
    log(f"   ⏩ Skipped  : {stats['skipped']}")
    log(f"   ❌ Errors   : {stats['errors']}")
    log(f"   📂 Total    : {total}")

if __name__ == "__main__":
    main()
