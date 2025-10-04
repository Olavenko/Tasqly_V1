# 🎨 QML Style Guide

This document defines **mandatory conventions** for writing QML in the Tasqly project.  
It ensures **consistency, readability, and maintainability** across all UI files.

---

## 1. Imports
- Always import in this order:
  1. QtQuick (base)
  2. QtQuick.Controls
  3. QtQuick.Layouts
  4. Project-specific imports (`Tasqly.Ui.Components`)
- Example:
  ```qml
  import QtQuick 2.15
  import QtQuick.Controls 2.15
  import QtQuick.Layouts 1.15
  import Tasqly.Ui.Components 1.0
  ```

---

## 2. IDs & Naming
- Root element must always have `id: root`.
- Use clear, descriptive IDs: `mainLayout`, `taskListView`, `addButton`.
- Avoid generic IDs like `item1`, `rect2`.

---

## 3. Properties & Signals
- Define **public API** clearly at the top of the file:
  - `property alias`
  - `property var`
  - `signal` definitions
- Add short comments above each property/signal.

---

## 4. Layout & Anchors
- Prefer **Layout objects** (ColumnLayout, RowLayout, GridLayout).  
- Avoid absolute positioning (`x`, `y`) unless absolutely necessary.
- Use `anchors.fill: parent` for default fills.

---

## 5. Styling & Theming
- ❌ Do NOT hardcode colors, fonts, or sizes.  
- ✅ Always use `ThemeManager.palette` or global style constants.  
- Example:
  ```qml
  color: ThemeManager.palette.primary
  font.pixelSize: ThemeManager.fonts.body
  ```

---

## 6. Comments & Sections
- Divide file into clear sections with comments:
  ```qml
  // 👉 Custom Properties
  // 👉 Layout Section
  // 👉 Behavior Section
  ```

---

## 7. Accessibility
- All interactive elements must support keyboard navigation (Tab, Enter, Esc).  
- Provide `Accessible.name` for screen readers where appropriate.

---

## 8. Performance
- Avoid complex bindings inside `Repeater` or `ListView`.  
- Use `Loader` for heavy components (load on demand).  
- Profile UI performance if a page feels slow.

---

## 9. File Naming
- **Pages**: PascalCase + `Page` suffix (e.g., `TaskOverviewPage.qml`).  
- **Components**: PascalCase + descriptive name (e.g., `TaskButton.qml`).  

---

## 10. Examples
```qml
// Good Example
Label {
    id: taskTitle
    text: modelData.title
    color: ThemeManager.palette.text
    font.pixelSize: ThemeManager.fonts.heading
}
```
