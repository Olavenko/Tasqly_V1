# ADR 0007 — QML File Policy

* **Status**: Accepted  
* **Date**: 2025-09-07  
* **Author**: Mohamed Ali  

---

## Context
During early UI development, inconsistencies appeared:  
- Hardcoded colors and fonts created a fragmented look.  
- Different developers used varying import orders and IDs.  
- Accessibility and testing were often skipped.  

To prevent these issues, a **QML File Policy** is established to ensure consistent, testable, and maintainable UI code.

---

## Decision

### 1. Page Files (`*.qml`)
- Must begin with a **header comment block** (see QmlPageTemplate).  
- Root element must be `Page`.  
- Must declare `id: root`.  
- Must use **NavigationService** for routing.  
- Must rely on **ThemeManager** for colors, fonts, icons.  

### 2. Component Files (`*.qml`)
- Must begin with a **header comment block** (see QmlComponentTemplate).  
- Root element must be `Item` or `Control`.  
- Public API defined via `property alias` / `signal` at the top.  
- Must not hardcode colors or fonts.  
- Must support focus/keyboard navigation if interactive.  

### 3. General Rules
- Imports must follow order: QtQuick → Controls → Layouts → Project imports.  
- Every file must have `id: root`.  
- Properties, signals, and layout separated into clear sections.  
- Accessibility required for all interactive elements (`Accessible.name`).  
- Every major component/page must have a **QtTest test case**.  

---

## ✅ Do / ❌ Don’t Examples

```qml
// ✅ Do: Use ThemeManager
color: ThemeManager.palette.primary

// ❌ Don’t: Hardcode color
color: "blue"
```

```qml
// ✅ Do: Proper imports order
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Tasqly.Ui.Components 1.0

// ❌ Don’t: Random order, old versions
import QtQuick 2.0
import Tasqly.Ui.Components 1.0
import QtQuick.Controls 1.4
```

---

## Testing & Accessibility Requirements
- Each Page/Component must have at least **one test case** (unit, snapshot, or integration).  
- Interactive elements must expose `Accessible.name`.  
- Default states should be tested (`visible`, `enabled`, etc.).  

---

## Consequences
- UI remains visually consistent and professional.  
- Testing becomes predictable and scalable.  
- Accessibility ensures broader usability.  
- Prevents fragmentation as the project grows.  

---

## ✅ Quick Checklist (For New QML Files)

| Item                       | Required |
|----------------------------|----------|
| Header block present       |    ☑    |
| Root element (Page/Item)   |    ☑    |
| `id: root` declared        |    ☑    |
| Imports in correct order   |    ☑    |
| ThemeManager used          |    ☑    |
| No hardcoded colors        |    ☑    |
| Properties & signals clear |    ☑    |
| Accessibility set          |    ☑    |
| Test case available        |    ☑    |
| Last Major Change noted    |    ☑    |

---

## References
- [QmlPageTemplate.md](../Templates/QmlPageTemplate.md)  
- [QmlComponentTemplate.md](../Templates/QmlComponentTemplate.md)  
- [QmlStyleGuide.md](../Templates/QmlStyleGuide.md)  
- [QmlTestingTemplate.md](../Templates/QmlTestingTemplate.md)  
