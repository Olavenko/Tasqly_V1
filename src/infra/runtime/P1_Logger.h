#pragma once
/*
 * 🧱 File: P1_Logger.h
 * --------------------
 * 📌 Purpose   : Defines a pure C++ thread-safe logger for infrastructure and runtime subsystems.
 * 🧱 Layer     : Infrastructure (Runtime)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * The P1_Logger provides a lightweight, thread-safe logging mechanism independent of Qt.
 * It supports log levels (TRACE, INFO, WARN, ERROR), timestamped outputs, and optional
 * masking of sensitive data through integration with P1_Error.
 *
 * ✅ Key Features:
 *   - Thread-safe writes (via std::mutex)
 *   - Console + optional file sink
 *   - Masking support (credentials, tokens)
 *   - Singleton instance (lazy-initialized)
 *
 * 🔗 Depends On:
 *   - P1_Error (for structured error logging)
 *   - <fstream>, <mutex>, <chrono>, <iostream>
 *
 * ⚙️ Integration:
 *   Used by: P1_S2_TaskRepositoryFactory, P1_Notifier
 */

#include <fstream>
#include <mutex>
#include <string>

#include "infra/runtime/P1_Error.h"

namespace tasqly::p1::infra::runtime {

// 🧩 Enumerates available log levels
enum class LogLevel { Trace = 0, Info, Warn, Error };

// 🧱 Core Logger — Singleton
class P1_Logger final
{
public:
  // 🔹 Access singleton instance
  static P1_Logger& instance();

  // 🔹 Configure output
  void setLogFile(const std::string& path);
  void setMinimumLevel(LogLevel level);

  // 🔹 Write logs (string-based)
  void trace(const std::string& msg);
  void info(const std::string& msg);
  void warn(const std::string& msg);
  void error(const std::string& msg);

  // 🔹 Write structured error logs
  void error(const P1_Error& err);

  // 🔹 Utility
  static std::string levelToString(LogLevel lvl);
  static std::string timestamp();

private:
  // 🧩 Internal writer
  void write(LogLevel level, const std::string& text);

  // 🧱 Constructor (private for singleton)
  P1_Logger();

  // 🧱 State
  std::mutex m_mutex;
  std::ofstream m_file;
  LogLevel m_minLevel = LogLevel::Trace;
  bool m_fileEnabled = false;
};

} // namespace tasqly::p1::infra::runtime
