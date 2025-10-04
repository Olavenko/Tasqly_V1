/*
 * 🧱 File: test_masking.cpp
 * -------------------------
 * 📌 Purpose   : Verify sensitive fields are masked in log output.
 * 🧱 Layer     : Tests (Unit - Infrastructure)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 * 🗓️ Updated   : 2025-09-25
 * 🔖 Version   : 2.0 (Strict checks & cleanup)
 * 🛡️ Stability : Stable
 *
 * 🧠 Description:
 * Ensures LogManager masks sensitive fields like password, token, secret,
 * and authorization, replacing them with "***" in the log output.
 * Enhancements in v2:
 * - Cleanup rotated logs before each test
 * - More explicit checks using EXPECT_NE/EXPECT_EQ
 *
 * 🔗 Depends On:
 *   - infra/logging/LogManager.h
 *   - app/settings/FeatureFlagsManager.h
 * 🔗 Related   : test_masking_concurrency.cpp
 * ✅ Tested In : TasqlyTestsRunner
 * ⚠️ Errors    : Test fails if sensitive keys leak unmasked
 * 🕒 Last Major Change: 2025-09-25
 */

#include <gtest/gtest.h>

// Qt
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QVariantMap>

// Project
#include "app/settings/FeatureFlagsManager.h"
#include "infra/logging/LogManager.h"

using tasqly::app::logging::Level;
using tasqly::infra::logging::LogManager;

/// 🛠️ Helper: read log file content
static QString readLogFile(const QString& path)
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly)) {
    return {};
  }
  QString content = QString::fromUtf8(f.readAll());
  f.close();
  return content;
}

/// 🧪 Fixture: enable logging feature flag
class MaskingTests : public ::testing::Test
{
protected:
  void SetUp() override { FeatureFlagsManager::instance().setFeature("features.logging", true); }

  void cleanup(const QString& basePath)
  {
    QFile::remove(basePath);
    for (int i = 1; i <= 3; ++i) {
      QFile::remove(basePath + "." + QString::number(i));
    }
  }
};

// 🧪 Test 1: default sensitive keys must be masked
TEST_F(MaskingTests, DefaultSensitiveKeysAreMasked)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/masking_default.log");
  cleanup(basePath);

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);

  QVariantMap ctx;
  ctx["username"] = "alice";
  ctx["password"] = "mypassword";
  ctx["token"] = "abcd1234";

  lm.log(Level::Info, "auth", "login attempt", ctx);
  lm.flush();
  lm.enableFileSink(false);

  QString log = readLogFile(basePath);
  EXPECT_NE(log.indexOf("username=alice"), -1);
  EXPECT_NE(log.indexOf("password=***"), -1);
  EXPECT_NE(log.indexOf("token=***"), -1);
}

// 🧪 Test 2: non-sensitive keys must remain visible
TEST_F(MaskingTests, NonSensitiveKeysRemainVisible)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/masking_visible.log");
  cleanup(basePath);

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);

  QVariantMap ctx;
  ctx["id"] = 42;
  ctx["city"] = "Cairo";

  lm.log(Level::Info, "profile", "update", ctx);
  lm.flush();
  lm.enableFileSink(false);

  QString log = readLogFile(basePath);
  EXPECT_NE(log.indexOf("id=42"), -1);
  EXPECT_NE(log.indexOf("city=Cairo"), -1);
}

// 🧪 Test 3: custom keys can be masked
TEST_F(MaskingTests, CustomKeyCanBeMasked)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/masking_custom.log");
  cleanup(basePath);

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);
  lm.addMaskedKey("apiKey");

  QVariantMap ctx;
  ctx["apiKey"] = "XYZ987";
  ctx["other"] = "value";

  lm.log(Level::Info, "network", "request", ctx);
  lm.flush();
  lm.enableFileSink(false);

  QString log = readLogFile(basePath);
  EXPECT_NE(log.indexOf("apiKey=***"), -1);
  EXPECT_NE(log.indexOf("other=value"), -1);
}

// 🧪 Test 4: overriding masked keys replaces defaults
TEST_F(MaskingTests, SetMaskedKeysOverridesDefaults)
{
  QDir dir(QCoreApplication::applicationDirPath());
  dir.mkpath("tests_tmp");
  QString basePath = dir.filePath("tests_tmp/masking_override.log");
  cleanup(basePath);

  LogManager lm;
  lm.enableConsole(false);
  lm.enableFileSink(true, basePath);

  // override → mask only "city"
  lm.setMaskedKeys({"city"});

  QVariantMap ctx;
  ctx["password"] = "mypassword"; // should not be masked now
  ctx["city"] = "Alex";

  lm.log(Level::Info, "test", "check", ctx);
  lm.flush();
  lm.enableFileSink(false);

  QString log = readLogFile(basePath);
  EXPECT_NE(log.indexOf("password=mypassword"), -1);
  EXPECT_NE(log.indexOf("city=***"), -1);
}
