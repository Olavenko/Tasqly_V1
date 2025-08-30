# 📐 UML Guidelines — Tasqly Project

This directory contains all **PlantUML** diagrams for the project (Use Case, Sequence, Component, …).  
The goal is to unify diagram styling using a **Corporate Dark Theme** and **FA6 icon macros** from [tupadr3/plantuml-icon-font-sprites](https://github.com/tupadr3/plantuml-icon-font-sprites).

---

## 🧱 Basics
- Every diagram must start with:
  ```puml
  @startuml
  !include ../../config/style.puml
  ```
- And must end with:
  ```puml
  @enduml
  ```
- The **style.puml** (in `docs/uml/config/style.puml`) includes:
  - Color and background theme (Corporate Dark).
  - Font settings (Segoe UI with fallback).
  - Includes for **font-awesome-6** icon macros.
  - Includes for **C4-PlantUML**.

---

## 🎨 Icons (FA6 Macros)
Instead of defining shapes manually, always use the FA6 macros:

```puml
FA6_USER(dev, "Developer", rectangle, #00EB09)
FA6_GITHUB(repo, "GitHub Repo", rectangle, #2AA1D1)
FA6_GEAR(build, "Build System", rectangle, #D97A2B)
FA6_FLASK(tests, "Test Runner", rectangle, #E5C55F)
FA6_DATABASE(db, "PostgreSQL DB", rectangle, #FF9800)
```

- Each macro creates a **shape + icon + label + color**.  
- Use the `alias` in relations:
  ```puml
  dev --> repo
  repo --> ci
  ```

---

## 📂 Folder Structure
```
docs/uml/
 ├── config/       # style.puml (style + includes)
 ├── phase0/       # Phase slices
 │    └── s0/
 │         ├── usecase.puml
 │         ├── sequence.puml
 │         ├── component.puml
 │         └── exports/   # Generated SVGs
 ├── tools/        # Tools (plantuml.jar)
 ├── uml-export.sh # Export script
 └── README.md     # This file
```

---

## ⚙️ Export
To generate SVG diagrams locally:
```bash
bash docs/uml/uml-export.sh
```

Outputs are always stored in the `exports/` folder next to each `.puml` file.

---

## ✨ Examples

### Use Case
```puml
@startuml
!include ../../config/style.puml
title Use Case — Project Initialization

FA6_USER(dev, "Developer", rectangle, #00EB09)
FA6_CLOUD(ci, "CI System", rectangle, #00BFFF)

rectangle "Tasqly Project Initialization" {
  usecase UC1 as "Commit Code"
  usecase UC2 as "Trigger CI Pipeline"
  usecase UC3 as "Build & Run Tests"
  usecase UC4 as "Report Status"
}

dev --> UC1
UC1 --> UC2
ci --> UC3
UC3 --> UC4
UC4 --> dev
@enduml
```

### Sequence
```puml
@startuml
!include ../../config/style.puml
title Sequence — Developer Commit to CI Result

FA6_USER(dev, "Developer", rectangle, #00EB09)
FA6_GITHUB(repo, "GitHub Repo", rectangle, #2AA1D1)
FA6_CLOUD(ci, "CI Pipeline", rectangle, #00BFFF)
FA6_GEAR(build, "Build System", rectangle, #D97A2B)
FA6_FLASK(tests, "Test Runner", rectangle, #E5C55F)

dev -> repo : Push Commit
repo -> ci : Trigger Pipeline
ci -> build : Compile Sources
build -> tests : Run Unit Tests
tests --> ci : Report Test Results
ci --> dev : Notify Status (✅ / ❌)
@enduml
```

### Component
```puml
@startuml
!include ../../config/style.puml
title Component — Project Initialization & Testing

FA6_DESKTOP(app, "TasqlyApp", rectangle, blue)
FA6_FLASK(tests, "TasqlyTests", rectangle, #E5C55F)

FA6_GEAR(cmake, "CMake", rectangle, #D97A2B)
FA6_WRENCH(clang, "Clang Tools", rectangle, #D97A2B)
FA6_BUG(cppcheck, "CppCheck", rectangle, #E94C3D)
FA6_SITEMAP(uml, "PlantUML", rectangle, #4CAF50)

FA6_CLOUD(ci, "GitHub Actions", rectangle, #00BFFF)
FA6_USER(dev, "Developer", rectangle, #00EB09)
FA6_DATABASE(db, "PostgreSQL DB", rectangle, #FF9800)

app --> cmake
tests --> cmake
cmake --> ci
clang --> ci
cppcheck --> ci
uml --> ci
dev --> app
app --> db
@enduml
```

---

## 📝 Notes
- **Do not** duplicate icon or C4 includes inside diagrams → everything is managed in `style.puml`.  
- Any library or icon updates must be done in `style.puml` only.  
- Official exports in CI will be managed later via workflows (`uml.yml`, `uml-pages.yml`).  

---

🔗 **Reference Libraries**  
- [C4-PlantUML](https://github.com/plantuml-stdlib/C4-PlantUML)  
- [tupadr3 PlantUML Icons](https://github.com/tupadr3/plantuml-icon-font-sprites)  
