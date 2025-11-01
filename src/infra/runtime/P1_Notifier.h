#pragma once
/*
 * 🧱 File: P1_Notifier.h
 * ----------------------
 * 📌 Purpose   : Provides a minimal, pure C++ runtime notifier for console and fallback events.
 * 🧱 Layer     : Infrastructure (Runtime)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * The P1_Notifier is a lightweight, thread-safe notification system that outputs
 * messages to the console and logs them using P1_Logger.
 *
 * It is designed for non-UI runtime feedback — particularly fallback alerts,
 * warnings, or error messages when the app switches to InMemory repositories.
 *
 * ⚙️ Responsibilities:
 *   - Display runtime messages (info/warn/error/fallback)
 *   - Integrate with Logger for persistent logs
 *   - Support silent mode (for CI or production environments)
 *   - No Qt or GUI dependencies
 *
 * 🔗 Depends On:
 *   - P1_Logger
 *   - P1_Error
 *
 * 🧩 Integration Points:
 *   - Used by P1_S2_TaskRepositoryFactory for fallback notifications
 *   - Used by infra/bootstrap for startup/shutdown messages
 */

#include <mutex>
#include <string>

#include "infra/runtime/P1_Error.h"

namespace tasqly::p1::infra::runtime {

// 🧱 Notifier Singleton
class P1_Notifier final
{
public:
  // 🔹 Access global instance
  static P1_Notifier& instance();

  // 🔹 Silent mode (no console output)
  void setSilent(bool enabled);
  bool isSilent() const noexcept;

  // 🔹 Notification methods
  void info(const std::string& message);
  void warn(const std::string& message);
  void error(const std::string& message);
  void error(const P1_Error& err);

  // 🔹 Special event — fallback activation
  void fallbackActivated(const std::string& repoName);

  // 🔹 User-friendly alias for info/warn messages (Toast-like runtime event)
  void toast(const std::string& message);

private:
  // 🧱 Constructor — singleton pattern
  P1_Notifier();

  // 🧩 Helper for console printing
  void print(const std::string& prefix, const std::string& message);

  // 🔒 Members
  bool m_silent = false;
  std::mutex m_mutex;
};

} // namespace tasqly::p1::infra::runtime
