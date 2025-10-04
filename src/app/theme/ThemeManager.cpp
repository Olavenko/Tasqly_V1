/*
 * 🧱 File: ThemeManager.cpp
 * -------------------------
 * 📌 Purpose   : Implementation of runtime theme manager (Light/Dark palettes + fonts).
 * 🧱 Layer     : Application (Theme)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-04
 * 🔖 Version   : 1.1 (Fonts + Transparent support)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Handles theme switching at runtime, generates palette & font tokens, and emits
 * signals for QML bindings. Works independently from settings persistence.
 */

#include "ThemeManager.h"
#include <QString>

// ➕ Constructor: default to Light mode
namespace {

const QVariantMap& lightPalette()
{
  static const QVariantMap palette{
      {QStringLiteral("background"), QStringLiteral("#FFFFFF")},
      {QStringLiteral("text"), QStringLiteral("#000000")},
      {QStringLiteral("primary"), QStringLiteral("#1976D2")},
      {QStringLiteral("secondary"), QStringLiteral("#424242")},
      {QStringLiteral("error"), QStringLiteral("#D32F2F")},
      {QStringLiteral("transparent"), QStringLiteral("transparent")},
  };
  return palette;
}

const QVariantMap& darkPalette()
{
  static const QVariantMap palette{
      {QStringLiteral("background"), QStringLiteral("#121212")},
      {QStringLiteral("text"), QStringLiteral("#FFFFFF")},
      {QStringLiteral("primary"), QStringLiteral("#90CAF9")},
      {QStringLiteral("secondary"), QStringLiteral("#BDBDBD")},
      {QStringLiteral("error"), QStringLiteral("#EF5350")},
      {QStringLiteral("transparent"), QStringLiteral("transparent")},
  };
  return palette;
}

const QVariantMap& fontTokens()
{
  static const QVariantMap fonts{
      {QStringLiteral("heading"), 20},
      {QStringLiteral("body"), 16},
      {QStringLiteral("caption"), 12},
  };
  return fonts;
}

} // namespace

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent)
    , m_mode(ThemeMode::Light)
{
  updatePalette();
  updateFonts();
}

// 🎨 Get current theme mode
ThemeManager::ThemeMode ThemeManager::mode() const
{
  return m_mode;
}

// 🎨 Switch to new mode
void ThemeManager::switchTo(ThemeMode newMode)
{
  if (m_mode == newMode)
    return;
  m_mode = newMode;
  updatePalette();
  emit modeChanged(m_mode);
  emit paletteChanged(m_palette);
  emit fontsChanged(m_fonts);
}

// 🎨 Get current palette
QVariantMap ThemeManager::palette() const
{
  return m_palette;
}

// 🔤 Get current fonts
QVariantMap ThemeManager::fonts() const
{
  return m_fonts;
}

// 🎨 Update palette tokens based on current mode
void ThemeManager::updatePalette()
{
  m_palette = (m_mode == ThemeMode::Light) ? lightPalette() : darkPalette();
}

// 🔤 Update font tokens (static for now, can be dynamic later)
void ThemeManager::updateFonts()
{
  m_fonts = fontTokens();
}
