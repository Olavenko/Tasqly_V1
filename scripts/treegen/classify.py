def classify_file_with_context(rel_path: str, name: str) -> str:
    """
    Classify file type based on extension, name, and relative path context.
    """

    lower = name.lower()

    # --- C++ Files ---
    if lower.endswith(".cpp"):
        if lower.startswith(("bench_", "bench")):
            return "C++ Benchmark"
        if lower.startswith(("test_", "tests_")):
            return "C++ Test"
        if lower == "main.cpp":
            return "App Entry Point"
        return "C++ Source"

    if lower.endswith(".h"):
        return "C++ Header"

    # --- QML Files ---
    if lower.endswith(".qml"):
        if lower == "main.qml":
            return "Root QML UI"
        if "components" in rel_path.lower():
            return "Component UI"
        if "pages" in rel_path.lower():
            return "Pages UI"
        return "Generic UI"

    # --- Markdown / Documentation ---
    if lower.endswith(".md"):
        if "docs/adr" in rel_path.lower():
            return "ADR Doc"
        if "policies" in rel_path.lower():
            return "Policies Doc"
        if "templates" in rel_path.lower():
            return "Template Doc"
        if "releases" in rel_path.lower():
            return "Release Doc"
        if name.upper() == "TESTING.MD":
            return "Testing Doc"
        return "Markdown Doc"

    # --- UML Diagrams ---
    if lower.endswith(".puml"):
        return "UML Diagram"
        
    # --- UML Exports ---
    if lower.endswith(".svg") or lower.endswith(".png"):
        return "UML Export"

    # --- Config / Build Files ---
    if lower.endswith((".yml", ".yaml")):
        return "CI Config"
    if lower.endswith(".cmake") or name == "CMakeLists.txt":
        return "Build Config"

    # --- Python Scripts ---
    if lower.endswith(".py"):
        return "Python Script"

    # --- Readme Special Case ---
    if "readme" in lower:
        return "Markdown Doc"

    # --- Fallback ---
    return "Other"
