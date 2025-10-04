/*
 * 🧱 File: PageState.h
 * --------------------
 * 📌 Purpose   : Represents UI page lifecycle states for loader/fallback logic.
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-05
 *
 * 🧠 Description:
 * Defines the `PageState` enum for SmartPageLoader and NavigationService.
 * Now based on QObject (instead of Q_GADGET) to provide full Qt MetaObject
 * support. Enables use in signals, QML bindings, and QSignalSpy tests.
 */

#pragma once

#include <QObject>
#include <QString>

// 🎛️ Lifecycle states of a page in the UI
class PageStateWrapper : public QObject
{
  Q_OBJECT
public:
  using QObject::QObject; // inherit constructors

  enum PageState {
    Ready,   // ✅ Content loaded and displayed
    Loading, // ⏳ Currently loading content
    Empty,   // 📭 No data to display
    Error    // ❌ Failed to load or invalid route
  };
  Q_ENUM(PageState)
};

// 🧩 Convert PageState → QString (for logging & QML)
inline QString toString(PageStateWrapper::PageState state)
{
  switch (state) {
  case PageStateWrapper::Ready:
    return "Ready";
  case PageStateWrapper::Loading:
    return "Loading";
  case PageStateWrapper::Empty:
    return "Empty";
  case PageStateWrapper::Error:
    return "Error";
  }
  return "Unknown";
}
