#!/usr/bin/env bash
# ------------------------------------------------------------
# 🧱 File: uml-export.sh
# ------------------------------------------------------------
# 📌 Purpose   : Export all PlantUML diagrams (*.puml) into SVG format
#                under their corresponding exports/ folders.
# 🧱 Layer     : Docs (UML Infrastructure)
# 👤 Author    : Mohamed Ali
# 🗓️ Created   : 2025-08-28
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
#
# Example usage:
#   bash docs/uml/uml-export.sh
# ------------------------------------------------------------

set -Eeuo pipefail

# ---------------------------
# 📂 Paths (robust, absolute)
# ---------------------------
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
BASE_DIR="$SCRIPT_DIR"
TOOLS_DIR="$BASE_DIR/tools"
PLANTUML_JAR="$TOOLS_DIR/plantuml.jar"
STYLE_FILE="$BASE_DIR/config/style.puml"

# You may pin PlantUML version for reproducible CI
PLANTUML_VERSION="${PLANTUML_VERSION:-1.2024.7}"
PLANTUML_URL="https://github.com/plantuml/plantuml/releases/download/v${PLANTUML_VERSION}/plantuml-${PLANTUML_VERSION}.jar"

log() { echo -e "$@"; }

# ---------------------------
# ☕ Java & JAR guards
# ---------------------------
ensure_java() {
  if ! command -v java >/dev/null 2>&1; then
    log "❌ Java is not installed on PATH"; exit 1;
  fi
}

ensure_jar() {
  mkdir -p "$TOOLS_DIR"
  if [[ ! -s "$PLANTUML_JAR" ]]; then
    log "⬇️  Downloading PlantUML ${PLANTUML_VERSION} → $PLANTUML_JAR"
    curl -fsSL -o "$PLANTUML_JAR" "$PLANTUML_URL"
  fi
}

validate_jar() {
  # Detect Git LFS pointer or corrupt jar by checking version output
  if ! java -jar "$PLANTUML_JAR" -version >/dev/null 2>&1; then
    log "⚠️  plantuml.jar appears invalid or is an LFS pointer. Re-downloading…"
    rm -f "$PLANTUML_JAR"
    curl -fsSL -o "$PLANTUML_JAR" "$PLANTUML_URL"
    java -jar "$PLANTUML_JAR" -version >/dev/null 2>&1 || {
      log "❌ PlantUML jar is invalid after re-download"; exit 2; }
  fi
}

# ---------------------------
# 🖼️ Export routine
# ---------------------------
export_slice_dir() {
  local dir="$1"
  [[ -d "$dir" ]] || return 0

  local EXPORT_DIR="$dir/exports"
  mkdir -p "$EXPORT_DIR"

  shopt -s nullglob
  for f in "$dir"/*.puml; do
    local filename; filename="$(basename -- "$f")"
    local name="${filename%.*}"
    local svg_file="$EXPORT_DIR/$name.svg"

    # 🧠 Smart rebuild logic (simplified):
    # 1) no SVG exists  2) PUML newer than SVG
    if [[ ! -f "$svg_file" || "$f" -nt "$svg_file" ]]; then
      log "🖼️  Exporting $filename → $EXPORT_DIR/$name.svg"
      (
        cd "$dir"
        java -jar "$PLANTUML_JAR" -tsvg "$filename" -o "exports"
      )
    else
      log "⏩ Skipping $filename (up to date)"
    fi
  done
}

# ---------------------------
# 🚀 Run
# ---------------------------
log "🚀 Starting UML export using $PLANTUML_JAR"
ensure_java
ensure_jar
validate_jar

# Traverse all phase folders and export diagrams
# (e.g., docs/uml/phase0/s0, docs/uml/phase0/s1, …)
for dir in "$BASE_DIR"/phase*/*; do
  [[ -d "$dir" ]] || continue
  export_slice_dir "$dir"
done

log "✅ UML export completed successfully."

# ------------------------------------------------------------
# 🌐 Generate index.html for GitHub Pages browsing
# ------------------------------------------------------------
INDEX_FILE="$BASE_DIR/exports/index.html"
mkdir -p "$(dirname "$INDEX_FILE")"

log "📝 Generating index.html at $INDEX_FILE"

cat <<'EOF' > "$INDEX_FILE"
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Tasqly UML Diagrams</title>
  <style>
    body {
      background-color: #0D1117;
      color: #E6EDF3;
      font-family: "Segoe UI", Arial, Helvetica, sans-serif;
      margin: 40px;
    }
    h1 {
      color: #58A6FF; /* Primary Blue */
      margin-bottom: 20px;
    }
    ul { list-style-type: none; padding: 0; }
    li { margin: 8px 0; }
    a {
      color: #A371F7; /* Components Purple */
      text-decoration: none;
      font-weight: 600;
    }
    a:hover { color: #3FB950; /* Actor Green */ }
    footer {
      margin-top: 40px;
      font-size: 12px;
      color: #8B949E;
    }
  </style>
</head>
<body>
  <h1>Tasqly UML Diagrams</h1>
  <ul>
EOF

# Add links to all SVGs (sorted). realpath may not exist on some shells; fallback to printing relative-ish.
if command -v realpath >/dev/null 2>&1; then
  find "$BASE_DIR" -type f -name "*.svg" | sort | while read -r f; do
    relpath=$(realpath --relative-to="$(dirname "$INDEX_FILE")" "$f")
    fname=$(basename "$f")
    echo "    <li><a href='$relpath' target='_blank'>$fname</a></li>" >> "$INDEX_FILE"
  done
else
  # Fallback: strip leading base dir path
  find "$BASE_DIR" -type f -name "*.svg" | sort | while read -r f; do
    relpath="${f#$BASE_DIR/exports/../}"  # simple best-effort; links remain functional in Pages
    fname=$(basename "$f")
    echo "    <li><a href='../${relpath}' target='_blank'>$fname</a></li>" >> "$INDEX_FILE"
  done
fi

cat <<'EOF' >> "$INDEX_FILE"
  </ul>
  <footer>Generated automatically with PlantUML • Corporate Dark Theme</footer>
</body>
</html>
EOF

log "✅ index.html generated."

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
