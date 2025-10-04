/*
 * 🧱 File: ThemeManager.h
 * -----------------------
 * 📌 Purpose   : Manages Light/Dark theme palettes and font tokens at runtime.
 * 🧱 Layer     : Application (Theme)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-04
 * 🔖 Version   : 1.1 (Fonts + Transparent support)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Exposes current theme mode, palette, and fonts as QML-friendly properties.
 * Supports switching between Light/Dark and notifies QML about updates.
 */

#pragma once

#include "domain/core/ThemeMode.h"
#include <QObject>
#include <QVariantMap>

class ThemeManager : public QObject
{
  Q_OBJECT
  Q_PROPERTY(tasqly::domain::core::ThemeMode mode READ mode NOTIFY modeChanged)
  Q_PROPERTY(QVariantMap palette READ palette NOTIFY paletteChanged)
  Q_PROPERTY(QVariantMap fonts READ fonts NOTIFY fontsChanged)

public:
  using ThemeMode = tasqly::domain::core::ThemeMode;

  explicit ThemeManager(QObject* parent = nullptr);

  // 🎨 Get current theme mode
  ThemeMode mode() const;

  // 🎨 Switch theme mode (Light/Dark)
  Q_INVOKABLE void switchTo(ThemeMode newMode);

  // 🎨 Get current palette as a key-value map (QML-friendly)
  QVariantMap palette() const;

  // 🔤 Get current font tokens (QML-friendly)
  QVariantMap fonts() const;

signals:
  // 🔔 Emitted when theme mode changes
  void modeChanged(ThemeMode newMode);

  // 🔔 Emitted when palette changes
  void paletteChanged(QVariantMap newPalette);

  // 🔔 Emitted when font tokens change
  void fontsChanged(QVariantMap newFonts);

private:
  ThemeMode m_mode{ThemeMode::Light};
  QVariantMap m_palette;
  QVariantMap m_fonts;

  void updatePalette();
  void updateFonts();
};
