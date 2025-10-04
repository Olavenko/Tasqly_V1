/*
 * 🧱 File: test_save_user_settings.cpp
 * -----------------------------------
 * 📌 Purpose   : Unit tests for SaveUserSettings use case (success + failure).
 * 🧱 Layer     : Tests (Unit / UseCases)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-15
 * 🔖 Version   : 1.1 (Updated to use real ThemeManager)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Verifies SaveUserSettings behavior:
 * - Success: Theme mode persisted + Info log.
 * - Failure: save() throws → handled + Error log.
 *
 * 🔗 Depends On:
 *   - SaveUserSettings.h
 *   - ThemeManager.h
 *   - GoogleTest
 * ✅ Tested In : TasqlyTestsRunner
 */

#include "app/logging/ILogManager.h"
#include "app/theme/ThemeManager.h"
#include "app/usecases/SaveUserSettings.h"

#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

// 🎭 Fake AppSettingsManager (captures calls)
class FakeAppSettingsManager : public AppSettingsManager
{
public:
  FakeAppSettingsManager(QObject* parent = nullptr)
      : AppSettingsManager(parent)
  {}

  void setThemeMode(ThemeMode mode) override { recordedMode = mode; }

  // 📝 Save with force flag support
  void save(bool force) override
  {
    lastForce = force;
    if (failOnSave) {
      throw std::runtime_error("DB unavailable");
    }
    saved = true;
  }

  bool saved{false};
  bool failOnSave{false};
  ThemeMode recordedMode{ThemeMode::Light};
  bool lastForce{false};
};

// 🎭 Fake ILogManager
class FakeLogManager : public tasqly::app::logging::ILogManager
{
public:
  using Level = tasqly::app::logging::Level;
  using RotationPolicy = tasqly::app::logging::RotationPolicy;

  void setLevel(Level) override {}
  Level level() const override { return Level::Info; }
  bool isEnabled(Level) const override { return true; }

  void enableConsole(bool) override {}
  void enableFileSink(bool, const QString& = QString()) override {}
  void setRotationPolicy(const RotationPolicy&) override {}
  void rotateNow() override {}
  void flush() override {}

  void addMaskedKey(const QString&) override {}
  void setMaskedKeys(const QStringList&) override {}
  QStringList maskedKeys() const override { return {}; }

  void log(Level level,
           const QString& category,
           const QString& message,
           const QVariantMap& = {}) override
  {
    logs.push_back({level, category, message});
  }

  struct Entry
  {
    Level level;
    QString category;
    QString message;
  };
  std::vector<Entry> logs;
};

// ✅ Test: Success case
TEST(SaveUserSettingsTest, SuccessCase)
{
  FakeAppSettingsManager fakeSettings;
  ThemeManager realTheme;
  FakeLogManager fakeLog;

  realTheme.switchTo(ThemeMode::Dark);

  SaveUserSettings uc(&fakeSettings, &realTheme, &fakeLog);
  uc.execute();

  EXPECT_TRUE(fakeSettings.saved);
  EXPECT_EQ(fakeSettings.recordedMode, ThemeMode::Dark);
  EXPECT_FALSE(fakeSettings.lastForce);

  ASSERT_FALSE(fakeLog.logs.empty());
  EXPECT_EQ(fakeLog.logs.back().level, tasqly::app::logging::Level::Info);
  EXPECT_TRUE(fakeLog.logs.back().message.contains("saved"));
}

// ✅ Test: Failure case
TEST(SaveUserSettingsTest, FailureCase)
{
  FakeAppSettingsManager fakeSettings;
  ThemeManager realTheme;
  FakeLogManager fakeLog;

  fakeSettings.failOnSave = true;

  SaveUserSettings uc(&fakeSettings, &realTheme, &fakeLog);
  uc.execute();

  EXPECT_FALSE(fakeSettings.saved);

  ASSERT_FALSE(fakeLog.logs.empty());
  EXPECT_EQ(fakeLog.logs.back().level, tasqly::app::logging::Level::Error);
  EXPECT_TRUE(fakeLog.logs.back().message.contains("Failed to save"));
}

// ✅ Test: Forced save propagates force flag
TEST(SaveUserSettingsTest, ForceSaveRequestsBlockingFlush)
{
  FakeAppSettingsManager fakeSettings;
  ThemeManager realTheme;
  FakeLogManager fakeLog;

  SaveUserSettings uc(&fakeSettings, &realTheme, &fakeLog);
  uc.execute(true);

  EXPECT_TRUE(fakeSettings.saved);
  EXPECT_TRUE(fakeSettings.lastForce);
}
