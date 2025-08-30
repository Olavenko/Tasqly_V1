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
# Example usage:
#   bash docs/uml/uml-export.sh
# ------------------------------------------------------------

set -e

PLANTUML_JAR="docs/uml/tools/plantuml.jar"
BASE_DIR="docs/uml"
STYLE_FILE="docs/uml/config/style.puml"

if [ ! -f "$PLANTUML_JAR" ]; then
  echo "❌ plantuml.jar not found at $PLANTUML_JAR"
  exit 1
fi

echo "🚀 Starting UML export using $PLANTUML_JAR"

# Traverse all phase folders and export diagrams
for dir in "$BASE_DIR"/phase*/*; do
  if [ -d "$dir" ]; then
    EXPORT_DIR="$dir/exports"
    mkdir -p "$EXPORT_DIR"
    for f in "$dir"/*.puml; do
      [ -e "$f" ] || continue

      filename=$(basename -- "$f")
      name="${filename%.*}"
      svg_file="$EXPORT_DIR/$name.svg"

      # 🧠 Smart rebuild logic:
      # 1. Build if no SVG exists
      # 2. Build if PUML newer than SVG
      # 3. Build if style.puml newer than SVG
      if [ ! -f "$svg_file" ] || [ "$f" -nt "$svg_file" ] || [ "$STYLE_FILE" -nt "$svg_file" ]; then
        echo "🖼️  Exporting $filename → $EXPORT_DIR/$name.svg"
        (
          cd "$dir"
          java -jar "../../tools/plantuml.jar" -tsvg "$filename" -o "exports"
        )
      else
        echo "⏩ Skipping $filename (up to date)"
      fi
    done
  fi
done

echo "✅ UML export completed successfully."

# ------------------------------------------------------------
# 🌐 Generate index.html for GitHub Pages browsing
# ------------------------------------------------------------
INDEX_FILE="$BASE_DIR/exports/index.html"
mkdir -p "$(dirname "$INDEX_FILE")"

echo "📝 Generating index.html at $INDEX_FILE"

cat <<'EOF' > "$INDEX_FILE"
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Tasqly UML Diagrams</title>
  <style>
    body {
      background-color: #000000;
      color: #FFFFFF;
      font-family: "Segoe UI", sans-serif;
      margin: 40px;
    }
    h1 {
      color: #4BE3FE; /* Activity Blue */
      margin-bottom: 20px;
    }
    ul {
      list-style-type: none;
      padding: 0;
    }
    li {
      margin: 8px 0;
    }
    a {
      color: #FF6F5C; /* Components Orange */
      text-decoration: none;
      font-weight: bold;
    }
    a:hover {
      color: #00CE6A; /* Actor Green */
    }
    footer {
      margin-top: 40px;
      font-size: 12px;
      color: #888;
    }
  </style>
</head>
<body>
  <h1>Tasqly UML Diagrams</h1>
  <ul>
EOF

# Add links to all SVGs
find "$BASE_DIR" -type f -name "*.svg" | sort | while read -r f; do
  relpath=$(realpath --relative-to="$(dirname "$INDEX_FILE")" "$f")
  fname=$(basename "$f")
  echo "    <li><a href='$relpath' target='_blank'>$fname</a></li>" >> "$INDEX_FILE"
done

cat <<'EOF' >> "$INDEX_FILE"
  </ul>
  <footer>Generated automatically with PlantUML • Corporate Dark Theme</footer>
</body>
</html>
EOF

echo "✅ index.html generated."

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