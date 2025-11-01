#pragma once
/*
 * 🧱 File: P1_AppSettings.h
 * -------------------------
 * 📌 Purpose   : Lightweight, environment-based configuration reader for runtime subsystems.
 * 🧱 Layer     : Infrastructure (Runtime)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-10-21
 * 🔖 Version   : 1.0 (Initial)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Provides a pure C++ settings manager that reads environment variables safely and
 * caches them for runtime usage. It replaces the legacy AppSettingsManager from Phase 0.
 *
 * Supports typed accessors, default values, and feature flags such as:
 *  - DB credentials and connection strings
 *  - Feature toggles (e.g., `features.db.fallback_inmemory`)
 *  - Logging and Notifier configurations
 *
 * ⚙️ Behavior:
 *  - Reads once on demand (lazy-loaded)
 *  - Caches results for repeated queries
 *  - Thread-safe via mutex
 *  - Does NOT depend on Qt or any external library
 *
 * 🔗 Depends On:
 *   - <unordered_map>, <mutex>, <string>, <optional>, <cstdlib>
 *
 * 🔗 Related:
 *   - P1_Logger (logs configuration reads)
 *   - P1_Notifier (for runtime feedback)
 *   - P1_S2_TaskRepositoryFactory (uses DB env vars)
 */

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

namespace tasqly::p1::infra::runtime {

// 🧱 Phase 1 App Settings Manager — Environment-based
class P1_AppSettings final
{
public:
  // 🔹 Singleton accessor
  static P1_AppSettings& instance();

  // 🔹 Fetch string setting
  std::optional<std::string> getString(const std::string& key) const;

  // 🔹 Fetch boolean setting (supports "1", "true", "yes")
  bool getBool(const std::string& key, bool defaultValue = false) const;

  // 🔹 Fetch integer setting
  int getInt(const std::string& key, int defaultValue = 0) const;

  // 🔹 Set value manually (for tests or overrides)
  void set(const std::string& key, const std::string& value);

  // 🔹 Print summary (for diagnostics)
  void printSummary() const;

  // 👉 Feature Flags
  // 🧩 Fallback to InMemory repository if PostgreSQL init fails
  bool isFeatureFallbackEnabled() const;

  // 🧩 Seed deterministic demo data into InMemory repository
  bool isFeatureSeedEnabled() const;

private:
  // 🧱 Constructor
  P1_AppSettings() = default;

  // 🧱 Helper to fetch from cache or environment
  std::optional<std::string> fetch(const std::string& key) const;

  // 🔒 State
  mutable std::mutex m_mutex;
  mutable std::unordered_map<std::string, std::string> m_cache;
};

} // namespace tasqly::p1::infra::runtime
