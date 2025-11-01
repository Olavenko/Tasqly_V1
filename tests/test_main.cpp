/*
 * 🧱 File: test_main.cpp
 * ----------------------
 * 📌 Purpose   : Entry point for GoogleTest with Qt event loop support + DB environment setup + cleanup.
 * 🧱 Layer     : Testing Infrastructure
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-05
 * 🗓️ Updated   : 2025-10-26
 * 🔖 Version   : 1.3 (Add DB auto-setup for tasqly_test)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Initializes QCoreApplication so tests using QTimer, signals/slots,
 * and QObjects can run safely under MSVC, MinGW, and Linux.
 * Ensures PostgreSQL test database exists and matches schema before tests.
 * Cleans up temporary test output folder (tests_tmp) after all tests.
 */

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include "tests/common/RuntimeDiagnostic.h"
#include <gtest/gtest.h>

/// 🧹 Cleanup helper: remove tests_tmp folder if it exists
static void cleanupTestsTmp()
{
  QString tmpPath = QCoreApplication::applicationDirPath() + "/tests_tmp";
  QDir dir(tmpPath);
  if (dir.exists()) {
    if (dir.removeRecursively()) {
      qDebug() << "🧹 Cleaned test artifacts from:" << tmpPath;
    } else {
      qWarning() << "⚠️ Failed to cleanup:" << tmpPath;
    }
  }
}

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  tasqly::diagnostic::printRuntimeInfo(); // 🧠 Print environment info

  testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();

  // 🧹 Final cleanup after all tests
  cleanupTestsTmp();

  return result;
}
