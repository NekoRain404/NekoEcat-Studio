// AlEventServiceTest / AlEventPluginTest — Tests for AlEventService and AlEventPlugin
//
// Test coverage:
//   - Signal forwarding from EcatClient
//   - Error forwarding
//   - Polling requires connection
//   - Plugin identity and accessibility
//   - Table population and severity filtering

#include <QTest>
#include <QApplication>
#include <QSignalSpy>
#include <QJsonObject>
#include <QJsonArray>

#include "plugins/alevent/AlEventPlugin.h"
#include "services/AlEventService.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

#include <QComboBox>
#include <QTableWidget>

// ── AlEventService tests ──────────────────────────────────────────────────

class AlEventServiceTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  AlEventService *svc_ = nullptr;

private slots:
  void init() {
    client_ = new EcatClient(this);
    svc_    = new AlEventService(client_, this);
  }

  void cleanup() {
    delete svc_;    svc_    = nullptr;
    delete client_; client_ = nullptr;
  }

  // The service should forward alEventLogResult into alEventUpdate.
  void testSignalForwarding() {
    QSignalSpy spy(svc_, &AlEventService::alEventUpdate);
    QVERIFY(spy.isValid());

    QJsonObject ev;
    ev["timestamp"]    = "12:00:00";
    ev["slavePosition"] = 1;
    ev["name"]          = "AL Status";
    ev["code"]          = 0x0001;
    ev["severity"]      = "Warning";
    ev["description"]   = "State changed to INIT";

    QJsonArray events;
    events.append(ev);

    QJsonObject payload;
    payload["events"] = events;

    emit client_->alEventLogResult(payload);
    QCOMPARE(spy.count(), 1);

    const QJsonObject received = spy.at(0).at(0).toJsonObject();
    QCOMPARE(received.value("events").toArray().size(), 1);
  }

  // Error forwarding from EcatClient.
  void testErrorForwarding() {
    QSignalSpy spy(svc_, &AlEventService::error);
    emit client_->errorMessage("boom");
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QString("boom"));
  }

  // Polling timer should fire requestUpdate only when connected.
  void testPollingRequiresConnection() {
    QSignalSpy spy(client_, &EcatClient::alEventLogResult);
    svc_->startPolling(50);  // 50 ms for fast test
    QTest::qWait(150);
    // Client is disconnected — no requests should have been sent.
    QCOMPARE(spy.count(), 0);
    svc_->stopPolling();
  }
};

// ── AlEventPlugin tests ───────────────────────────────────────────────────

class AlEventPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names (EN/ZH)
  void testIdentity() {
    EventBus bus;
    EcatClient client;
    AlEventService svc(&client);
    AlEventPlugin plugin(&bus, &svc);
    QCOMPARE(plugin.id(),            QString("alevent"));
    QCOMPARE(plugin.displayName(),   QString("AL Events"));
    QCOMPARE(plugin.displayNameZh(), QString("AL事件"));
  }

  // Verify default ordering value
  void testDefaultOrder() {
    EventBus bus;
    EcatClient client;
    AlEventService svc(&client);
    AlEventPlugin plugin(&bus, &svc);
    QCOMPARE(plugin.defaultOrder(), 65);
  }

  // Verify plugin is visible
  void testVisible() {
    EventBus bus;
    EcatClient client;
    AlEventService svc(&client);
    AlEventPlugin plugin(&bus, &svc);
    QVERIFY(plugin.visible());
  }

  // Verify main widget is created
  void testWidgetNotNull() {
    EventBus bus;
    EcatClient client;
    AlEventService svc(&client);
    AlEventPlugin plugin(&bus, &svc);
    QVERIFY(plugin.widget() != nullptr);
  }

  // Pushing data through the service should populate the plugin table.
  void testServiceIntegration() {
    EventBus bus;
    EcatClient client;
    AlEventService svc(&client);
    AlEventPlugin plugin(&bus, &svc);

    QJsonObject ev;
    ev["timestamp"]     = "12:00:00";
    ev["slavePosition"] = 1;
    ev["name"]          = "AL Status";
    ev["code"]          = 0x0001;
    ev["severity"]      = "Warning";
    ev["description"]   = "State changed to INIT";

    QJsonArray events;
    events.append(ev);

    QJsonObject payload;
    payload["events"] = events;

    emit svc.alEventUpdate(payload);

    auto *table = plugin.widget()->findChild<QTableWidget *>();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 1);
  }

  // Severity filter should hide non-matching rows.
  void testSeverityFilter() {
    EventBus bus;
    EcatClient client;
    AlEventService svc(&client);
    AlEventPlugin plugin(&bus, &svc);

    // Add two events: one Error, one Info.
    QJsonObject ev1;
    ev1["timestamp"]     = "12:00:00";
    ev1["slavePosition"] = 1;
    ev1["name"]          = "Evt";
    ev1["code"]          = 1;
    ev1["severity"]      = "Error";
    ev1["description"]   = "desc1";

    QJsonObject ev2;
    ev2["timestamp"]     = "12:00:01";
    ev2["slavePosition"] = 2;
    ev2["name"]          = "Evt2";
    ev2["code"]          = 2;
    ev2["severity"]      = "Info";
    ev2["description"]   = "desc2";

    QJsonArray events;
    events.append(ev1);
    events.append(ev2);

    QJsonObject payload;
    payload["events"] = events;
    emit svc.alEventUpdate(payload);

    auto *table = plugin.widget()->findChild<QTableWidget *>();
    auto *combo = plugin.widget()->findChild<QComboBox *>();
    QVERIFY(table != nullptr);
    QVERIFY(combo != nullptr);
    QCOMPARE(table->rowCount(), 2);

    // Filter to "Error" — row 0 visible, row 1 hidden.
    combo->setCurrentIndex(1);  // "Error"
    QVERIFY(!table->isRowHidden(0));
    QVERIFY(table->isRowHidden(1));

    // Filter back to "All" — both visible.
    combo->setCurrentIndex(0);
    QVERIFY(!table->isRowHidden(0));
    QVERIFY(!table->isRowHidden(1));
  }
};

// ── Combined runner ───────────────────────────────────────────────────────

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  int status = 0;
  {
    AlEventServiceTest t;
    status |= QTest::qExec(&t, argc, argv);
  }
  {
    AlEventPluginTest t;
    status |= QTest::qExec(&t, argc, argv);
  }
  return status;
}

// Required for the moc-included Q_OBJECT classes in this translation unit.
#include "al_event_service_test.moc"
