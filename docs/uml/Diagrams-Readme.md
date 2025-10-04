# 📐 UML Guidelines — Tasqly Project

This directory contains all **PlantUML** diagrams for the project (Use Case, Sequence, Component, …).
The goal is to unify diagram styling using a **Corporate Dark Theme**, a **Theme Toggle Flag**, and **FA6 icon macros**.

---

## 🧱 Basics

* Every diagram must start with:

  ```puml
  @startuml
  !include ../../config/style.puml
  ```

* And must end with:

  ```puml
  @enduml
  ```

* The **style.puml** (in `docs/uml/config/style.puml`) includes:

  * Corporate Dark Theme (WCAG-friendly).
  * **Theme Toggle Flag** (`USE_CUSTOM_THEME`) to switch between **custom colors** and **PlantUML defaults**.

    * Enable custom colors:

      ```puml
      !define USE_CUSTOM_THEME
      !include ../../config/style.puml
      ```
    * Disable (fallback to PlantUML-like defaults):

      ```puml
      !include ../../config/style.puml
      ```
  * **Icon Color Mapping** to align FA6 icons with PlantUML defaults (e.g., Palette = yellow, Notifier = yellow, Database = wheat).
  * Font settings (Segoe UI with fallback).
  * Includes for **font-awesome-6** icon macros.
  * Includes for **C4-PlantUML** DSL.

---

## 🎨 Icons (FA6 Macros)

* Always use the FA6 macros defined in `style.puml` instead of raw shapes.
* Example:

```puml
FA6_USER(dev, Developer, rectangle, #00EB09)
FA6_GITHUB(repo, GitHub_Repo, rectangle, #2AA1D1)
FA6_CLOUD(ci, CI_Pipeline, rectangle, #00BFFF)
FA6_GEAR(build, Build_System, rectangle, #D97A2B)
FA6_FLASK(tests, Test_Runner, rectangle, #E5C55F)
```

* Each macro creates a **shape + icon + label**.
* Default icon colors are set by **flags** in `style.puml`.

---

## 📂 Folder Structure

---

├── Diagrams-Readme.md [Markdown Doc]
├── config
│   └── style.puml [UML Diagram]
├── exports
│   └── index.html
├── phase0
│   ├── s0
│   │   ├── component.puml [UML Diagram]
│   │   ├── exports
│   │   │   ├── component.svg
│   │   │   ├── sequence.svg
│   │   │   └── usecase.svg
│   │   ├── sequence.puml [UML Diagram]
│   │   └── usecase.puml [UML Diagram]
│   ├── s1
│   │   ├── component.puml [UML Diagram]
│   │   ├── component_with_flags.puml [UML Diagram]
│   │   ├── exports
│   │   │   ├── component.svg
│   │   │   ├── component_with_flags.svg
│   │   │   ├── sequence.svg
│   │   │   ├── sequence_with_flags.svg
│   │   │   ├── usecase.svg
│   │   │   └── usecase_with_flags.svg
│   │   ├── sequence.puml [UML Diagram]
│   │   ├── sequence_with_flags.puml [UML Diagram]
│   │   ├── usecase.puml [UML Diagram]
│   │   └── usecase_with_flags.puml [UML Diagram]
│   ├── s2
│   │   ├── component.puml [UML Diagram]
│   │   ├── exports
│   │   │   ├── component.svg
│   │   │   ├── sequence.svg
│   │   │   └── usecase.svg
│   │   ├── sequence.puml [UML Diagram]
│   │   └── usecase.puml [UML Diagram]
│   ├── s3
│   │   ├── component.puml [UML Diagram]
│   │   ├── exports
│   │   │   ├── component.svg
│   │   │   ├── sequence.svg
│   │   │   └── usecase.svg
│   │   ├── sequence.puml [UML Diagram]
│   │   └── usecase.puml [UML Diagram]
│   └── s4
│       ├── component.puml [UML Diagram]
│       ├── exports
│       │   ├── component.svg
│       │   ├── sequence.svg
│       │   ├── sequence_tasklist.svg
│       │   └── usecase.svg
│       ├── sequence.puml [UML Diagram]
│       ├── sequence_tasklist.puml [UML Diagram]
│       └── usecase.puml [UML Diagram]
├── tools
│   └── plantuml.jar [Tool]
├── uml-export.py [Python Script]
└── uml-export.sh [Script]

---

📊 Summary
- Directories: 14
- Files: 44
⏱️ Updated: 2025-09-28 10:09:55

---

## ✨ Examples

### Use Case

