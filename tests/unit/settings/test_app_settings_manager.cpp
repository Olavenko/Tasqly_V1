/*
 * 🧱 File: test_app_settings_manager.cpp
 * --------------------------------------
 * 📌 Purpose   : Unit tests for AppSettingsManager (INI persistence, refactored).
 * 🧱 Layer     : Tests (Unit - Settings)
 * 👤 Author    : Tasqly Team
 * 🗓️ Created   : 2025-09-27
 * 🗓️ Updated   : 2025-10-02
 * 🔖 Version   : 5.0 (Refactored to use ISettingsStore mocks)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Comprehensive tests for AppSettingsManager covering:
 * - Default values & round-trip persistence
 * - Debounced save & periodic sync
 * - Invalid/corrupt INI handling
 * - Persistence errors & cooldown suppression
 * - Direct write() behavior
 * - Sync exceptions & status errors
 *
 * 🔗 Depends On:
 *   - app/settings/AppSettingsManager.h
 * ✅ Tested In : TasqlyTestsRunner
 */

#include "app/settings/AppSettingsManager.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QSettings>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTest>
#include <gtest/gtest.h>

// Standard includes
#include <QVector>
#include <memory>
#include <stdexcept>
#include <utility>

/// 🧪 Test double: QSettings-based store for testing
class TestIniSettingsStore : public ISettingsStore
{
public:
  explicit TestIniSettingsStore(QString path)
      : m_settings(std::move(path), QSettings::IniFormat)
  {
    m_settings.setFallbacksEnabled(false);
  }

  QVariant value(const QString& key, const QVariant& defaultValue) const override
  {
    return m_settings.value(key, defaultValue);
  }

  void setValue(const QString& key, const QVariant& value) override
  {
    m_settings.setValue(key, value);
  }

  void sync() override { m_settings.sync(); }

  QSettings::Status status() const override { return m_settings.status(); }

  QString fileName() const override { return m_settings.fileName(); }

private:
  mutable QSettings m_settings;
};

/// 🧪 Test double: Store that always throws on sync()
class ThrowingSettingsStore : public ISettingsStore
{
public:
  QVariant value(const QString&, const QVariant& defaultValue) const override
  {
    return m_value.isValid() ? m_value : defaultValue;
  }

  void setValue(const QString&, const QVariant& value) override { m_value = value; }

  void sync() override { throw std::runtime_error("Simulated sync failure"); }

  QSettings::Status status() const override { return QSettings::NoError; }

  QString fileName() const override { return QStringLiteral("throwing-store.ini"); }

private:
  QVariant m_value;
};

/// 🧪 Test double: Store with sequence of statuses
class StatusSequenceSettingsStore : public ISettingsStore
{
public:
  explicit StatusSequenceSettingsStore(QVector<QSettings::Status> statuses)
      : m_statuses(std::move(statuses))
  {}

  QVariant value(const QString&, const QVariant& defaultValue) const override
  {
    return m_value.isValid() ? m_value : defaultValue;
  }

  void setValue(const QString&, const QVariant& value) override { m_value = value; }

  void sync() override {}

  QSettings::Status status() const override
  {
    if (m_statusIndex < m_statuses.size()) {
      return m_statuses[m_statusIndex++];
    }
    return QSettings::NoError;
  }

  QString fileName() const override { return QStringLiteral("status-sequence-store.ini"); }

private:
  QVariant m_value;
  mutable QVector<QSettings::Status> m_statuses;
  mutable int m_statusIndex{0};
};

/// 🕵️ Test double: Store that records sync invocations
class SpySettingsStore : public ISettingsStore
{
public:
  QVariant value(const QString&, const QVariant& defaultValue) const override
  {
    return lastValue.isValid() ? lastValue : defaultValue;
  }

  void setValue(const QString&, const QVariant& value) override
  {
    lastValue = value;
    ++setValueCalls;
  }

  void sync() override { ++syncCalls; }

  QSettings::Status status() const override { return QSettings::NoError; }

  QString fileName() const override { return QStringLiteral("spy-store.ini"); }

  QVariant lastValue;
  int setValueCalls{0};
  int syncCalls{0};
};

/// 🧪 Test double: Store with fixed file path exposure
class FixedPathSettingsStore : public ISettingsStore
{
public:
  explicit FixedPathSettingsStore(QString path)
      : m_path(std::move(path))
  {}

  QVariant value(const QString&, const QVariant& defaultValue) const override
  {
    return defaultValue;
  }

