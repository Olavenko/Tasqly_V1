#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

# 📝 Collect all C++/header files
mapfile -t FILES < <(find src tests benchmarks . -maxdepth 1 \( -name '*.cpp' -o -name '*.h' \))

# 🔎 Auto-detect clang-format binary (try multiple names)
for candidate in clang-format-20 clang-format; do
  if command -v "$candidate" &> /dev/null; then
    CLANGFMT=$(command -v "$candidate")
    break
  fi
done

if [[ -z "${CLANGFMT:-}" ]]; then
  echo "❌ No suitable clang-format found (tried: clang-format-20, clang-format)"
  exit 1
fi

echo "Using clang-format binary: $CLANGFMT"
"$CLANGFMT" --version

# ✅ Apply formatting
for f in "${FILES[@]}"; do
  "$CLANGFMT" -i "$f"
done
echo "✨   All .cpp/.h files formatted."

# ✅ Verify formatting
"$CLANGFMT" --dry-run --Werror "${FILES[@]}"