```puml
@startuml
' 🧱 File: usecase.puml
' ---------------------
' 📌 Purpose : Use Case — "Navigate to Page" / "Handle Route Error"
' 🧱 Layer   : Docs (UML)
' 👤 Author  : Mohamed Ali
' 🗓️ Created : 2025-09-05

!define USE_CUSTOM_THEME
!include ../../config/style.puml

' 🧑 Actor
FA6_USER(user, "User", rectangle, #3FB950)

' 🎯 Use Cases
rectangle "Navigation Use Cases" {
  usecase UC_Navigate as "Navigate to Page"
  usecase UC_Error as "Handle Route Error"
}

' 🖥️ Components with icons
FA6_DESKTOP(sidebar, "Sidebar (QML)", rectangle, #58A6FF)
FA6_CUBES(nav, "NavigationService", rectangle, #A371F7)
FA6_LAYER_GROUP(stack, "StackView", rectangle, #39C5CF)
FA6_CUBES(loader, "SmartPageLoader", rectangle, #FFD700)
FA6_BELL(fallback, "SmartFallback", rectangle, #FFA500)

' 🔗 Relations
user --> sidebar
sidebar --> UC_Navigate
UC_Navigate --> nav
UC_Navigate --> stack
UC_Navigate --> loader
UC_Error --> fallback

@enduml
```

### Sequence

```puml
@startuml
' 🧱 File: sequence.puml
' ----------------------
' 📌 Purpose : Sequence — "Sidebar → NavigationService → StackView → SmartPageLoader → (success|fallback)"
' 🧱 Layer   : Docs (UML)
' 👤 Author  : Mohamed Ali
' 🗓️ Created : 2025-09-05

!define USE_CUSTOM_THEME
!include ../../config/style.puml

' 🧑 Participants
FA6_USER(USER, "User", participant, #3FB950)
FA6_DESKTOP(SIDEBAR, "Sidebar (QML)", participant, #58A6FF)
FA6_CUBES(NAV, "NavigationService", participant, #A371F7)
FA6_LAYER_GROUP(STACK, "StackView", participant, #39C5CF)
FA6_CUBES(LOADER, "SmartPageLoader", participant, #FFD700)
FA6_BELL(FALLBACK, "SmartFallback", participant, #FFA500)

' 🔗 Flow
USER -> SIDEBAR : click(pageId)
SIDEBAR -> NAV : navigate(pageId)
NAV -> STACK : push(route)
STACK -> LOADER : loadPage(route)

alt success
    LOADER -> STACK : show(page)
else invalid route
    NAV -> FALLBACK : show("❌ Unknown route")
    FALLBACK --> SIDEBAR : toast + no stack change
end

@enduml
```

### Component

```puml
@startuml
' 🧱 File: component.puml
' -----------------------
' 📌 Purpose : Component — "NavigationService, PageLoader, Fallback"
' 🧱 Layer   : Docs (UML)
' 👤 Author  : Mohamed Ali
' 🗓️ Created : 2025-09-05

!define USE_CUSTOM_THEME
!include ../../config/style.puml

' 🧑 Components
FA6_CUBES(appNav, "App.Navigation", rectangle, #58A6FF)
FA6_CUBES(navService, "NavigationService", rectangle, #A371F7)
FA6_CUBES(route, "Route (struct)", rectangle, #39C5CF)
FA6_LAYER_GROUP(stackView, "StackView", rectangle, #43FFE6)
FA6_CUBES(pageLoader, "SmartPageLoader", rectangle, #FFD700)
FA6_BELL(fallbackUI, "SmartFallback", rectangle, #FFA500)

' 🔗 Relations
appNav --> navService
navService --> route
navService --> stackView
stackView --> pageLoader
pageLoader --> fallbackUI

@enduml
```

---

## 📝 Notes

* Do **not** duplicate icon or C4 includes inside diagrams → everything is centralized in `style.puml`.
* Use the **Theme Toggle Flag** to switch between Corporate Dark Theme (`!define USE_CUSTOM_THEME`) and PlantUML defaults (no define).
* Icon colors are also mapped via defines (`ICON_COLOR_PALETTE`, `ICON_COLOR_NOTIFIER`, etc.) in `style.puml`.
* Any library or icon updates must be done in `style.puml` only.
* Official exports in CI will be managed via workflows (`uml.yml`, `uml-pages.yml`).

---

🔗 **Reference Libraries**

* [C4-PlantUML](https://github.com/plantuml-stdlib/C4-PlantUML)
* [tupadr3 PlantUML Icons](https://github.com/tupadr3/plantuml-icon-font-sprites)
