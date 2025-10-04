/*
 * 🧱 File: test_feature_flag_signal.cpp
 * -------------------------------------
 * 📌 Purpose   : Unit test for FeatureFlagsManager::featureChanged signal.
 * 🧱 Layer     : Tests (Unit)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-04
 *
 * 🧠 Description:
 * Verifies that when a feature flag changes via setFeature(), the
 * featureChanged(QString,bool) signal is emitted with the correct
 * key and value.
 */

#include <gtest/gtest.h>
#include <QSignalSpy>

#include "app/settings/FeatureFlagsManager.h"

TEST(FeatureFlagsSignalTests, EmitsCorrectKeyAndValue)
{
  auto& features = FeatureFlagsManager::instance();
  const QString key = QStringLiteral("features.test_signal");

  // Ensure known starting state
  features.setFeature(key, false);

  QSignalSpy spy(&features, &FeatureFlagsManager::featureChanged);
  features.setFeature(key, true);

  ASSERT_EQ(spy.count(), 1);
  const auto args = spy.takeFirst();
  EXPECT_EQ(args.at(0).toString(), key);
  EXPECT_EQ(args.at(1).toBool(), true);
}
