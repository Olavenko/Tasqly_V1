/*
 * 🧱 File: test_feature_flag_logging.cpp
 * -------------------------------------
 * 📌 Purpose   : Integration test — verify that logging subsystem respects `features.logging` flag.
 * 🧱 Layer     : Tests (Integration)
 * 👤 Author    : Mohamed Ali
 * 🗓️ Created   : 2025-09-03
 *
 * 🧠 Description:
 * Ensures that when `features.logging` is disabled via FeatureFlagsManager,
 * LogManager does not produce log output. When enabled, logs are written normally.
 */

#include <gtest/gtest.h>
#include <QTemporaryFile>
#include <QTextStream>
#include <QFile>

#include "src/app/settings/FeatureFlagsManager.h"
#include "src/infra/logging/LogManager.h"

using namespace tasqly::infra::logging;

TEST(Integration_FeatureFlags, LoggingRespectsFeatureFlag) {
    auto& features = FeatureFlagsManager::instance();
    LogManager logger;

    // 🔧 Setup: create temp file for logging
    QTemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.open());
    QString logPath = tmpFile.fileName();
    tmpFile.close();

    logger.enableFileSink(true, logPath);

    // 🟢 Case 1: Enable logging → expect output
    features.setFeature("features.logging", true);
    logger.log(LogManager::Level::Info, "test_enabled", "Message should appear");

    QFile file1(logPath);
    ASSERT_TRUE(file1.open(QIODevice::ReadOnly | QIODevice::Text));
    QString contents1 = QString::fromUtf8(file1.readAll());
    file1.close();

    EXPECT_TRUE(contents1.contains("Message should appear"))
        << "Expected log message not found when logging enabled";

    // 🔴 Case 2: Disable logging → expect no new output
    features.setFeature("features.logging", false);
    logger.log(LogManager::Level::Info, "test_disabled", "Message should NOT appear");

    QFile file2(logPath);
    ASSERT_TRUE(file2.open(QIODevice::ReadOnly | QIODevice::Text));
    QString contents2 = QString::fromUtf8(file2.readAll());
    file2.close();

    EXPECT_EQ(contents1, contents2)
        << "Log file changed even though logging feature flag was disabled";
}