  void setValue(const QString&, const QVariant&) override {}

  void sync() override {}

  QSettings::Status status() const override { return QSettings::NoError; }

  QString fileName() const override { return m_path; }

private:
  QString m_path;
};

/// 🧪 Test double: Store that throws when attempting to write defaults
class ThrowingOnDefaultsStore : public ISettingsStore
{
public:
  QVariant value(const QString&, const QVariant& defaultValue) const override
  {
    Q_UNUSED(defaultValue);
    return QVariant::fromValue(QStringLiteral("dark"));
  }

  void setValue(const QString&, const QVariant&) override
  {
    throw std::runtime_error("Simulated failure writing defaults");
  }

  void sync() override { throw std::runtime_error("Simulated sync failure for defaults"); }

  QSettings::Status status() const override
  {
    if (!m_statusReturned) {
      m_statusReturned = true;
      return QSettings::AccessError;
    }
    return QSettings::NoError;
  }

  QString fileName() const override { return QStringLiteral("throwing-defaults.ini"); }

private:
  mutable bool m_statusReturned{false};
};

/// 🎭 Subclass to expose protected write() for testing
class TestableAppSettingsManager : public AppSettingsManager
{
public:
  using AppSettingsManager::AppSettingsManager;
  using AppSettingsManager::write;

  void overrideSettingsPath(const QString& path)
  {
    m_settings = std::make_unique<TestIniSettingsStore>(path);
  }
};

/// 🧪 Fixture for AppSettingsManager
class AppSettingsManagerTest : public ::testing::Test
{
protected:
  QString iniPath;

  void SetUp() override
  {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configDir);
    iniPath = configDir + "/settings.ini";

    QFile::remove(iniPath);
  }

  void TearDown() override { QFile::remove(iniPath); }
};

// 📝 Test 1: Default theme is Light
TEST_F(AppSettingsManagerTest, DefaultThemeIsLight)
{
  AppSettingsManager mgr;
  mgr.load();
  EXPECT_EQ(mgr.themeMode(), ThemeMode::Light);
}

// 📝 Test 2: Setting same theme does not emit change
TEST_F(AppSettingsManagerTest, SettingSameThemeDoesNotEmitChange)
{
  AppSettingsManager mgr;
  QSignalSpy themeSpy(&mgr, &AppSettingsManager::themeModeChanged);

  mgr.setThemeMode(ThemeMode::Light);

  EXPECT_EQ(themeSpy.count(), 0);
}

// 📝 Test 3: Round-trip save/load works
TEST_F(AppSettingsManagerTest, RoundTripSaveLoad)
{
  {
    AppSettingsManager mgr;
    mgr.setThemeMode(ThemeMode::Dark);
    mgr.save(true);
  }
  {
    AppSettingsManager mgr2;
    mgr2.load();
    EXPECT_EQ(mgr2.themeMode(), ThemeMode::Dark);
  }
}

// 📝 Test 4: Invalid value resets to Light
TEST_F(AppSettingsManagerTest, InvalidIniResetsToDefault)
{
  QSettings s(iniPath, QSettings::IniFormat);
  s.setValue("themeMode", "foobar");
  s.sync();

  AppSettingsManager mgr;
  mgr.load();
  EXPECT_EQ(mgr.themeMode(), ThemeMode::Light);
}

// 📝 Test 5: Debounced save writes after delay
TEST_F(AppSettingsManagerTest, DebouncedSaveWritesAfterDelay)
{
  AppSettingsManager mgr;
  mgr.setThemeMode(ThemeMode::Dark);
  mgr.save();

  QTRY_COMPARE_WITH_TIMEOUT(QSettings(iniPath, QSettings::IniFormat).value("themeMode").toString(),
                            QString("dark"),
                            2000);
}

// 📝 Test 6: Settings file path mirrors backing store
TEST_F(AppSettingsManagerTest, SettingsFilePathMatchesStore)
{
  auto store = std::make_unique<FixedPathSettingsStore>(QStringLiteral("/tmp/custom-settings.ini"));
  const auto expectedPath = store->fileName();
  AppSettingsManager mgr(std::move(store));

  EXPECT_EQ(mgr.settingsFilePath(), expectedPath);
}

