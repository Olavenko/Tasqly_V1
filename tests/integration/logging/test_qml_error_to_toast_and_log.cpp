/*
 * 🧱 File: test_qml_error_to_toast_and_log.cpp
 * -------------------------
 * 📌 Purpose   : Integration test: QML error → ErrorReporter → LogManager + Toast.
 * 🧱 Layer     : Tests (Integration)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 *
 * 🧠 Description:
 * Simulates an error triggered from QML, ensures it is logged
 * by LogManager and the toastRequested signal is emitted.
 */

#include "src/app/logging/ErrorReporter.h"
#include "src/domain/core/Error.h"
#include "src/infra/logging/LogManager.h"
#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QtTest/QSignalSpy>

using namespace tasqly::infra::logging;
using namespace tasqly::app::logging;
using tasqly::domain::core::Error;

static QString readLogFile(const QString& path)
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly)) {
    return QString();
  }
  QString content = QString::fromUtf8(f.readAll());
  f.close();
  return content;
}

TEST(IntegrationTests, QmlErrorTriggersToastAndLog)
{
  QString basePath = QDir::currentPath() + "/tests_tmp/integration_qml.log";
  QFile::remove(basePath);

  // Setup LogManager
  auto logManager = std::make_shared<LogManager>();
  logManager->enableConsole(false);
  logManager->enableFileSink(true, basePath);

  // Setup ErrorReporter
  ErrorReporter reporter(logManager);

  // Spy on toastRequested
  QSignalSpy spy(&reporter, &ErrorReporter::toastRequested);

  // Simulate error as if from QML
  Error err(1001, "User login failed", {{"password", "1234"}, {"username", "alice"}});
  reporter.report(err, Severity::Error, "auth");

  logManager->flush();
  logManager->enableFileSink(false);

  // --- Verify log file ---
  QString log = readLogFile(basePath);
  EXPECT_TRUE(log.contains("User login failed"));
  EXPECT_TRUE(log.contains("password=***")); // masked
  EXPECT_TRUE(log.contains("username=alice"));

  // --- Verify toast signal ---
  EXPECT_GE(spy.count(), 1);
  QList<QVariant> args = spy.takeFirst();
  EXPECT_EQ(args.at(0).toInt(), static_cast<int>(Severity::Error));
  EXPECT_EQ(args.at(1).toString(), QString("User login failed"));
}
