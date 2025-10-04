/*
 * 🧱 File: LogManagerTestAccessor.h
 * ---------------------------------
 * 📌 Purpose   : Friend accessor for LogManager internals (used in tests/benchmarks).
 * 🧱 Layer     : Tests (Fakes / Accessors)
 * 👤 Author    : Tasqly Team
 * 🗓️ Created   : 2025-09-25
 * 🔖 Version   : 1.0 (Global accessor)
 * 🛡️ Stability : Stable (testing only, not production)
 *
 * 🧠 Description:
 * Exposes private internals of LogManager for controlled usage in:
 * - Unit tests (e.g., check default log path)
 * - Benchmarks (e.g., force rotation latency measurement)
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h
 * 🔗 Related   : test_log_manager_misc.cpp, Bench_LogRotation.cpp
 * ✅ Tested In : TasqlyTestsRunner, TasqlyBenchmarksRunner
 * ⚠️ Errors    : Not for production code
 * 🕒 Last Major Change: 2025-09-25
 */

#pragma once

#include <QMutexLocker>
#include "infra/logging/LogManager.h"
#include <functional>

/// 🧪 Global accessor (friend in LogManager.h)
class LogManagerTestAccessor
{
public:
  /// 📝 Force rotation immediately (bypass debounce/periodic timers)
  static void forceRotate(tasqly::infra::logging::LogManager& mgr)
  {
    QMutexLocker lock(&mgr.m_mutex);
    mgr.doRotate_locked();
  }

  /// 📝 Expose default log path for unit tests
  static QString defaultLogPath(tasqly::infra::logging::LogManager& mgr)
  {
    return mgr.defaultLogPath();
  }

  static bool isFileSinkEnabled(const tasqly::infra::logging::LogManager& mgr)
  {
    QMutexLocker lock(&mgr.m_mutex);
    return mgr.m_fileEnabled;
  }

  static bool isConsoleEnabled(const tasqly::infra::logging::LogManager& mgr)
  {
    QMutexLocker lock(&mgr.m_mutex);
    return mgr.m_consoleEnabled;
  }

  static tasqly::infra::logging::ILogFile* injectedFile(tasqly::infra::logging::LogManager& mgr)
  {
    QMutexLocker lock(&mgr.m_mutex);
    return mgr.m_file.get();
  }

  static void setFormatHook(tasqly::infra::logging::LogManager& mgr, std::function<void()> hook)
  {
    QMutexLocker lock(&mgr.m_mutex);
    mgr.m_onFormatLine = std::move(hook);
  }
};
