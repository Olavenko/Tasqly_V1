/*
 * 🧪 File: test_error_reporter.cpp
 * --------------------------------
 * 📌 Purpose   : Integration tests for ErrorReporter → LogManager + Notifier.
 * 🧱 Layer     : Tests (Integration / Logging)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-15
 *
 * 🧠 Description:
 * Ensures ErrorReporter:
 * - Logs errors via ILogManager.
 * - Emits toastRequested() for UI/Notifier.
 *
 * 🔗 Depends On:
 *   - ErrorReporter.h / .cpp
 *   - LogManager.h / .cpp
 *   - Notifier.h / .cpp
 *   - Error.h
 * ✅ Tested In : TasqlyTestsRunner (GoogleTest + Qt Test)
 */

#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QSignalSpy>

#include "app/logging/ErrorReporter.h"
#include "domain/core/Error.h"

using namespace tasqly::app::logging;
using namespace tasqly::domain::core;

// 🎭 Fake LogManager to spy calls
class SpyLogManager : public ILogManager
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
  Level currentLevel = Level::Info;

  bool isEnabled(Level level) const override
  {
    return static_cast<int>(level) >= static_cast<int>(currentLevel);
  }

  void log(Level lvl, const QString& cat, const QString& msg, const QVariantMap& ctx = {}) override
  {
    calls.push_back({lvl, cat, msg, ctx});
  }

  // Unused in tests
  void setLevel(Level) override {}
  Level level() const override { return currentLevel; }
  void enableConsole(bool) override {}
  void enableFileSink(bool, const QString&) override {}
  void setRotationPolicy(const RotationPolicy&) override {}
  void rotateNow() override {}
  void flush() override {}
  void addMaskedKey(const QString&) override {}
  void setMaskedKeys(const QStringList&) override {}
  QStringList maskedKeys() const override { return {}; }
};

// 🧪 Test Fixture
class ErrorReporterTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    spy = std::make_shared<SpyLogManager>();
    reporter = new ErrorReporter(spy);
  }
  void TearDown() override { delete reporter; }
  std::shared_ptr<SpyLogManager> spy;
  ErrorReporter* reporter;
};

// ✅ Test: logs error via LogManager
TEST_F(ErrorReporterTest, ReportsErrorToLogManager)
{
  Error err(42, "Something failed", {{"key", "val"}});
  reporter->report(err, Severity::Error, "testcat");

  ASSERT_EQ(spy->calls.size(), 1u);
  EXPECT_EQ(spy->calls[0].msg, "Something failed");
  EXPECT_EQ(spy->calls[0].category, "testcat");
  EXPECT_EQ(spy->calls[0].ctx.value("key").toString(), "val");
  EXPECT_EQ(static_cast<int>(spy->calls[0].lvl), static_cast<int>(Level::Error));
}

// ✅ Test: emits toastRequested signal
TEST_F(ErrorReporterTest, EmitsToastRequestedSignal)
{
  QSignalSpy spySignal(reporter, &ErrorReporter::toastRequested);
  Error err(99, "Show me toast");

  reporter->report(err, Severity::Warning, "test");

  ASSERT_EQ(spySignal.count(), 1);
  QList<QVariant> args = spySignal.takeFirst();
  EXPECT_EQ(args[0].toInt(), static_cast<int>(Severity::Warning));
  EXPECT_EQ(args[1].toString(), "Show me toast");
}
