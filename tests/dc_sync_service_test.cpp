// DcSyncServiceTest — Tests for DcSyncService and DcSyncPlugin
//
// Test coverage:
//   - Signal forwarding from EcatClient to DcSyncService
//   - Error forwarding
//   - Polling requires connection
//   - Plugin identity, ordering, visibility
//   - EventBus integration populates plugin table

#include <QTest>
#include <QApplication>
#include <QSignalSpy>
#include <QJsonObject>
#include <QJsonArray>

#include "plugins/dcsync/DcSyncPlugin.h"
#include <QTableWidget>
#include "services/DcSyncService.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

// ── DcSyncService tests ───────────────────────────────────────────────

class DcSyncServiceTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  DcSyncService *svc_ = nullptr;

private slots:
  void init() {
    client_ = new EcatClient(this);
    svc_    = new DcSyncService(client_, this);
  }

  void cleanup() {
    delete svc_;    svc_    = nullptr;
    delete client_; client_ = nullptr;
  }

  // Verify dcSyncStatusResult is forwarded into dcSyncUpdate signal
  void testSignalForwarding() {
    QSignalSpy spy(svc_, &DcSyncService::dcSyncUpdate);
    QVERIFY(spy.isValid());

    QJsonObject slave;
    slave["position"]  = 1;
    slave["name"]      = "EL1008";
    slave["dcCapable"] = true;
    slave["syncing"]   = true;
    slave["driftNs"]   = 42.0;
    slave["jitterMin"] = 1.0;
    slave["jitterMax"] = 9.0;
    slave["jitterAvg"] = 4.5;

    QJsonArray slaves;
    slaves.append(slave);

    QJsonObject payload;
    payload["slaves"]                = slaves;
    payload["referenceClockPosition"] = 0;
    payload["referenceClockName"]    = "EK1100";

    emit client_->dcSyncStatusResult(payload);
    QCOMPARE(spy.count(), 1);

    const QJsonObject received = spy.at(0).at(0).toJsonObject();
    QCOMPARE(received.value("referenceClockPosition").toInt(), 0);
    QCOMPARE(received.value("slaves").toArray().size(), 1);
  }

  // Verify error messages are forwarded from EcatClient
  void testErrorForwarding() {
    QSignalSpy spy(svc_, &DcSyncService::error);
    emit client_->errorMessage("boom");
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QString("boom"));
  }

  // Verify polling timer does not fire when disconnected
  void testPollingRequiresConnection() {
    QSignalSpy spy(client_, &EcatClient::dcSyncStatusResult);
    svc_->startPolling(50);  // 50 ms for fast test
    QTest::qWait(150);
    // Client is disconnected — no requests should have been sent.
    QCOMPARE(spy.count(), 0);
    svc_->stopPolling();
  }
};

// ── DcSyncPlugin tests ────────────────────────────────────────────────

class DcSyncPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id and display names
  void testIdentity() {
    EventBus bus;
    EcatClient client;
    DcSyncService svc(&client);
    DcSyncPlugin plugin(&bus, &svc);
    QCOMPARE(plugin.id(),           QString("dcsync"));
    QCOMPARE(plugin.displayName(),  QString("DC Sync"));
    QCOMPARE(plugin.displayNameZh(), QString("DC同步"));
  }

  // Verify default tab order
  void testDefaultOrder() {
    EventBus bus;
    EcatClient client;
    DcSyncService svc(&client);
    DcSyncPlugin plugin(&bus, &svc);
    QCOMPARE(plugin.defaultOrder(), 60);
  }

  // Verify plugin is visible
  void testVisible() {
    EventBus bus;
    EcatClient client;
    DcSyncService svc(&client);
    DcSyncPlugin plugin(&bus, &svc);
    QVERIFY(plugin.visible());
  }

  // Verify main widget is created
  void testWidgetNotNull() {
    EventBus bus;
    EcatClient client;
    DcSyncService svc(&client);
    DcSyncPlugin plugin(&bus, &svc);
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify EventBus data populates plugin table rows
  void testEventBusIntegration() {
    EventBus bus;
    EcatClient client;
    DcSyncService svc(&client);
    DcSyncPlugin plugin(&bus, &svc);

    QJsonObject slave;
    slave["position"]  = 1;
    slave["name"]      = "EL2004";
    slave["dcCapable"] = true;
    slave["syncing"]   = false;
    slave["driftNs"]   = 0.0;
    slave["jitterMin"] = 0.0;
    slave["jitterMax"] = 0.0;
    slave["jitterAvg"] = 0.0;

    QJsonArray slaves;
    slaves.append(slave);

    QJsonObject payload;
    payload["slaves"]                = slaves;
    payload["referenceClockPosition"] = 0;
    payload["referenceClockName"]    = "EK1100";

    bus.emitDcSyncUpdate(payload);

    // Table should have 2 rows: 1 ref-clock + 1 slave.
    auto *table = plugin.widget()->findChild<QTableWidget *>();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 2);
  }
};

// ── Combined runner ───────────────────────────────────────────────────

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  int status = 0;
  {
    DcSyncServiceTest t;
    status |= QTest::qExec(&t, argc, argv);
  }
  {
    DcSyncPluginTest t;
    status |= QTest::qExec(&t, argc, argv);
  }
  return status;
}

// Required for the moc-included Q_OBJECT classes in this translation unit.
#include "dc_sync_service_test.moc"
