/*
 * 🧱 File: ILogManager.h
 * ----------------------
 * 📌 Purpose   : Logging interface (app layer) for unified logging across the app.
 * 🧱 Layer     : Application (Logging)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-02
 *
 * 🧠 Description:
 * Pure abstract interface to decouple UI/use-cases from the concrete logging backend.
 * Covers: levels, console/file sinks, rotation policy, masking of sensitive fields,
 * and a single entry-point for structured logs with category + context.
 * Implementation will be provided by infra/logging/LogManager.{h,cpp} in Slice 1.
 */

#pragma once

// Qt
#include <QString>
#include <QStringList>
#include <QVariantMap>

namespace tasqly::app::logging {

// 🎚️ Log severity/verbosity levels (app-wide contract)
enum class Level : int {
  Trace = 0,
  Debug = 1,
  Info = 2,
  Warn = 3,
  Error = 4,
  Critical = 5,
  Off = 6
};

// 🔁 File rotation policy (size-based, with rolling files)
struct RotationPolicy
{
  quint64 maxBytes = 5 * 1024 * 1024; // 5 MiB default
  int maxFiles = 3;                   // keep N rotated files
};

// 🪪 Interface for logging manager
class ILogManager
{
public:
  virtual ~ILogManager() = default;

  // ⚙️ Level configuration
  // 🎚️ Set global log level (messages below this level are ignored)
  virtual void setLevel(Level level) = 0;

  // 🔎 Get current global log level
  virtual Level level() const = 0;

  // ✅ Fast-path check before formatting heavy messages
  virtual bool isEnabled(Level level) const = 0;

  // 🖥️ Console sink control
  // 🟢 Enable/disable console logging
  virtual void enableConsole(bool enabled) = 0;

  // 💾 File sink control
  // 🟢 Enable/disable file sink; if enabling with empty path, impl chooses default path
  virtual void enableFileSink(bool enabled, const QString& filePath = QString()) = 0;

  // 🔁 Set rotation policy for the file sink
  virtual void setRotationPolicy(const RotationPolicy& policy) = 0;

  // 🔄 Force rotation now (best-effort; impl may fallback to console on failure)
  virtual void rotateNow() = 0;

  // 🧽 Flush pending buffers (best-effort)
  virtual void flush() = 0;

  // 🫣 Masking rules (by key) — values of these keys will be redacted in output
  // ➕ Add a single key to mask (e.g., "password", "token")
  virtual void addMaskedKey(const QString& key) = 0;

  // 🧾 Replace the full masked keys set
  virtual void setMaskedKeys(const QStringList& keys) = 0;

  // 📋 Inspect currently masked keys (for testing/diagnostics)
  virtual QStringList maskedKeys() const = 0;

  // 🪵 Main logging API (structured)
  // 🏷️ category: logical component name (e.g., "infra.db", "ui.toast")
  // 🧳 context : auxiliary fields (will be masked per rules)
  virtual void log(Level level,
                   const QString& category,
                   const QString& message,
                   const QVariantMap& context = {})
      = 0;
};

} // namespace tasqly::app::logging
