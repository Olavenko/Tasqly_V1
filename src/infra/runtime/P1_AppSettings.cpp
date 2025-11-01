/*
 * 🧱 File: P1_AppSettings.cpp
 * ---------------------------
 * 📌 Purpose   : Implementation of P1_AppSettings — environment-based config reader (Phase 1).
 * 🧱 Layer     : Infrastructure (Runtime)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 1.0
 *
 * 🧠 Description:
 * Provides a unified environment configuration system for Tasqly runtime subsystems.
 * Reads and caches environment variables in a thread-safe way.
 * Integrates with P1_Logger to log key reads and overrides.
 *
 * 🔗 Related:
 *   - P1_AppSettings.h
 *   - P1_Logger
 */

#include "P1_AppSettings.h"
#include "P1_Logger.h"

#include <algorithm>
#include <cctype>
#include <iostream>

namespace tasqly::p1::infra::runtime {

// ⚙️ Singleton accessor
P1_AppSettings& P1_AppSettings::instance()
{
  static P1_AppSettings settings;
  return settings;
}

// 🧱 Helper: Fetch environment or cache
std::optional<std::string> P1_AppSettings::fetch(const std::string& key) const
{
  std::lock_guard<std::mutex> lock(m_mutex);

  // 🧱 Check cache first
  if (auto it = m_cache.find(key); it != m_cache.end()) {
    return it->second;
  }

  // 🧩 Read from environment (cross-platform)
  std::string value;

#if defined(_MSC_VER)
  // ✅ MSVC (Visual Studio) — use _dupenv_s safely
  char* buffer = nullptr;
  size_t len = 0;
  if (_dupenv_s(&buffer, &len, key.c_str()) == 0 && buffer != nullptr) {
    value = buffer;
    free(buffer);
  } else {
    return std::nullopt;
  }
#else
  // ✅ MinGW / GCC / Clang — use portable getenv()
  const char* raw = std::getenv(key.c_str());
  if (!raw)
    return std::nullopt;
  value = raw;
#endif

  // 🧱 Cache the result
  m_cache[key] = value;

  // 🪵 Log successful read
  P1_Logger::instance().info("[AppSettings] Read " + key + "=" + value);

  return value;
}

// 🧩 Public: Get string
std::optional<std::string> P1_AppSettings::getString(const std::string& key) const
{
  return fetch(key);
}

// 🧩 Public: Get boolean
bool P1_AppSettings::getBool(const std::string& key, bool defaultValue) const
{
  auto val = fetch(key);
  if (!val.has_value())
    return defaultValue;

  std::string lower = *val;
  std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
    return std::tolower(c);
  });

  if (lower == "1" || lower == "true" || lower == "yes" || lower == "on")
    return true;
  if (lower == "0" || lower == "false" || lower == "no" || lower == "off")
    return false;

  return defaultValue;
}

// 🧩 Public: Get integer
int P1_AppSettings::getInt(const std::string& key, int defaultValue) const
{
  auto val = fetch(key);
  if (!val.has_value())
    return defaultValue;

  try {
    return std::stoi(*val);
  } catch (...) {
    P1_Logger::instance().warn("[AppSettings] Invalid integer for key: " + key);
    return defaultValue;
  }
}

// 🧩 Public: Set or override value
void P1_AppSettings::set(const std::string& key, const std::string& value)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_cache[key] = value;

  P1_Logger::instance().info("[AppSettings] Override " + key + "=" + value);
}

// 🧩 Public: Print summary of loaded keys
void P1_AppSettings::printSummary() const
{
  std::lock_guard<std::mutex> lock(m_mutex);

  if (m_cache.empty()) {
    P1_Logger::instance().info("[AppSettings] No cached settings.");
    return;
  }

  P1_Logger::instance().info("[AppSettings] Cached environment variables:");
  for (const auto& [k, v] : m_cache) {
    std::cout << "  - " << k << " = " << v << std::endl;
  }
}

// 🧩 Feature: Fallback to InMemory when DB fails
bool P1_AppSettings::isFeatureFallbackEnabled() const
{
  // 📝 Default → true (fail-safe for runtime resilience)
  return getBool("FEATURES_DB_FALLBACK_INMEMORY", true);
}

// 🧩 Feature: Seed demo data in InMemory repo
bool P1_AppSettings::isFeatureSeedEnabled() const
{
  // 📝 Default → false (only for dev/test parity)
  return getBool("FEATURES_INMEMORY_SEED", false);
}

} // namespace tasqly::p1::infra::runtime
