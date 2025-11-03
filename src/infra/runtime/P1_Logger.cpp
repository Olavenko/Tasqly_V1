/*
 * 🧱 File: P1_Logger.cpp
 * ----------------------
 * 📌 Purpose   : Implementation of P1_Logger — thread-safe logging system for runtime subsystems.
 * 🧱 Layer     : Infrastructure (Runtime)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 1.1 (Added capability checks)
 *
 * 🧠 Description:
 * Implements a pure C++ logging facility with console and optional file output.
 * Thread-safe and mask-aware, designed for use by Notifier, Repository Factory,
 * and other runtime infrastructure components.
 *
 * 🔗 Related:
 *   - P1_Logger.h
 *   - P1_Error.h
 */

#include "P1_Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace tasqly::p1::infra::runtime {

// ⚙️ Singleton instance
P1_Logger& P1_Logger::instance()
{
  static P1_Logger logger;
  return logger;
}

// 🧩 Constructor (private)
P1_Logger::P1_Logger()
{
  // Default console-only logger
  m_minLevel = LogLevel::Trace;
  m_fileEnabled = false;
}

// 🧱 Set output file
void P1_Logger::setLogFile(const std::string& path)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_file.open(path, std::ios::app);
  m_fileEnabled = m_file.is_open();
  if (!m_fileEnabled) {
    std::cerr << "[LOGGER] Failed to open log file: " << path << std::endl;
  } else {
    std::cout << "[LOGGER] File logging enabled: " << path << std::endl;
  }
}

// 🧱 Minimum log level
void P1_Logger::setMinimumLevel(LogLevel level)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_minLevel = level;
}

// 🧩 Level utilities
std::string P1_Logger::levelToString(LogLevel lvl)
{
  switch (lvl) {
  case LogLevel::Trace:
    return "TRACE";
  case LogLevel::Info:
    return "INFO";
  case LogLevel::Warn:
    return "WARN";
  case LogLevel::Error:
    return "ERROR";
  default:
    return "UNKNOWN";
  }
}

// 🧩 Timestamp utility
std::string P1_Logger::timestamp()
{
  using namespace std::chrono;
  const auto now = system_clock::now();
  const std::time_t now_c = system_clock::to_time_t(now);
  std::tm tm_buf{};
#if defined(_WIN32)
  localtime_s(&tm_buf, &now_c);
#else
  localtime_r(&now_c, &tm_buf);
#endif
  std::ostringstream oss;
  oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

// 🧱 Core write function
void P1_Logger::write(LogLevel level, const std::string& text)
{
  if (level < m_minLevel)
    return;

  std::lock_guard<std::mutex> lock(m_mutex);

  const std::string line = "[" + timestamp() + "][" + levelToString(level) + "]: " + text;

  // Console sink
  std::cout << line << std::endl;

  // File sink (optional)
  if (m_fileEnabled && m_file.is_open()) {
    m_file << line << std::endl;
    m_file.flush();
  }
}

// 🧩 Public log methods
void P1_Logger::trace(const std::string& msg)
{
  write(LogLevel::Trace, msg);
}
void P1_Logger::info(const std::string& msg)
{
  write(LogLevel::Info, msg);
}
void P1_Logger::warn(const std::string& msg)
{
  write(LogLevel::Warn, msg);
}
void P1_Logger::error(const std::string& msg)
{
  write(LogLevel::Error, msg);
}

// 🧱 Structured P1_Error logging
void P1_Logger::error(const P1_Error& err)
{
  std::ostringstream oss;
  oss << err.toString(true); // Mask sensitive data
  write(LogLevel::Error, oss.str());
}

} // namespace tasqly::p1::infra::runtime
