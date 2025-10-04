/*
 * 🧱 File: test_log_manager_filesink.cpp
 * --------------------------------------
 * 📌 Purpose   : Cover LogManager file sink edge-cases.
 * 🧱 Layer     : Tests (Unit - Logging)
 * 👤 Author    : OpenAI Assistant
 * 🗓️ Created   : 2025-10-09
 *
 * 🧠 Description:
 * Ensures LogManager correctly handles file sink configuration when:
 * - An empty/whitespace path is supplied (should fallback to default path).
 * - Switching between different file paths flushes and closes the previous file.
 */

#include <gtest/gtest.h>

#include "infra/logging/LogManager.h"

#include <memory>

using tasqly::infra::logging::ILogFile;
using tasqly::infra::logging::LogManager;

namespace {

struct FileCapture
{
  QString lastFileName;
  int openCount{0};
  int closeCount{0};
  int flushCount{0};
};

class TrackingFile final : public ILogFile
{
public:
  explicit TrackingFile(std::shared_ptr<FileCapture> capture)
      : m_capture(std::move(capture))
  {}

  void setFileName(const QString& name) override
  {
    m_name = name;
    m_capture->lastFileName = name;
  }

  QString fileName() const override { return m_name; }

  bool open(QIODevice::OpenMode) override
  {
    m_capture->openCount++;
    m_open = true;
    return true;
  }

  bool isOpen() const override { return m_open; }

  void close() override
  {
    if (m_open) {
      m_capture->closeCount++;
      m_open = false;
    }
  }

  qint64 write(const QByteArray& data) override { return data.size(); }

  bool flush() override
  {
    m_capture->flushCount++;
    return true;
  }

  bool exists() const override { return m_exists; }

  bool remove() override
  {
    m_exists = false;
    return true;
  }

  bool rename(const QString& newName) override
  {
    m_name = newName;
    m_capture->lastFileName = newName;
    return true;
  }

  bool copy(const QString&) override { return true; }

private:
  std::shared_ptr<FileCapture> m_capture;
  QString m_name;
  bool m_open{false};
  bool m_exists{true};
};

} // namespace

TEST(LogManagerFileSinkTests, UsesDefaultPathWhenWhitespaceProvided)
{
  auto capture = std::make_shared<FileCapture>();
  auto factory = [capture]() { return std::make_unique<TrackingFile>(capture); };

  LogManager lm(nullptr, false, factory);
  lm.enableConsole(false);
  lm.enableFileSink(true, QStringLiteral("   "));

  ASSERT_EQ(1, capture->openCount);
  EXPECT_TRUE(capture->lastFileName.endsWith(QStringLiteral("tasqly.log")));

  lm.enableFileSink(false);
  EXPECT_EQ(1, capture->closeCount);
}

TEST(LogManagerFileSinkTests, SwitchingFilePathFlushesAndReopens)
{
  auto capture = std::make_shared<FileCapture>();
  auto factory = [capture]() { return std::make_unique<TrackingFile>(capture); };

  LogManager lm(nullptr, false, factory);
  lm.enableConsole(false);

  lm.enableFileSink(true, QStringLiteral("first.log"));
  ASSERT_EQ(1, capture->openCount);
  EXPECT_EQ(QStringLiteral("first.log"), capture->lastFileName);

  lm.enableFileSink(true, QStringLiteral("second.log"));

  EXPECT_EQ(2, capture->openCount);
  EXPECT_EQ(1, capture->closeCount);
  EXPECT_EQ(1, capture->flushCount);
  EXPECT_EQ(QStringLiteral("second.log"), capture->lastFileName);
}
