# ⚙️ Setup & Usage Guide for TreeGen

TreeGen is a utility script that generates a raw project tree with metadata, tags, and snapshots.

---

## 📦 Requirements

Install the required Python packages:

```bash
pip install -r requirements.txt
```

Or manually:

```bash
pip install colorama pathspec
```

---

## 📄 requirements.txt

```
colorama
pathspec
```

Save this file as `requirements.txt` in your project root (next to `scripts/`).

---

## 🚀 Running the Script

From the **project root** (`Tasqly_V1`), run:

```bash
py -3.13 -m scripts.treegen.gen_tree --phase Phase0 --slice Slice0
```

### Optional Arguments

- `--phase PHASENAME`  
  Define the current phase (default: `Phase0`).

- `--slice SLICENAME`  
  Define the current slice (optional).

Example:

```bash
py -3.13 -m scripts.treegen.gen_tree --phase Phase1 --slice Slice2
```

---

## 📂 Outputs

TreeGen generates reports under:

```
reports/project_tree/
    ├── Project_Tree.md       # Latest Markdown snapshot
    ├── Project_Tree.json     # Latest JSON snapshot
    └── snapshots/            # Timestamped snapshots
```

- **Markdown**: Human-readable tree view + summary.  
- **JSON**: Machine-readable metadata (size, hash, mtime, tags, etc).  
- **Snapshots**: Historical records with timestamps.

---

## 🎨 Console Output

When you run the script, the console will show:
- ✅ New files (green)
- ⚠️ Changed files (yellow)
- ❌ Deleted files (red)

---

## 🔧 Notes
- Files larger than **5MB** are skipped automatically.  
- Supports `.gitignore` exclusions.  
- Colored output requires **colorama**.  
- `.gitignore` parsing requires **pathspec**.
