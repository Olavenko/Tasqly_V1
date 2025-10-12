import re
from pathlib import Path

def detect_layer_tags(parts: list[str]) -> list[str]:
    """Detect architectural layer tags (App, Domain, Infra, UI, Tests, Benchmarks)."""
    tags = []
    if "src" in parts:
        if "app" in parts:
            tags.append("App")
        elif "domain" in parts:
            tags.append("Domain")
        elif "infra" in parts:
            tags.append("Infra")
        elif "ui" in parts:
            if "qml" not in parts:
                tags.append("UI")
    elif "tests" in parts:
        if "unit" in parts:
            tags.append("UnitTest")
        elif "integration" in parts:
            tags.append("IntegrationTest")
        elif "qml" in parts:
            tags.append("QmlTest")
        elif "fakes" in parts:
            tags.append("Test/Fake")
    elif "benchmarks" in parts:
        tags.append("Benchmark")
    return tags


def detect_qml_tags(parts: list[str], ftype: str) -> list[str]:
    """Detect QML-related tags such as Component UI or Pages UI."""
    tags = []
    if ftype == "QML UI":
        if "components" in parts:
            tags.append("Component UI")
        elif "pages" in parts:
            tags.append("Pages UI")
        else:
            tags.append("Generic UI")
    return tags


def detect_doc_tags(parts: list[str], filename: str, ftype: str) -> list[str]:
    """Detect documentation-related tags for ADR, Release, and embedded phase/slice refs."""
    tags = []
    if ftype in ("ADR Doc", "Release Doc"):
        for p in parts:
            if re.match(r"^phase\d+(\.\d+)?$", p, re.IGNORECASE):
                num = re.sub(r"^phase", "", p, flags=re.IGNORECASE)
                tags.append(f"Phase{num}")
            if re.match(r"^slice\d+$", p, re.IGNORECASE):
                num = re.sub(r"^slice", "", p, flags=re.IGNORECASE)
                tags.append(f"Slice{num}")
        extracted = extract_phase_slice_from_name(filename)
        if extracted:
            m = re.match(r"(Phase\d+(?:\.\d+)?)/(Slice\d+)", extracted)
            if m:
                tags.append(m.group(1))
                tags.append(m.group(2))
    return tags


def detect_uml_tags(parts: list[str], ftype: str) -> list[str]:
    """Detect UML diagram tags related to Phase and Slice directories."""
    tags = []
    if ftype == "UML Diagram" and "uml" in parts:
        uml_phase, uml_slice = None, None
        for p in parts:
            if re.match(r"^phase\d+(\.\d+)?$", p, re.IGNORECASE):
                num = re.sub(r"^phase", "", p, flags=re.IGNORECASE)
                uml_phase = f"Phase{num}"
            if re.match(r"^slice\d+$", p, re.IGNORECASE):
                num = re.sub(r"^slice", "", p, flags=re.IGNORECASE)
                uml_slice = f"Slice{num}"
        if uml_phase:
            tags.append(uml_phase)
        if uml_slice:
            tags.append(uml_slice)
    return tags


def extract_phase_slice_from_name(name: str):
    """Extract phase/slice info from filename like phase1.2-slice3 → Phase1.2/Slice3"""
    match = re.match(r"phase(\d+)(?:[._](\d+))?-slice(\d+)", name.lower())
    if match:
        phase_main, phase_sub, slice_num = match.groups()
        phase = f"Phase{phase_main}" + (f".{phase_sub}" if phase_sub else "")
        slice_ = f"Slice{slice_num}"
        return f"{phase}/{slice_}"
    return None


def deduplicate_and_normalize(tags: list[str]) -> list[str]:
    """Remove duplicates while preserving order."""
    return list(dict.fromkeys(tags))


def generate_tags(rel_path: str, ftype: str, phase: str, slice_name: str) -> list[str]:
    """
    Generate all tags for a file based on:
    - Directory parts (src, tests, benchmarks, docs, uml, etc.)
    - File type (C++/QML/Docs/UML)
    - Phase/Slice from CLI args (always added)
    """
    parts = Path(rel_path).parts
    filename = Path(rel_path).name
    tags = []

    # Layer/QML/Doc/UML specific tags
    tags += detect_layer_tags(parts)
    tags += detect_qml_tags(parts, ftype)
    tags += detect_doc_tags(parts, filename, ftype)
    tags += detect_uml_tags(parts, ftype)

    # ✅ Always append current Phase/Slice (not conditional anymore)
    phase_tag = phase if phase else "Phase0"
    slice_tag = slice_name if slice_name else "Slice0"
    tags.append(phase_tag)
    tags.append(slice_tag)

    return deduplicate_and_normalize(tags)
