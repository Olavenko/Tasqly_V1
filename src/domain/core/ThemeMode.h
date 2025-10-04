/*
 * 🧱 File: ThemeMode.h
 * --------------------
 * 📌 Purpose   : Strongly-typed enum for supported theme modes (Light/Dark/System).
 * 🧱 Layer     : Domain (Core)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-04
 * 🗓️ Updated   : 2025-09-22
 * 🔖 Version   : 2.0 (Refactored to enum class, removed wrapper)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Provides a strongly-typed ThemeMode enum exposed to QML via Q_ENUM_NS.
 * - Eliminates old wrapper class (ThemeTypes).
 * - Adds "System" mode for OS preference support.
 * - Used by ThemeManager and AppSettingsManager for theme logic.
 *
 * 🔗 Depends On:
 *   - QtCore (QObject, QString)
 * 🔗 Related   : ThemeManager.h/.cpp, AppSettingsManager.h/.cpp
 * ✅ Tested In : Bench_AppSettingsManager.cpp, Bench_ThemeManager.cpp
 * ⚠️ Errors    : Invalid strings default to ThemeMode::Light
 * ⚡ Performance: O(1) conversions via switch/if
 * 🔒 Security  : N/A
 * 🔒 Thread Safety: Enum is POD, thread-safe
 * 🧭 Benchmark Measurement Policy: Not benchmarked directly, measured via managers
 * 🕒 Last Major Change: 2025-09-22
 */

#pragma once

#include <QString>

namespace tasqly::domain::core {

/// 🎨 Application theme modes
enum class ThemeMode {
  Light = 0, ///< Light theme
  Dark = 1,  ///< Dark theme
  System = 2 ///< Follow OS / system preference
};

/// 🛠️ Utilities for ThemeMode (string ↔ enum conversions)
struct ThemeModeUtils
{
  static QString toString(ThemeMode mode)
  {
    switch (mode) {
    case ThemeMode::Light:
      return "light";
    case ThemeMode::Dark:
      return "dark";
    case ThemeMode::System:
      return "system";
    }
    return "light"; // fallback
  }

  static ThemeMode fromString(const QString& str)
  {
    if (str.compare("dark", Qt::CaseInsensitive) == 0)
      return ThemeMode::Dark;
    if (str.compare("system", Qt::CaseInsensitive) == 0)
      return ThemeMode::System;
    return ThemeMode::Light;
  }
};

} // namespace tasqly::domain::core
