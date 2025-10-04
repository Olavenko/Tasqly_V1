# ADR 0002: Automation-Scripts-for-CMake-Sorting-and-Maintenance

- **Status**: Accepted
- **Date**: 2025-09-06
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice3 (Navigation, Loader & Fallback)
- **Tags**: cmake, automation, scripts, maintenance

---

## Context
To improve maintainability and consistency across the **CMake build system**, we introduced several **sorting scripts** under `/scripts`.  
These scripts automatically reorder specific CMake blocks to enforce a consistent, readable style across the project.

The scripts complement earlier automation (`gen_structure.py`, `gen_adr_index.py`) and are now part of the official tooling.

---

## Problem
- Over time, `CMakeLists.txt` blocks (e.g., `qt_add_qml_module`, `target_sources`, `target_link_libraries`) were growing and becoming **hard to read**.  
- Manual ordering led to inconsistencies between developers.  
- Mixing of layers (App/Domain/Infra) made code navigation harder.  
- Risk of merge conflicts increased due to inconsistent formatting.

---

## Decision
We adopted **Python scripts** to reorder and normalize these blocks.  

### Implemented Scripts
1. **`sort_add_executable.py`**  
   - Reorders `add_executable(TasqlyTests ...)` block.  
   - Groups test sources into:
     - 🔗 Integration Tests  
     - 🛠️ Unit Tests  
     - 🧪 Main Test Files  
     - 📦 Resources  

2. **`sort_qml_module.py`**  
   - Reorders `qt_add_qml_module(...)` block.  
   - Enforces order: App → Domain → Infra → Extras → Resources.  
   - Separates QML core (Main.qml) from extra QML files.

3. **`sort_target_link_libraries.py`**  
   - Reorders `target_link_libraries(TasqlyTests ...)`.  
   - Groups into:  
     - 🧪 Testing (GTest)  
     - 🎨 Qt Libraries  
     - 📦 Other Libraries  

4. **`sort_target_sources.py`**  
   - Reorders `target_sources(TasqlyTests ...)`.  
   - Groups into:  
     - 🏛️ App Layer  
     - 🧩 Domain Layer  
     - 🏗️ Infra Layer  
     - 📂 Other  

---

## How to Use
Run scripts directly from project root (after editing CMake files):  

```bash
python scripts/sort_add_executable.py         --> py -3.13 scripts\sort_add_executable.py
python scripts/sort_qml_module.py             --> py -3.13 scripts\sort_qml_module.py
python scripts/sort_target_link_libraries.py  --> py -3.13 scripts\sort_target_link_libraries.py
python scripts/sort_target_sources.py         --> py -3.13 scripts\sort_target_sources.py
