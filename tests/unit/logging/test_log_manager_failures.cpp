/*
 * 🧱 File: test_log_manager_failures.cpp
 * --------------------------------------
 * 📌 Purpose   : Verify LogManager error handling when injected log file stubs fail.
 * 🧱 Layer     : Tests (Unit - Logging)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-10-05
 * 🔖 Version   : 1.0 (Injectable file stub coverage)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Validates that LogManager gracefully falls back to console logging when the
 * injected file object fails during write, flush, or rotation operations.
 */

#include <gtest/gtest.h>

#include "infra/logging/LogManager.h"
#include "tests/fakes/LogManagerTestAccessor.h"

#include <QFileInfo>
#include <QHash>
#include <QtGlobal>

#include <memory>
#include <utility>

using tasqly::infra::logging::LogManager;
using Level = tasqly::app::logging::Level;

namespace {

// 🛑 Stub: always fails on write
class FailingWriteFile final : public tasqly::infra::logging::ILogFile
{
public:
  void setFileName(const QString& name) override { m_name = name; }
  QString fileName() const override { return m_name; }
  bool open(QIODevice::OpenMode) override
  {
    m_open = true;
    return true;
  }
  bool isOpen() const override { return m_open; }
  void close() override { m_open = false; }
  qint64 write(const QByteArray&) override { return -1; }
  bool flush() override { return false; }
  bool exists() const override { return false; }
  bool remove() override { return true; }
  bool rename(const QString&) override { return true; }
  bool copy(const QString&) override { return true; }

private:
  QString m_name;
  bool m_open{false};
};

// 🛑 Stub: always fails on flush
class FailingFlushFile final : public tasqly::infra::logging::ILogFile
{
public:
  void setFileName(const QString& name) override { m_name = name; }
  QString fileName() const override { return m_name; }
  bool open(QIODevice::OpenMode) override
  {
    m_open = true;
    return true;
  }
  bool isOpen() const override { return m_open; }
  void close() override { m_open = false; }
  qint64 write(const QByteArray& data) override
  {
    Q_UNUSED(data);
    return 1;
  }
  bool flush() override { return false; }
  bool exists() const override { return false; }
  bool remove() override { return true; }
  bool rename(const QString&) override { return true; }
  bool copy(const QString&) override { return true; }

private:
  QString m_name;
  bool m_open{false};
};

// 🔄 Rotation behavior stub
struct RotationBehavior
{
  QHash<QString, bool> exists;
  QHash<QString, bool> removeResult;
  QHash<QString, bool> renameResult;
  QHash<QString, bool> copyResult;
};

class RotationStubFile final : public tasqly::infra::logging::ILogFile
{
public:
  explicit RotationStubFile(std::shared_ptr<RotationBehavior> behavior)
      : m_behavior(std::move(behavior))
  {}

  void setFileName(const QString& name) override { m_name = name; }
  QString fileName() const override { return m_name; }
  bool open(QIODevice::OpenMode) override
  {
    m_open = true;
    return true;
  }
  bool isOpen() const override { return m_open; }
  void close() override { m_open = false; }
  qint64 write(const QByteArray& data) override
  {
    m_behavior->exists[m_name] = true;
    return data.size();
  }
  bool flush() override { return true; }
  bool exists() const override { return m_behavior->exists.value(m_name, false); }
  bool remove() override
  {
    const bool result = m_behavior->removeResult.value(m_name, true);
    if (result)
      m_behavior->exists[m_name] = false;
    return result;
  }
  bool rename(const QString& newName) override
  {
    const bool result = m_behavior->renameResult.value(m_name, true);
    if (result) {
      const bool wasExisting = m_behavior->exists.value(m_name, false);
      m_behavior->exists[newName] = wasExisting;
      m_behavior->exists[m_name] = false;
      m_name = newName;
    }
    return result;
  }
  bool copy(const QString& newName) override
  {
    const bool result = m_behavior->copyResult.value(m_name, true);
    if (result) {
      const bool wasExisting = m_behavior->exists.value(m_name, false);
      m_behavior->exists[newName] = wasExisting;
    }
    return result;
  }

private:
  QString m_name;
  bool m_open{false};
  std::shared_ptr<RotationBehavior> m_behavior;
};

} // namespace

