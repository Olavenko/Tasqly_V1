/*
 * 🧪 File: test_navigation_service_extended.cpp
 * ---------------------------------------------
 * 📌 Purpose   : Extended unit/integration tests for NavigationService + NavigateTo
 * 🧱 Layer     : Tests (Unit + Integration / Navigation)
 * 👤 Author    : Tasqly AI Assistant
 * 🗓️ Created   : 2025-09-21
 *
 * 🧠 Description:
 * Complements existing tests by covering missing scenarios:
 *  - Notifier integration on navigation failure.
 *  - goBack with multi-route history.
 *  - currentRoute correctness.
 *  - retryLast with valid last route.
 *  - NavigateTo retry with invalid lastRoute.
 *  - NavigateTo lastRoute storage validation.
 */

#include "app/logging/ILogManager.h"
#include "app/navigation/NavigationService.h"
#include "app/notifier/Notifier.h"
#include "app/usecases/NavigateTo.h"
#include "domain/core/PageState.h"
#include "domain/core/Route.h"
#include <gtest/gtest.h>
#include <QDateTime>
#include <QSignalSpy>

Q_DECLARE_METATYPE(Route)
Q_DECLARE_METATYPE(PageStateWrapper::PageState)

// 🧩 Dummy logger
class DummyLogger : public tasqly::app::logging::ILogManager
{
public:
  void setLevel(tasqly::app::logging::Level) override {}
  tasqly::app::logging::Level level() const override { return tasqly::app::logging::Level::Info; }
  bool isEnabled(tasqly::app::logging::Level) const override { return true; }
  void enableConsole(bool) override {}
  void enableFileSink(bool, const QString& = QString()) override {}
  void setRotationPolicy(const tasqly::app::logging::RotationPolicy&) override {}
  void rotateNow() override {}
  void flush() override {}
  void addMaskedKey(const QString&) override {}
  void setMaskedKeys(const QStringList&) override {}
  QStringList maskedKeys() const override { return {}; }
  void log(tasqly::app::logging::Level,
           const QString&,
           const QString&,
           const QVariantMap& = {}) override
  {}
};

// 🧪 Fixture
class NavigationServiceExtendedTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    qRegisterMetaType<Route>("Route");
    qRegisterMetaType<PageStateWrapper::PageState>("PageStateWrapper::PageState");
    Notifier::resetInstance();
    notifier = new Notifier();
    logger = new DummyLogger();
    service = new NavigationService(logger);
  }

  void TearDown() override
  {
    delete service;
    delete logger;
    delete notifier;
    Notifier::resetInstance();
  }

  DummyLogger* logger;
  NavigationService* service;
  Notifier* notifier;
};

// ✅ Test 1: Notifier emits when navigation fails
TEST_F(NavigationServiceExtendedTest, InvalidRouteEmitsNotifier)
{
  QSignalSpy spy(notifier, &Notifier::notify);
  Route invalid;
  service->navigateTo(invalid);
  ASSERT_TRUE(spy.wait(500));
  EXPECT_EQ(spy.count(), 1);
  EXPECT_TRUE(spy.takeFirst().at(0).toString().contains("Invalid"));
}

// ✅ Test 2: goBack returns to previous route
TEST_F(NavigationServiceExtendedTest, GoBackWithHistoryRestoresPreviousRoute)
{
  Route r1{"dashboard", QDateTime::currentDateTimeUtc()};
  Route r2{"settings", QDateTime::currentDateTimeUtc().addSecs(1)};
  service->navigateTo(r1);
  service->navigateTo(r2);
  service->goBack();
  EXPECT_EQ(service->currentRoute().id, "dashboard");
}

// ✅ Test 3: currentRoute matches last navigated
TEST_F(NavigationServiceExtendedTest, CurrentRouteReturnsTopOfStack)
{
  Route r{"profile", QDateTime::currentDateTimeUtc()};
  service->navigateTo(r);
  EXPECT_EQ(service->currentRoute().id, "profile");
}

// ✅ Test 4: retryLast with valid route works
TEST_F(NavigationServiceExtendedTest, RetryLastValidRouteSucceeds)
{
  Route r{"reports", QDateTime::currentDateTimeUtc()};
  QSignalSpy spy(service, &NavigationService::navigated);
  service->navigateTo(r);
  spy.clear();
  service->retryLast();
  QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
  ASSERT_GT(spy.count(), 0);
  Route emitted = spy.takeFirst().at(0).value<Route>();
  EXPECT_EQ(emitted.id, "reports");
}

// ✅ Test 5: NavigateTo retry with invalid lastRoute emits Error
TEST_F(NavigationServiceExtendedTest, NavigateToRetryWithInvalidRouteFails)
{
  NavigateTo usecase(service, logger);
  QSignalSpy spy(&usecase, &NavigateTo::pageStateChanged);
  usecase.retry();
  QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
  ASSERT_GT(spy.count(), 0);
  EXPECT_EQ(spy.takeLast().at(0).toInt(), PageStateWrapper::Error);
}

// ✅ Test 6: NavigateTo execute stores lastRoute
TEST_F(NavigationServiceExtendedTest, NavigateToExecuteStoresLastRoute)
{
  NavigateTo usecase(service, logger);
  QVariantMap data;
  data["id"] = "tasks";
  data["timestamp"] = QDateTime::currentDateTimeUtc();
  usecase.execute(data);
  // Retry should reuse same id
  QSignalSpy spy(&usecase, &NavigateTo::pageStateChanged);
  usecase.retry();
  QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
  ASSERT_GT(spy.count(), 0);
  EXPECT_GE(spy.count(), 2); // Loading + Ready
}