// 📝 Test 7: Forced save writes immediately
TEST_F(AppSettingsManagerTest, ForcedSaveWritesImmediately)
{
  AppSettingsManager mgr;
  mgr.setThemeMode(ThemeMode::Dark);
  QSignalSpy spy(&mgr, &AppSettingsManager::persistenceError);

  mgr.save(true);

  QSettings s(iniPath, QSettings::IniFormat);
  EXPECT_EQ(s.value("themeMode").toString(), "dark");
  EXPECT_EQ(spy.count(), 0);
}

// 📝 Test 8: Deferred save schedules flush via debounce timer
TEST_F(AppSettingsManagerTest, DeferredSaveSchedulesFlush)
{
  auto store = std::make_unique<SpySettingsStore>();
  auto* spyStore = store.get();
  AppSettingsManager mgr(std::move(store));

  mgr.setThemeMode(ThemeMode::Dark);
  mgr.save();

  EXPECT_EQ(spyStore->syncCalls, 0);
  EXPECT_EQ(spyStore->setValueCalls, 1);
  EXPECT_EQ(spyStore->lastValue.toString(), QStringLiteral("dark"));

  QTRY_VERIFY_WITH_TIMEOUT(spyStore->syncCalls >= 1, 2000);
}

// 📝 Test 9: Forced save bypasses debounce and syncs immediately
TEST_F(AppSettingsManagerTest, ForcedSaveBypassesDebounce)
{
  auto store = std::make_unique<SpySettingsStore>();
  auto* spyStore = store.get();
  AppSettingsManager mgr(std::move(store));

  mgr.setThemeMode(ThemeMode::Dark);
  mgr.save(true);

  EXPECT_EQ(spyStore->setValueCalls, 1);
  EXPECT_EQ(spyStore->syncCalls, 1);
  EXPECT_EQ(spyStore->lastValue.toString(), QStringLiteral("dark"));
}

// 📝 Test 10: Corrupt INI file resets to Light and emits settingsCorrupt
TEST_F(AppSettingsManagerTest, CorruptIniFileResetsToDefault)
{
  QFile f(iniPath);
  ASSERT_TRUE(f.open(QIODevice::WriteOnly));
  f.write("%%% BAD INI");
  f.close();

  AppSettingsManager mgr;
  QSignalSpy corruptSpy(&mgr, &AppSettingsManager::settingsCorrupt);

  mgr.load();
  EXPECT_EQ(mgr.themeMode(), ThemeMode::Light);
  EXPECT_GE(corruptSpy.count(), 1);
}

// 📝 Test 11: PersistenceError triggered on read-only INI
TEST_F(AppSettingsManagerTest, PersistenceErrorOnReadOnlyFile)
{
  QTemporaryDir tmpDir;
  ASSERT_TRUE(tmpDir.isValid());

  QString roPath = tmpDir.path() + "/readonly.ini";
  QFile f(roPath);
  ASSERT_TRUE(f.open(QIODevice::WriteOnly));
  f.write("themeMode=light");
  f.close();
  f.setPermissions(QFileDevice::ReadOwner | QFileDevice::ReadUser);

  TestableAppSettingsManager mgr;
  mgr.overrideSettingsPath(roPath);
  QSignalSpy spy(&mgr, &AppSettingsManager::persistenceError);

  mgr.setThemeMode(ThemeMode::Dark);
  mgr.save(true);

  EXPECT_GE(spy.count(), 1);
}

// 📝 Test 12: Error cooldown suppression works
TEST_F(AppSettingsManagerTest, ErrorCooldownSuppression)
{
  QTemporaryDir tmpDir;
  ASSERT_TRUE(tmpDir.isValid());

  QString roPath = tmpDir.path() + "/readonly2.ini";
  QFile f(roPath);
  ASSERT_TRUE(f.open(QIODevice::WriteOnly));
  f.write("themeMode=light");
  f.close();
  f.setPermissions(QFileDevice::ReadOwner | QFileDevice::ReadUser);

  TestableAppSettingsManager mgr;
  mgr.overrideSettingsPath(roPath);
  QSignalSpy spy(&mgr, &AppSettingsManager::persistenceError);

  mgr.save(true);
  int firstCount = spy.count();

  mgr.save(true); // suppressed by cooldown
  EXPECT_EQ(spy.count(), firstCount);

  QTest::qWait(3100);
  mgr.save();
  QTRY_VERIFY_WITH_TIMEOUT(spy.count() >= firstCount + 1, 2000);
}