// ------------------ Tests ------------------

TEST(LogManagerFailureInjectionTests, DisablesFileSinkWhenWriteFails)
{
  auto factory = []() { return std::make_unique<FailingWriteFile>(); };
  LogManager lm(nullptr, false, factory);

  lm.enableFileSink(true, QStringLiteral("stub_write_fail.log"));
  lm.log(Level::Info, "fail", "write failure");

  EXPECT_FALSE(LogManagerTestAccessor::isFileSinkEnabled(lm));
  EXPECT_TRUE(LogManagerTestAccessor::isConsoleEnabled(lm));
}

TEST(LogManagerFailureInjectionTests, DisablesFileSinkWhenFlushFails)
{
  auto factory = []() { return std::make_unique<FailingFlushFile>(); };
  LogManager lm(nullptr, false, factory);

  lm.setAutoFlush(true);
  lm.enableFileSink(true, QStringLiteral("stub_flush_fail.log"));
  lm.log(Level::Info, "fail", "flush failure");

  EXPECT_FALSE(LogManagerTestAccessor::isFileSinkEnabled(lm));
  EXPECT_TRUE(LogManagerTestAccessor::isConsoleEnabled(lm));
}

TEST(LogManagerFailureInjectionTests, RotationFailureDisablesFileSink)
{
  auto behavior = std::make_shared<RotationBehavior>();
  auto factory = [behavior]() { return std::make_unique<RotationStubFile>(behavior); };
  LogManager lm(nullptr, false, factory);

  const QString basePath = QStringLiteral("stub_rotation_fail.log");
  const QString absoluteBase = QFileInfo(basePath).absoluteFilePath();
  const QString oldest = QStringLiteral("%1.%2").arg(absoluteBase).arg(3);
  behavior->exists.insert(oldest, true);
  behavior->removeResult.insert(oldest, false);

  lm.setRotationPolicy({1, 3});
  lm.enableFileSink(true, basePath);
  lm.log(Level::Info, "fail", "rotation failure");

  LogManagerTestAccessor::forceRotate(lm);

  EXPECT_FALSE(LogManagerTestAccessor::isFileSinkEnabled(lm));
  EXPECT_TRUE(LogManagerTestAccessor::isConsoleEnabled(lm));
}

TEST(LogManagerFailureInjectionTests, RotationFallsBackToCopyWhenRenameFails)
{
  auto behavior = std::make_shared<RotationBehavior>();
  auto factory = [behavior]() { return std::make_unique<RotationStubFile>(behavior); };
  LogManager lm(nullptr, false, factory);

  const QString basePath = QStringLiteral("stub_rotation_copy_fallback.log");
  const QString absoluteBase = QFileInfo(basePath).absoluteFilePath();
  const QString rotated = QStringLiteral("%1.1").arg(absoluteBase);

  behavior->exists.insert(absoluteBase, true);
  behavior->renameResult.insert(absoluteBase, false);
  behavior->copyResult.insert(absoluteBase, true);
  behavior->removeResult.insert(absoluteBase, true);

  lm.setRotationPolicy({1, 2});
  lm.enableFileSink(true, basePath);
  lm.log(Level::Info, "fallback", "trigger rotation");

  LogManagerTestAccessor::forceRotate(lm);

  EXPECT_TRUE(LogManagerTestAccessor::isFileSinkEnabled(lm));
  EXPECT_TRUE(behavior->exists.value(rotated, false));
  EXPECT_FALSE(behavior->exists.value(absoluteBase, true));
}
