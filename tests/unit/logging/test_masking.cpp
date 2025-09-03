/*
 * 🧱 File: test_masking.cpp
 * -------------------------
 * 📌 Purpose   : Verify sensitive fields are masked in log output.
 * 🧱 Layer     : Tests (Unit)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 *
 * 🧠 Description:
 * Ensures LogManager masks sensitive fields like password, token, secret,
 * and authorization, replacing them with "***" in the log output.
 */

#include "src/infra/logging/LogManager.h"
#include <gtest/gtest.h>

#include <QDir>
#include <QFile>
#include <QVariantMap>

using namespace tasqly::infra::logging;
using tasqly::app::logging::Level;

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

TEST(MaskingTests, DefaultSensitiveKeysAreMasked)
{
  QString basePath = QDir::currentPath() + "/tests_tmp/masking_default.log";
  QFile::remove(basePath);

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
  EXPECT_TRUE(log.contains("username=alice"));
  EXPECT_TRUE(log.contains("password=***"));
  EXPECT_TRUE(log.contains("token=***"));
}

TEST(MaskingTests, NonSensitiveKeysRemainVisible)
{
  QString basePath = QDir::currentPath() + "/tests_tmp/masking_visible.log";
  QFile::remove(basePath);

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
  EXPECT_TRUE(log.contains("id=42"));
  EXPECT_TRUE(log.contains("city=Cairo"));
}

TEST(MaskingTests, CustomKeyCanBeMasked)
{
  QString basePath = QDir::currentPath() + "/tests_tmp/masking_custom.log";
  QFile::remove(basePath);

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
  EXPECT_TRUE(log.contains("apiKey=***"));
  EXPECT_TRUE(log.contains("other=value"));
}

TEST(MaskingTests, SetMaskedKeysOverridesDefaults)
{
  QString basePath = QDir::currentPath() + "/tests_tmp/masking_override.log";
  QFile::remove(basePath);

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
  EXPECT_TRUE(log.contains("password=mypassword"));
  EXPECT_TRUE(log.contains("city=***"));
}