// 📝 Test 13: write() buffers value and periodic sync persists it
TEST_F(AppSettingsManagerTest, WriteBuffersAndPeriodicSyncPersists)
{
  TestableAppSettingsManager mgr;
  mgr.setThemeMode(ThemeMode::Dark);
  mgr.write();

  QSettings s1(iniPath, QSettings::IniFormat);
  EXPECT_EQ(s1.value("themeMode").toString(), "dark");

  QTest::qWait(5500);
  QSettings s2(iniPath, QSettings::IniFormat);
  EXPECT_EQ(s2.value("themeMode").toString(), "dark");
}

// 📝 Test 14: Destructor flushes settings to disk
TEST_F(AppSettingsManagerTest, DestructorFlushesFinalSync)
{
  {
    TestableAppSettingsManager mgr;
    mgr.setThemeMode(ThemeMode::Dark);
    mgr.write();
  }
  QSettings s(iniPath, QSettings::IniFormat);
  EXPECT_EQ(s.value("themeMode").toString(), "dark");
}

// 📝 Test 15: Save emits persistenceError when sync throws
TEST_F(AppSettingsManagerTest, SaveEmitsPersistenceErrorWhenSyncThrows)
{
  auto store = std::make_unique<ThrowingSettingsStore>();
  AppSettingsManager mgr(std::move(store));
  QSignalSpy spy(&mgr, &AppSettingsManager::persistenceError);

  mgr.setThemeMode(ThemeMode::Dark);
  mgr.save(true);

  EXPECT_GE(spy.count(), 1);
}

// 📝 Test 16: Debounced sync surfaces errors and enforces cooldown
TEST_F(AppSettingsManagerTest, DebouncedSyncEmitsErrorAndHonorsCooldown)
{
  auto store = std::make_unique<ThrowingSettingsStore>();
  AppSettingsManager mgr(std::move(store));
  QSignalSpy spy(&mgr, &AppSettingsManager::persistenceError);

  mgr.setThemeMode(ThemeMode::Dark);
  QTRY_VERIFY_WITH_TIMEOUT(spy.count() >= 1, 2000);
  int initialCount = spy.count();

  mgr.setThemeMode(ThemeMode::Light);
  QTest::qWait(400);
  EXPECT_EQ(spy.count(), initialCount);

  QTest::qWait(3200);
  mgr.setThemeMode(ThemeMode::Dark);
  QTRY_VERIFY_WITH_TIMEOUT(spy.count() >= initialCount + 1, 2000);
}

// 📝 Test 17: Load emits settingsCorrupt and persistenceError on status errors
TEST_F(AppSettingsManagerTest, LoadSignalsCorruptionWhenStatusIsBad)
{
  auto store = std::make_unique<StatusSequenceSettingsStore>(
      QVector<QSettings::Status>{QSettings::FormatError, QSettings::AccessError});
  AppSettingsManager mgr(std::move(store));
  QSignalSpy corruptSpy(&mgr, &AppSettingsManager::settingsCorrupt);
  QSignalSpy errorSpy(&mgr, &AppSettingsManager::persistenceError);

  mgr.load();

  EXPECT_EQ(mgr.themeMode(), ThemeMode::Light);
  EXPECT_GE(corruptSpy.count(), 1);
  EXPECT_GE(errorSpy.count(), 1);
}

// 📝 Test 18: write() reports persistenceError when status is non-zero
TEST_F(AppSettingsManagerTest, WriteEmitsPersistenceErrorWhenStatusIsBad)
{
  auto store = std::make_unique<StatusSequenceSettingsStore>(
      QVector<QSettings::Status>{QSettings::AccessError});
  TestableAppSettingsManager mgr(std::move(store));
  QSignalSpy spy(&mgr, &AppSettingsManager::persistenceError);

  mgr.setThemeMode(ThemeMode::Dark);
  mgr.write();

  EXPECT_GE(spy.count(), 1);
}

// 📝 Test 19: load() surfaces persistenceError when defaults cannot be written
TEST_F(AppSettingsManagerTest, LoadEmitsErrorWhenWritingDefaultsFails)
{
  auto store = std::make_unique<ThrowingOnDefaultsStore>();
  AppSettingsManager mgr(std::move(store));
  QSignalSpy corruptSpy(&mgr, &AppSettingsManager::settingsCorrupt);
  QSignalSpy errorSpy(&mgr, &AppSettingsManager::persistenceError);

  mgr.load();

  EXPECT_EQ(mgr.themeMode(), ThemeMode::Light);
  EXPECT_GE(corruptSpy.count(), 1);
  EXPECT_GE(errorSpy.count(), 1);
}
