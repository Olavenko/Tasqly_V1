from colorama import Fore, Style

def print_status(json_store, old_data, phase_name):
    """
    Print a colored and aligned summary of project tree changes.
    Includes current phase in the summary line.
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
        if old_r.get("hash") != new_r.get("hash") or new_r.get("status") == "changed":
            modified.add(f)

    same_count = sum(1 for f in json_store if f.get("status") == "same")
    new_count = sum(1 for f in json_store if f.get("status") == "new")
    changed_count = sum(1 for f in json_store if f.get("status") == "changed")

    label_width = 18
    count_width = max(len(str(new_count)), len(str(changed_count)), len(str(len(deleted))), 2)

    print(Fore.CYAN + "\n📊 Project Tree Update Summary:" + Style.RESET_ALL)
    print(Fore.GREEN  + f"  🟢  {'New files'.ljust(label_width)}: {str(new_count).rjust(count_width)}" + Style.RESET_ALL)
    print(Fore.YELLOW + f"  🟠  {'Modified files'.ljust(label_width)}: {str(changed_count).rjust(count_width)}" + Style.RESET_ALL)
    print(Fore.RED    + f"  🔴  {'Deleted files'.ljust(label_width)}: {str(len(deleted)).rjust(count_width)}" + Style.RESET_ALL)
    print(Fore.LIGHTBLACK_EX + f"  ⚪  {'Unchanged files'.ljust(label_width)}: {str(same_count).rjust(count_width)}\n" + Style.RESET_ALL)

    # Highlight the current phase
    if new_count > 0:
        print(Fore.GREEN + f"     → New in {phase_name}: {new_count} files" + Style.RESET_ALL)
    if changed_count > 0:
        print(Fore.YELLOW + f"     → Modified since last snapshot: {changed_count} files" + Style.RESET_ALL)
    if len(deleted) > 0:
        print(Fore.RED + f"     → Deleted since last snapshot: {len(deleted)} files" + Style.RESET_ALL)

    # === NEW: Phase comparison summary ===
    try:
        # Identify current and previous phases
        current_files = [r for r in json_store if r.get("phase") == phase_name]
        prev_phases = sorted({r.get("phase") for r in json_store if r.get("phase") != phase_name})
        previous_phase = prev_phases[-1] if prev_phases else None
        prev_files = [r for r in json_store if r.get("phase") == previous_phase] if previous_phase else []

        print(Fore.LIGHTCYAN_EX + f"\n📘 Current Phase Summary: {phase_name} ({len(current_files)} files)" + Style.RESET_ALL)
        if previous_phase:
            print(Fore.LIGHTBLACK_EX + f"📗 Previous Phase: {previous_phase} ({len(prev_files)} files)" + Style.RESET_ALL)

    except Exception as e:
        print(Fore.RED + f"\n⚠️ Phase summary display failed: {e}" + Style.RESET_ALL)
