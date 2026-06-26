// MultiMasterPluginTest — Tests for Multi-Master Plugin and Service
//
// Test coverage:
//   - MultiMasterService: add/remove masters, discover, configure, monitor, sync
//   - MultiMasterPlugin: identity, widget creation, master list, details
//   - MasterComparisonWidget: set masters, difference counting
//   - Signal emission verification
#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QLabel>
#include <QPlainTextEdit>
#include "plugins/multimaster/MultiMasterPlugin.h"
#include "plugins/multimaster/MasterComparisonWidget.h"
#include "services/MultiMasterService.h"

class MultiMasterPluginTest : public QObject {
  Q_OBJECT
private slots:
  // ── Service Tests ──────────────────────────────────────────────────

  void testServiceCreation() {
    MultiMasterService svc(nullptr, nullptr);
    QCOMPARE(svc.masterCount(), 0);
  }

  void testAddMaster() {
    MultiMasterService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &MultiMasterService::masterDiscovered);

    MmMasterInfo info;
    info.adapterName = "eth0";
    info.state = MultiMasterState::Idle;
    QVERIFY(svc.addMaster(info));
    QCOMPARE(svc.masterCount(), 1);
    QCOMPARE(spy.count(), 1);
  }

  void testAddDuplicateMaster() {
    MultiMasterService svc(nullptr, nullptr);
    MmMasterInfo info;
    info.masterId = 0;
    info.adapterName = "eth0";
    svc.addMaster(info);

    MmMasterInfo dup;
    dup.masterId = 0;
    dup.adapterName = "eth1";
    QVERIFY(!svc.addMaster(dup));
    QCOMPARE(svc.masterCount(), 1);
  }

  void testRemoveMaster() {
    MultiMasterService svc(nullptr, nullptr);
    MmMasterInfo info;
    info.adapterName = "eth0";
    svc.addMaster(info);
    QCOMPARE(svc.masterCount(), 1);

    QVERIFY(svc.removeMaster(0));
    QCOMPARE(svc.masterCount(), 0);
  }

  void testRemoveNonexistentMaster() {
    MultiMasterService svc(nullptr, nullptr);
    QVERIFY(!svc.removeMaster(99));
  }

  void testConfigureMaster() {
    MultiMasterService svc(nullptr, nullptr);
    MmMasterInfo info;
    info.adapterName = "eth0";
    svc.addMaster(info);

    MmMasterConfig config;
    config.adapterName = "eth1";
    config.cycleTime = 2000;
    QVERIFY(svc.configureMaster(0, config));

    auto updated = svc.masterInfo(0);
    QCOMPARE(updated.adapterName, QString("eth1"));
  }

  void testConfigureNonexistentMaster() {
    MultiMasterService svc(nullptr, nullptr);
    MmMasterConfig config;
    QVERIFY(!svc.configureMaster(99, config));
  }

  void testMonitorMaster() {
    MultiMasterService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &MultiMasterService::masterStatusChanged);

    MmMasterInfo info;
    info.adapterName = "eth0";
    info.slaveCount = 5;
    info.state = MultiMasterState::Active;
    svc.addMaster(info);

    auto status = svc.monitorMaster(0);
    QCOMPARE(status.masterId, 0);
    QCOMPARE(status.slaveCount, 5);
    QCOMPARE(spy.count(), 1);
  }

  void testMonitorNonexistentMaster() {
    MultiMasterService svc(nullptr, nullptr);
    auto status = svc.monitorMaster(99);
    QCOMPARE(status.state, MultiMasterState::Unknown);
  }

  void testSynchronizeMastersFailsClosedWithoutBackend() {
    MultiMasterService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &MultiMasterService::masterSyncCompleted);

    MmMasterInfo src;
    src.adapterName = "eth0";
    src.state = MultiMasterState::Active;
    svc.addMaster(src);

    MmMasterInfo dst;
    dst.adapterName = "eth1";
    dst.state = MultiMasterState::Active;
    svc.addMaster(dst);

    QVERIFY(!svc.synchronizeMasters(0, 1));
    QCOMPARE(spy.count(), 1);

    auto result = qvariant_cast<MmMasterSyncResult>(spy.at(0).at(0));
    QVERIFY(!result.success);
    QCOMPARE(result.sourceId, 0);
    QCOMPARE(result.targetId, 1);
    QCOMPARE(result.recordsSynced, 0);
    QCOMPARE(svc.masterInfo(0).state, MultiMasterState::Active);
    QCOMPARE(svc.masterInfo(1).state, MultiMasterState::Active);
  }

  void testSynchronizeInvalidMasters() {
    MultiMasterService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &MultiMasterService::masterSyncCompleted);

    QVERIFY(!svc.synchronizeMasters(0, 99));
    QCOMPARE(spy.count(), 1);

    auto result = qvariant_cast<MmMasterSyncResult>(spy.at(0).at(0));
    QVERIFY(!result.success);
  }

  void testAllMasters() {
    MultiMasterService svc(nullptr, nullptr);
    MmMasterInfo a;
    a.adapterName = "eth0";
    svc.addMaster(a);
    MmMasterInfo b;
    b.adapterName = "eth1";
    svc.addMaster(b);

    auto all = svc.allMasters();
    QCOMPARE(all.size(), 2);
  }

  void testMasterInfo() {
    MultiMasterService svc(nullptr, nullptr);
    MmMasterInfo info;
    info.adapterName = "eth0";
    info.ipAddress = "192.168.1.1";
    info.macAddress = "AA:BB:CC:DD:EE:FF";
    info.slaveCount = 10;
    svc.addMaster(info);

    auto retrieved = svc.masterInfo(0);
    QCOMPARE(retrieved.adapterName, QString("eth0"));
    QCOMPARE(retrieved.ipAddress, QString("192.168.1.1"));
    QCOMPARE(retrieved.macAddress, QString("AA:BB:CC:DD:EE:FF"));
    QCOMPARE(retrieved.slaveCount, 10);
  }

  // ── Plugin Tests ───────────────────────────────────────────────────

  void testPluginIdentity() {
    MultiMasterService svc(nullptr, nullptr);
    MultiMasterPlugin plugin(&svc);

    QCOMPARE(plugin.id(), QString("multimaster"));
    QCOMPARE(plugin.displayName(), QString("Multi-Master"));
    QCOMPARE(plugin.displayNameZh(), QString("多主站"));
    QCOMPARE(plugin.defaultOrder(), 30);
    QCOMPARE(plugin.visible(), true);
  }

  void testWidgetCreation() {
    MultiMasterService svc(nullptr, nullptr);
    MultiMasterPlugin plugin(&svc);
    QVERIFY(plugin.widget() != nullptr);
  }

  void testPluginServiceAccessor() {
    MultiMasterService svc(nullptr, nullptr);
    MultiMasterPlugin plugin(&svc);
    QCOMPARE(plugin.service(), &svc);
  }

  void testInitialMasterCount() {
    MultiMasterService svc(nullptr, nullptr);
    MultiMasterPlugin plugin(&svc);
    QCOMPARE(plugin.masterCount(), 0);
  }

  void testSelectedMasterId() {
    MultiMasterService svc(nullptr, nullptr);
    MultiMasterPlugin plugin(&svc);
    QCOMPARE(plugin.selectedMasterId(), -1);
  }

  void testPluginWithMasters() {
    MultiMasterService svc(nullptr, nullptr);
    MmMasterInfo info;
    info.adapterName = "eth0";
    info.slaveCount = 3;
    svc.addMaster(info);

    MultiMasterPlugin plugin(&svc);
    QCOMPARE(plugin.masterCount(), 1);
  }

  // ── Comparison Widget Tests ────────────────────────────────────────

  void testComparisonWidgetCreation() {
    MasterComparisonWidget w;
    QCOMPARE(w.differenceCount(), 0);
  }

  void testComparisonSetMasters() {
    MasterComparisonWidget w;

    MmMasterInfo left;
    left.masterId = 0;
    left.adapterName = "eth0";
    left.slaveCount = 5;
    left.state = MultiMasterState::Active;
    left.ipAddress = "192.168.1.1";
    left.macAddress = "AA:BB:CC:DD:EE:01";

    MmMasterStatus leftStatus;
    leftStatus.masterId = 0;
    leftStatus.errorCount = 0;

    MmMasterInfo right;
    right.masterId = 1;
    right.adapterName = "eth1";
    right.slaveCount = 3;
    right.state = MultiMasterState::Idle;
    right.ipAddress = "192.168.1.2";
    right.macAddress = "AA:BB:CC:DD:EE:02";

    MmMasterStatus rightStatus;
    rightStatus.masterId = 1;
    rightStatus.errorCount = 2;

    w.setLeftMaster(left, leftStatus);
    w.setRightMaster(right, rightStatus);

    QVERIFY(w.differenceCount() > 0);
  }

  void testComparisonIdenticalMasters() {
    MasterComparisonWidget w;

    MmMasterInfo info;
    info.masterId = 0;
    info.adapterName = "eth0";
    info.slaveCount = 5;
    info.state = MultiMasterState::Active;
    info.ipAddress = "192.168.1.1";
    info.macAddress = "AA:BB:CC:DD:EE:01";

    MmMasterStatus status;
    status.masterId = 0;
    status.errorCount = 0;

    w.setLeftMaster(info, status);
    w.setRightMaster(info, status);

    QCOMPARE(w.differenceCount(), 0);
  }

  void testComparisonClear() {
    MasterComparisonWidget w;

    MmMasterInfo info;
    info.adapterName = "eth0";
    MmMasterStatus status;

    w.setLeftMaster(info, status);
    w.setRightMaster(info, status);
    w.clearComparison();
    QCOMPARE(w.differenceCount(), 0);
  }

  void testComparisonMergeSignal() {
    MasterComparisonWidget w;
    QSignalSpy spy(&w, &MasterComparisonWidget::mergeRequested);

    MmMasterInfo left;
    left.masterId = 0;
    left.adapterName = "eth0";
    MmMasterStatus ls;

    MmMasterInfo right;
    right.masterId = 1;
    right.adapterName = "eth1";
    MmMasterStatus rs;

    w.setLeftMaster(left, ls);
    w.setRightMaster(right, rs);

    QCOMPARE(spy.count(), 0);
  }

  // ── Signal Tests ───────────────────────────────────────────────────

  void testMasterDiscoveredSignal() {
    MultiMasterService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &MultiMasterService::masterDiscovered);

    MmMasterInfo info;
    info.adapterName = "eth0";
    svc.addMaster(info);

    QCOMPARE(spy.count(), 1);
    auto discovered = qvariant_cast<MmMasterInfo>(spy.at(0).at(0));
    QCOMPARE(discovered.adapterName, QString("eth0"));
  }

  void testMasterStatusChangedSignal() {
    MultiMasterService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &MultiMasterService::masterStatusChanged);

    MmMasterInfo info;
    info.adapterName = "eth0";
    info.state = MultiMasterState::Active;
    svc.addMaster(info);

    svc.monitorMaster(0);
    QCOMPARE(spy.count(), 1);
  }

  void testMasterSyncCompletedSignalCarriesOfflineFailure() {
    MultiMasterService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &MultiMasterService::masterSyncCompleted);

    MmMasterInfo a;
    a.adapterName = "eth0";
    svc.addMaster(a);

    MmMasterInfo b;
    b.adapterName = "eth1";
    svc.addMaster(b);

    svc.synchronizeMasters(0, 1);
    QCOMPARE(spy.count(), 1);
    auto result = qvariant_cast<MmMasterSyncResult>(spy.at(0).at(0));
    QVERIFY(!result.success);
    QVERIFY(result.message.contains(QStringLiteral("backend")));
  }

  void testMasterErrorSignal() {
    MultiMasterService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &MultiMasterService::masterError);

    QCOMPARE(spy.count(), 0);
  }
};

QTEST_MAIN(MultiMasterPluginTest)
#include "multimaster_plugin_test.moc"
