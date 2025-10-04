/*
 * 🧱 File: test_error_reporter_unit.cpp
 * -------------------------------------
 * 📌 Purpose   : Unit tests for ErrorReporter — cover edge cases not in integration tests.
 * 🧱 Layer     : Tests (Unit - Infrastructure / Logging)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-23
 * 🗓️ Updated   : 2025-09-25
 * 🔖 Version   : 2.0 (Aligned with C++ Standards)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * - Ensures disabled logging path still emits toast.
 * - Covers reportRaw normalization (empty message → "Unknown error").
 * - Validates mapSeverityToLevel for all severities.
 * - Tests merge helper (last-writer-wins).
 *
 * 🔗 Depends On:
 *   - app/logging/ErrorReporter.h
 *   - domain/core/Error.h
 * 🔗 Related   : integration/logging/test_error_reporter_integration.cpp
 * ✅ Tested In : TasqlyTestsRunner
 * ⚠️ Errors    : Fails if ErrorReporter fails to emit signals or log normalization is broken
 * 🕒 Last Major Change: 2025-09-25
 */

#include <gtest/gtest.h>

// Qt
#include <QCoreApplication>
#include <QSignalSpy>
#include <QVariantMap>

// Project
#include "app/logging/ErrorReporter.h"
#include "domain/core/Error.h"

using tasqly::app::logging::ErrorReporter;
using tasqly::app::logging::ILogManager;
using tasqly::app::logging::Level;
using tasqly::app::logging::Severity;
using tasqly::domain::core::Error;

/// 🎭 Dummy LogManager: spy implementation for unit testing
class DummyLogManager : public ILogManager
{
public:
  struct Call
  {
    Level lvl;
    QString category;
    QString msg;
    QVariantMap ctx;
  };

  std::vector<Call> calls;
  bool enabled = true;

  bool isEnabled(Level) const override { return enabled; }

  void log(Level lvl, const QString& cat, const QString& msg, const QVariantMap& ctx = {}) override
  {
    calls.push_back({lvl, cat, msg, ctx});
  }

  // Unused API (no-op implementations for tests)
  void setLevel(Level) override {}
  Level level() const override { return Level::Info; }
  void enableConsole(bool) override {}
  void enableFileSink(bool, const QString&) override {}
  void setRotationPolicy(const tasqly::app::logging::RotationPolicy&) override {}
  void rotateNow() override {}
  void flush() override {}
  void addMaskedKey(const QString&) override {}
  void setMaskedKeys(const QStringList&) override {}
  QStringList maskedKeys() const override { return {}; }
};

/// 🧪 Fixture for ErrorReporter unit tests
class ErrorReporterUnitTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    spy = std::make_shared<DummyLogManager>();
    reporter = new ErrorReporter(spy);
  }

  void TearDown() override { delete reporter; }

  std::shared_ptr<DummyLogManager> spy;
  ErrorReporter* reporter{};
};

// 🧪 Test 1: when logging disabled → toast still emitted, no new log
TEST_F(ErrorReporterUnitTest, DisabledLoggingStillEmitsToast)
{
  reporter->report(Error(1, "X"), Severity::Info, "cat");
  EXPECT_EQ(spy->calls.size(), 1u);

  spy->enabled = false; // disable logging
  QSignalSpy sig(reporter, &ErrorReporter::toastRequested);

  reporter->report(Error(2, "Y"), Severity::Critical, "cat2");

  EXPECT_EQ(sig.count(), 1);
  EXPECT_EQ(spy->calls.size(), 1u) << "No new log should be written when disabled";
}

// 🧪 Test 2: reportRaw normalizes empty message
TEST_F(ErrorReporterUnitTest, ReportRawNormalizesEmptyMessage)
{
  QSignalSpy sig(reporter, &ErrorReporter::toastRequested);
  reporter->reportRaw(100, "", Severity::Error, "cat");

  ASSERT_EQ(sig.count(), 1);
  QList<QVariant> args = sig.takeFirst();
  EXPECT_EQ(args[1].toString(), "Unknown error");
}

// 🧪 Test 3: mapSeverityToLevel covers all cases
TEST(ErrorReporterStatic, MapSeverityToLevel_AllVariants)
{
  EXPECT_EQ(ErrorReporter::mapSeverityToLevel(Severity::Info), Level::Info);
  EXPECT_EQ(ErrorReporter::mapSeverityToLevel(Severity::Warning), Level::Warn);
  EXPECT_EQ(ErrorReporter::mapSeverityToLevel(Severity::Error), Level::Error);
  EXPECT_EQ(ErrorReporter::mapSeverityToLevel(Severity::Critical), Level::Critical);
}

// 🧪 Test 4: merge gives priority to extraCtx (last-writer-wins)
TEST(ErrorReporterStatic, MergeContextLastWriterWins)
{
  QVariantMap base{{"k1", "v1"}, {"common", "old"}};
  QVariantMap extra{{"k2", "v2"}, {"common", "new"}};

  QVariantMap merged = ErrorReporter::merge(base, extra);
  EXPECT_EQ(merged.value("k1").toString(), "v1");
  EXPECT_EQ(merged.value("k2").toString(), "v2");
  EXPECT_EQ(merged.value("common").toString(), "new");
}
