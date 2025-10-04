import os
import re

# Root directory of the project
PROJECT_ROOT = r"C:\Users\Olavenko\Desktop\TasqlyQt\Tasqly_V1"

# === Regex Fix Functions ===
def fix_src_includes(content):
    return re.sub(r'#include\s+"src/', '#include "', content)

def fix_relative_app_includes(content):
    return re.sub(r'#include\s+"(?:\.\./)*app/', '#include "app/', content)

def fix_both(content):
    content = fix_src_includes(content)
    content = fix_relative_app_includes(content)
    return content

# === File Processor ===
def process_files(fix_func, description):
    changed_files = 0
    total_files = 0
    total_replacements = 0
    changed_list = []

    for root, _, files in os.walk(PROJECT_ROOT):
        for file in files:
            if file.endswith((".h", ".cpp")):
                filepath = os.path.join(root, file)
                total_files += 1

                with open(filepath, "r", encoding="utf-8") as f:
                    content = f.read()

                new_content = fix_func(content)

                if new_content != content:
                    changed_files += 1
                    # Count replacements roughly by diff in lines
                    diff = sum(
                        1 for (old, new) in zip(content.splitlines(), new_content.splitlines())
                        if old != new
                    )
                    total_replacements += diff
                    changed_list.append((filepath, diff))

                    with open(filepath, "w", encoding="utf-8") as f:
                        f.write(new_content)

    # Console Output with nice icons
    print(f"\n=== {description} ===")
    print(f"📂 Scanned files     : {total_files}")
    print(f"📝 Modified files    : {changed_files}")
    print(f"🔄 Total replacements: {total_replacements}\n")

    if changed_list:
        print("📑 Files changed:")
        for filepath, diff in changed_list:
            print(f"   ✔ {filepath}  (+{diff} changes)")
    else:
        print("✅ No files needed modification.\n")

# === Main Menu ===
if __name__ == "__main__":
    print("\n=== Include Fixer Script ===")
    print("1️⃣  Remove 'src/' from includes")
    print("2️⃣  Normalize '../../app/...' to 'app/...'")
    print("3️⃣  Apply both fixes together")
    choice = input("Select an option (1, 2, or 3): ").strip()

    if choice == "1":
        process_files(fix_src_includes, "Fix: Remove src/")
    elif choice == "2":
        process_files(fix_relative_app_includes, "Fix: Normalize relative app/")
    elif choice == "3":
        process_files(fix_both, "Fix: Apply both")
    else:
        print("❌ Invalid choice. Exiting.")
