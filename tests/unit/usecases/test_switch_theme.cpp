#include "app/settings/AppSettingsManager.h"
#include "app/settings/FeatureFlagsManager.h"
#include "app/theme/ThemeManager.h"
#include "app/usecases/SwitchTheme.h"

#include <gtest/gtest.h>

#include <QHash>
#include <QString>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

#include "infra/logging/LogManager.h"
#include "tests/fakes/LogManagerTestAccessor.h"

namespace {

class InMemorySettingsStore : public ISettingsStore
{
public:
  QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const override
  {
    return m_values.value(key, defaultValue);
  }

  void setValue(const QString& key, const QVariant& value) override { m_values.insert(key, value); }

  void sync() override {}

  QSettings::Status status() const override { return QSettings::NoError; }

  QString fileName() const override { return QStringLiteral("in-memory"); }

private:
  QHash<QString, QVariant> m_values;
};

class TrackingSettingsManager : public AppSettingsManager
{
public:
  TrackingSettingsManager()
      : AppSettingsManager(std::make_unique<InMemorySettingsStore>())
  {}

  ThemeMode themeMode() const override { return m_themeMode; }

  void setThemeMode(ThemeMode mode) override
  {
    ++setThemeModeCalls;
    m_themeMode = mode;
  }

  void save(bool force = false) override
  {
    ++saveCalls;
    AppSettingsManager::save(force);
  }

  void resetCounters()
  {
    setThemeModeCalls = 0;
    saveCalls = 0;
  }

  int setThemeModeCalls{0};
  int saveCalls{0};
};

class ScopedLogHook
{
public:
  ScopedLogHook(tasqly::infra::logging::LogManager& log, std::function<void()> hook)
      : m_log(log)
  {
    LogManagerTestAccessor::setFormatHook(m_log, std::move(hook));
  }

  ~ScopedLogHook() { LogManagerTestAccessor::setFormatHook(m_log, {}); }

private:
  tasqly::infra::logging::LogManager& m_log;
};

class FeatureFlagGuard
{
public:
  FeatureFlagGuard(QString key, bool enabled)
      : m_key(std::move(key))
  {
    auto& features = FeatureFlagsManager::instance();
    m_previous = features.isEnabled(m_key);
    features.setFeature(m_key, enabled);
  }

  ~FeatureFlagGuard() { FeatureFlagsManager::instance().setFeature(m_key, m_previous); }

private:
  QString m_key;
  bool m_previous{false};
};

class ScopedStdStreamCapture
{
public:
  ScopedStdStreamCapture()
  {
    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
  }

  ~ScopedStdStreamCapture()
  {
    testing::internal::GetCapturedStdout();
    testing::internal::GetCapturedStderr();
  }
};

class ThrowingSettingsManager : public TrackingSettingsManager
{
public:
  void save(bool = false) override
  {
    ++saveCalls;
    throw std::runtime_error("disk write failed");
  }
};

} // namespace

TEST(SwitchThemeUseCaseTest, PerformsTransitionWhenModeDiffers)
{
  TrackingSettingsManager settings;
  ThemeManager theme;
  SwitchTheme usecase(&settings, &theme, nullptr);

  usecase.execute(ThemeMode::Dark);

  EXPECT_EQ(theme.mode(), ThemeMode::Dark);
  EXPECT_EQ(settings.themeMode(), ThemeMode::Dark);
  EXPECT_EQ(settings.setThemeModeCalls, 1);
  EXPECT_EQ(settings.saveCalls, 1);
}

TEST(SwitchThemeUseCaseTest, SkipsWorkWhenRuntimeAndPersistedAlreadyMatch)
{
  TrackingSettingsManager settings;
  ThemeManager theme;
  SwitchTheme usecase(&settings, &theme, nullptr);

  usecase.execute(ThemeMode::Dark);
  settings.resetCounters();
  theme.switchTo(ThemeMode::Dark);

  usecase.execute(ThemeMode::Dark);

  EXPECT_EQ(theme.mode(), ThemeMode::Dark);
  EXPECT_EQ(settings.themeMode(), ThemeMode::Dark);
  EXPECT_EQ(settings.setThemeModeCalls, 0);
  EXPECT_EQ(settings.saveCalls, 0);
}

TEST(SwitchThemeUseCaseTest, PersistsWhenOnlyRuntimeMatches)
{
  TrackingSettingsManager settings;
  ThemeManager theme;
  SwitchTheme usecase(&settings, &theme, nullptr);

  settings.setThemeMode(ThemeMode::Dark);
  settings.resetCounters();

  usecase.execute(ThemeMode::Light);

  EXPECT_EQ(theme.mode(), ThemeMode::Light);
  EXPECT_EQ(settings.themeMode(), ThemeMode::Light);
  EXPECT_EQ(settings.setThemeModeCalls, 1);
  EXPECT_EQ(settings.saveCalls, 1);
}

TEST(SwitchThemeUseCaseTest, LogsInformationWhenThemeSwitches)
{
  TrackingSettingsManager settings;
  ThemeManager theme;
  FeatureFlagGuard loggingGuard(QStringLiteral("features.logging"), true);
  tasqly::infra::logging::LogManager logManager;
  ScopedStdStreamCapture capture;

  int logCount = 0;
  ScopedLogHook hook(logManager, [&]() { ++logCount; });

  SwitchTheme usecase(&settings, &theme, &logManager);
  usecase.execute(ThemeMode::Dark);

  EXPECT_EQ(logCount, 1);
  EXPECT_EQ(theme.mode(), ThemeMode::Dark);
  EXPECT_EQ(settings.themeMode(), ThemeMode::Dark);
}

TEST(SwitchThemeUseCaseTest, LogsErrorWhenPersistenceThrows)
{
  ThrowingSettingsManager settings;
  ThemeManager theme;
  FeatureFlagGuard loggingGuard(QStringLiteral("features.logging"), true);
  tasqly::infra::logging::LogManager logManager;
  ScopedStdStreamCapture capture;

  int logCount = 0;
  ScopedLogHook hook(logManager, [&]() { ++logCount; });

  SwitchTheme usecase(&settings, &theme, &logManager);

  EXPECT_NO_THROW(usecase.execute(ThemeMode::Dark));

  EXPECT_EQ(logCount, 1);
  EXPECT_EQ(theme.mode(), ThemeMode::Dark);
  EXPECT_EQ(settings.themeMode(), ThemeMode::Dark);
  EXPECT_EQ(settings.setThemeModeCalls, 1);
  EXPECT_EQ(settings.saveCalls, 1);
}
