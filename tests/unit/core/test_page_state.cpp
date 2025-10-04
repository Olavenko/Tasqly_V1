/*
 * 🧪 File: test_page_state.cpp
 * ----------------------------
 * 📌 Purpose   : Tests for PageState enum and toString helper.
 * 🧱 Layer     : Unit (Domain/Core)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-23
 *
 * 🧠 Description:
 * Ensures PageState values map correctly to strings and are
 * registered in Qt's meta-object system (Q_ENUM).
 */

#include "domain/core/PageState.h"
#include <gtest/gtest.h>
#include <QMetaEnum>

// ✅ Test 1: toString returns correct string for each state
TEST(PageStateTests, ToStringMatchesEnum)
{
  EXPECT_EQ(toString(PageStateWrapper::Ready), "Ready");
  EXPECT_EQ(toString(PageStateWrapper::Loading), "Loading");
  EXPECT_EQ(toString(PageStateWrapper::Empty), "Empty");
  EXPECT_EQ(toString(PageStateWrapper::Error), "Error");
}

// ✅ Test 2: QMetaEnum contains all values
TEST(PageStateTests, MetaEnumContainsValues)
{
  const QMetaObject &mo = PageStateWrapper::staticMetaObject;
  int index = mo.indexOfEnumerator("PageState");
  ASSERT_NE(index, -1);

  QMetaEnum metaEnum = mo.enumerator(index);

  EXPECT_STREQ(metaEnum.valueToKey(PageStateWrapper::Ready), "Ready");
  EXPECT_STREQ(metaEnum.valueToKey(PageStateWrapper::Loading), "Loading");
  EXPECT_STREQ(metaEnum.valueToKey(PageStateWrapper::Empty), "Empty");
  EXPECT_STREQ(metaEnum.valueToKey(PageStateWrapper::Error), "Error");

  EXPECT_EQ(metaEnum.keyToValue("Ready"), PageStateWrapper::Ready);
  EXPECT_EQ(metaEnum.keyToValue("Loading"), PageStateWrapper::Loading);
  EXPECT_EQ(metaEnum.keyToValue("Empty"), PageStateWrapper::Empty);
  EXPECT_EQ(metaEnum.keyToValue("Error"), PageStateWrapper::Error);
}

// ✅ Test 3: toString handles invalid value → "Unknown"
TEST(PageStateTests, ToStringHandlesInvalidValue)
{
  auto invalid = static_cast<PageStateWrapper::PageState>(999);
  EXPECT_EQ(toString(invalid), "Unknown");
}
