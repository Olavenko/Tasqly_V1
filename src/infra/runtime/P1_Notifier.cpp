/*
 * 🧱 File: P1_Notifier.cpp
 * ------------------------
 * 📌 Purpose   : Implementation of P1_Notifier — lightweight console notifier with logger integration.
 * 🧱 Layer     : Infrastructure (Runtime)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 1.0
 *
 * 🧠 Description:
 * Provides runtime feedback for events such as initialization, fallback, and critical errors.
 * Outputs messages to both the console and the P1_Logger system.
 * Thread-safe and independent of any UI or Qt dependencies.
 *
 * 🔗 Related:
 *   - P1_Notifier.h
 *   - P1_Logger
 *   - P1_Error
 */

#include "P1_Notifier.h"
#include "P1_Logger.h"

#include <iostream>
#include <sstream>

namespace tasqly::p1::infra::runtime {

// ⚙️ Singleton accessor
P1_Notifier& P1_Notifier::instance()
{
  static P1_Notifier notifier;
  return notifier;
}

// 🧱 Constructor
P1_Notifier::P1_Notifier()
    : m_silent(false)
{
  // Default: console + logger enabled
  P1_Logger::instance().info("P1_Notifier initialized (console + logger active)");
}

// 🧩 Silent mode
void P1_Notifier::setSilent(bool enabled)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_silent = enabled;
  P1_Logger::instance().info(std::string("Notifier silent mode: ") + (enabled ? "ON" : "OFF"));
}

bool P1_Notifier::isSilent() const noexcept
{
  return m_silent;
}

// 🧩 Print helper (thread-safe)
void P1_Notifier::print(const std::string& prefix, const std::string& message)
{
  if (m_silent)
    return;

  std::lock_guard<std::mutex> lock(m_mutex);
  std::cout << prefix << " " << message << std::endl;
}

// 🧩 Info message
void P1_Notifier::info(const std::string& message)
{
  print("[INFO]", message);
  P1_Logger::instance().info(message);
}

// 🧩 Warning message
void P1_Notifier::warn(const std::string& message)
{
  print("[WARN]", message);
  P1_Logger::instance().warn(message);
}

// 🧩 Error message (string)
void P1_Notifier::error(const std::string& message)
{
  print("[ERROR]", message);
  P1_Logger::instance().error(message);
}

// 🧩 Error message (structured P1_Error)
void P1_Notifier::error(const P1_Error& err)
{
  std::ostringstream oss;
  oss << "[ERROR] " << err.toString(true);
  print("[ERROR]", err.toString(true));
  P1_Logger::instance().error(err);
}

// 🧩 Special event — fallback to InMemory
void P1_Notifier::fallbackActivated(const std::string& repoName)
{
  std::ostringstream msg;
  msg << "⚠️  Database fallback activated — switched to [" << repoName << "]";
  print("[FALLBACK]", msg.str());
  P1_Logger::instance().warn(msg.str());
}

// 🧩 Toast (console-level short feedback)
void P1_Notifier::toast(const std::string& message)
{
  if (isSilent())
    return;

  // ⚙️ Choose appropriate prefix automatically
  if (message.find("⚠️") != std::string::npos)
    warn(message);
  else if (message.find("❌") != std::string::npos)
    error(message);
  else
    info(message);
}

} // namespace tasqly::p1::infra::runtime
