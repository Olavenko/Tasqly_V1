/*
 * 🧱 File: test_log_facade.cpp
 * ----------------------------
 * 📌 Purpose   : Unit tests for LogFacade (QML-facing logging).
 * 🧱 Layer     : Testing (Unit)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-14
 *
 * 🧠 Description:
 * Verifies that LogFacade correctly forwards log messages
 * to the underlying LogManager implementation.
 *
 * 🔗 Depends On:
 *   - app/logging/LogFacade.h
 *   - app/logging/ILogManager.h
 * 🔗 Related   : LogManager (infra), FeatureFlagsManager
 * ✅ Tested In : TasqlyTestsRunner (GoogleTest)
 * 🕒 Last Major Change: 2025-09-14
 */

#include "app/logging/ILogManager.h"
#include "app/logging/LogFacade.h"

#include <gtest/gtest.h>

using tasqly::app::logging::Level;

// 👉 Fake ILogManager to capture log calls
class FakeLogManager : public tasqly::app::logging::ILogManager
{
public:
  struct Entry
  {
    Level level;
    QString category;
    QString message;
  };

  std::vector<Entry> entries;

  // 🚀 Capture log calls into entries
  void log(Level level,
           const QString& category,
           const QString& message,
           const QVariantMap& context = {}) override
  {
    entries.push_back({level, category, message});
    Q_UNUSED(context);
  }

  // 👉 Stub implementations for interface
  void setLevel(Level) override {}
  Level level() const override { return Level::Info; }
  bool isEnabled(Level) const override { return true; }

  void enableConsole(bool) override {}
  void enableFileSink(bool, const QString& = QString()) override {}
  void setRotationPolicy(const tasqly::app::logging::RotationPolicy&) override {}
  void rotateNow() override {}
  void flush() override {}

  void addMaskedKey(const QString&) override {}
  void setMaskedKeys(const QStringList&) override {}
  QStringList maskedKeys() const override { return {}; }
};

// 👉 Test fixture
class LogFacadeTest : public ::testing::Test
{
protected:
  FakeLogManager fake;
  LogFacade facade{&fake};
};

// ✅ Info message is forwarded
TEST_F(LogFacadeTest, InfoMessageForwarded)
{
  facade.info("hello");
  ASSERT_EQ(fake.entries.size(), 1);
  EXPECT_EQ(fake.entries[0].level, Level::Info);
  EXPECT_EQ(fake.entries[0].category.toStdString(), "QML");
  EXPECT_EQ(fake.entries[0].message.toStdString(), "hello");
}

// ⚠️ Warning message is forwarded
TEST_F(LogFacadeTest, WarnMessageForwarded)
{
  facade.warn("be careful");
  ASSERT_EQ(fake.entries.size(), 1);
  EXPECT_EQ(fake.entries[0].level, Level::Warn);
  EXPECT_EQ(fake.entries[0].category.toStdString(), "QML");
  EXPECT_EQ(fake.entries[0].message.toStdString(), "be careful");
}

// ❌ Error message is forwarded
TEST_F(LogFacadeTest, ErrorMessageForwarded)
{
  facade.error("boom");
  ASSERT_EQ(fake.entries.size(), 1);
  EXPECT_EQ(fake.entries[0].level, Level::Error);
  EXPECT_EQ(fake.entries[0].category.toStdString(), "QML");
  EXPECT_EQ(fake.entries[0].message.toStdString(), "boom");
}
