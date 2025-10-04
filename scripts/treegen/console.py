from colorama import Fore, Style

def print_status(json_store, old_data):
    """
    Print a colored and aligned summary of project tree changes.
    """

    def get_key(record):
        return record.get("file") or record.get("path")

    # Build maps for quick lookup
    old_map = {get_key(r): r for r in old_data} if old_data else {}
    new_map = {get_key(r): r for r in json_store}

    old_files = set(old_map.keys())
    new_files = set(new_map.keys())

    added = new_files - old_files
    deleted = old_files - new_files

    modified = set()
    for f in (new_files & old_files):
        old_r, new_r = old_map[f], new_map[f]
        if old_r.get("ftype") != new_r.get("ftype") or old_r.get("tags") != new_r.get("tags"):
            modified.add(f)

    # Alignment setup
    label_width = 16
    max_count = max(len(str(len(added))), len(str(len(modified))), len(str(len(deleted))))
    count_width = max(2, max_count)

    print(Fore.CYAN + "\n📊 Project Tree Update Summary:" + Style.RESET_ALL)
    print(Fore.GREEN  + f"  ✅  {'New files'.ljust(label_width)}: {str(len(added)).rjust(count_width)}" + Style.RESET_ALL)
    print(Fore.YELLOW + f"  ✏️  {'Modified files'.ljust(label_width)}: {str(len(modified)).rjust(count_width)}" + Style.RESET_ALL)
    print(Fore.RED    + f"  ❌  {'Deleted files'.ljust(label_width)}: {str(len(deleted)).rjust(count_width)}\n" + Style.RESET_ALL)
