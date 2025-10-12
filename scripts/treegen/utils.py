import hashlib
from pathlib import Path
from colorama import Fore, Style
from .tags import generate_tags
import pathspec  # ← جديد

# --- Configuration ---
HARDCODED_EXCLUDES = {".git", "build", "__pycache__", ".idea", ".vs"}
MAX_FILE_SIZE = 5 * 1024 * 1024  # 5MB

# Placeholder for gitignore spec (initialized later)
gitignore_spec = None

# Project root (اضبطه على حسب مكانك)
ROOT = Path(__file__).resolve().parent.parent.parent


# --- Gitignore Loader ---
def load_gitignore():
    """
    Load and parse the .gitignore file into a PathSpec object.
    Called once at program startup.
    """
    global gitignore_spec
    gitignore_path = ROOT / ".gitignore"
    if gitignore_path.exists():
        with open(gitignore_path, "r") as f:
            gitignore_spec = pathspec.PathSpec.from_lines("gitwildmatch", f)
    else:
        gitignore_spec = pathspec.PathSpec.from_lines("gitwildmatch", [])


def is_excluded(path: Path) -> bool:
    """
    Check if a file or directory should be excluded
    based on hardcoded rules or .gitignore.
    """
    # Hardcoded excludes
    if path.name in HARDCODED_EXCLUDES:
        return True

    # Apply .gitignore rules (if loaded)
    if gitignore_spec and gitignore_spec.match_file(str(path.relative_to(ROOT))):
        return True

    return False


# --- Hashing ---
def compute_hash(path: Path) -> str:
    """
    Compute SHA1 hash for a file. Return 'ERROR' if file cannot be read.
    """
    h = hashlib.sha1()
    try:
        with path.open("rb") as f:
            while chunk := f.read(8192):
                h.update(chunk)
    except (FileNotFoundError, PermissionError):
        return "ERROR"
    return h.hexdigest()


# --- File Processing ---
def process_file(entry, rel_path, ftype, phase, slice_name, run_ts, old_data):
    """
    Process a single file:
    - Skip if too large
    - Compute hash and detect status (new/changed/same)
    - Generate tags
    - Count lines of code (LOC)
    - Return structured metadata record + console label
    """

    file_size, mtime = entry.stat().st_size, entry.stat().st_mtime
    if file_size > MAX_FILE_SIZE:
        return None, None, f"{entry.name} [SKIPPED: >5MB]"

    # ابحث عن نسخة قديمة من نفس الملف في snapshot السابق
    old_entry = next((e for e in old_data if e["file"] == rel_path), None) if old_data else None

    # --- Detect status and hash ---
    if old_entry and old_entry.get("size") == file_size and old_entry.get("mtime") == mtime:
        file_hash = old_entry["hash"]
        status = "same"
    else:
        file_hash = compute_hash(entry)
        if old_entry is None:
            status = "new"
        elif old_entry["hash"] != file_hash:
            status = "changed"
        else:
            status = "same"

    # --- Preserve old metadata when appropriate ---
    file_ts = old_entry["timestamp"] if old_entry else run_ts
    effective_phase = old_entry.get("phase", phase) if old_entry else phase
    effective_slice = old_entry.get("slice", slice_name) if old_entry else slice_name

    # --- Generate tags (بناءً على المرحلة الحالية أو القديمة حسب الملف) ---
    tags = generate_tags(rel_path, ftype, effective_phase, effective_slice)

    # --- Count lines (LOC) ---
    loc = None
    try:
        if ftype not in ("Other", "Binary") and file_size < MAX_FILE_SIZE:
            with entry.open("r", encoding="utf-8", errors="ignore") as f:
                loc = sum(1 for _ in f)
    except Exception:
        loc = None

    # --- Build labels ---
    label = f"{entry.name} [{ftype}] {' '.join(f'[{t}]' for t in tags)}" if tags else f"{entry.name} [{ftype}]"

    if status == "new":
        console_label = Fore.GREEN + label + Style.RESET_ALL
    elif status == "changed":
        console_label = Fore.YELLOW + label + Style.RESET_ALL
    else:
        console_label = Fore.LIGHTBLACK_EX + label + Style.RESET_ALL

    # --- Final record ---
    record = {
        "file": rel_path,
        "type": ftype,
        "tags": tags,
        "phase": effective_phase,
        "slice": effective_slice,
        "status": status,
        "timestamp": file_ts,
        "size": file_size,
        "mtime": mtime,
        "hash": file_hash,
    }

    if loc is not None:
        record["loc"] = loc

    return record, console_label, label
