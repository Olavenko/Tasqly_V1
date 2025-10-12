def classify_file_with_context(rel_path: str, name: str) -> str:
    """
    Classify file type based on extension, name, and relative path context.
    Supports C++, QML, Docs, Scripts, Configs, UML, and Misc.
    """

    lower = name.lower()
    rel_lower = rel_path.lower()

    # --- C++ Files ---
    if lower.endswith((".cpp", ".cc", ".cxx")):
        if lower.startswith(("bench_", "bench")):
            return "C++ Benchmark"
        if lower.startswith(("test_", "tests_")):
            return "C++ Test"
        if lower == "main.cpp":
            return "App Entry Point"
        return "C++ Source"

    if lower.endswith((".h", ".hpp", ".hxx")):
        return "C++ Header"

    # --- QML Files ---
    if lower.endswith(".qml"):
        if lower == "main.qml":
            return "Root QML UI"
        # Normalize all other QML types to "QML UI" for summary consistency
        return "QML UI"

    # --- Markdown / Documentation ---
    if lower.endswith(".md"):
        if "docs/adr" in rel_lower:
            return "ADR Doc"
        if "policies" in rel_lower:
            return "Policies Doc"
        if "templates" in rel_lower:
            return "Template Doc"
        if "releases" in rel_lower:
            return "Release Doc"
        if name.upper() == "TESTING.MD":
            return "Testing Doc"
        return "Markdown Doc"

    # --- UML Diagrams & Exports ---
    if lower.endswith(".puml"):
        return "UML Diagram"
    if lower.endswith((".svg", ".png")) and "uml" in rel_lower:
        return "UML Diagram"

    # --- Config / Build Files ---
    if lower.endswith((".yml", ".yaml")):
        return "CI Config"
    if lower.endswith(".cmake") or name == "CMakeLists.txt":
        return "Build Config"

    # --- Python Scripts ---
    if lower.endswith(".py"):
        return "Python Script"

    # --- README special case ---
    if "readme" in lower:
        return "Markdown Doc"

    # --- Fallback ---
    return "Other"
