// HotConnectServiceTest — Tests for HotConnectService
//
// Test coverage:
//   - Initial state (empty groups)
//   - Group creation and removal
//   - Group activation and deactivation with signals
//   - Redundant activate/deactivate calls
//   - Group history tracking
//   - Group state change signal
//   - Removing an active group

#include <QTest>
#include <QSignalSpy>
#include "services/HotConnectService.h"

class HotConnectServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify empty initial state
  void testInitialState() {
    HotConnectService svc;
    QCOMPARE(svc.allGroups().size(), 0);
    QCOMPARE(svc.activeGroupCount(), 0);
  }
  // Test creating a group with slave positions
  void testCreateGroup() {
    HotConnectService svc;
    int id = svc.createGroup("TestGroup", {0, 1, 2});
    QVERIFY(id > 0);
    QCOMPARE(svc.allGroups().size(), 1);
    auto info = svc.groupInfo(id);
    QCOMPARE(info.name, QString("TestGroup"));
    QCOMPARE(info.slavePositions.size(), 3);
    QCOMPARE(info.state, HotConnectGroupState::Inactive);
  }
  // Test removing an existing group
  void testRemoveGroup() {
    HotConnectService svc;
    int id = svc.createGroup("G1", {0});
    QVERIFY(svc.removeGroup(id));
    QCOMPARE(svc.allGroups().size(), 0);
  }
  // Verify removing a nonexistent group fails
  void testRemoveNonexistent() {
    HotConnectService svc;
    QVERIFY(!svc.removeGroup(99));
  }
  // Test activating a group and verifying signal
  void testActivateGroup() {
    HotConnectService svc;
    QSignalSpy spy(&svc, &HotConnectService::groupActivated);
    int id = svc.createGroup("G1", {0});
    QVERIFY(svc.activateGroup(id));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), id);
    QVERIFY(svc.isGroupActive(id));
    QCOMPARE(svc.activeGroupCount(), 1);
  }
  // Test deactivating a group and verifying signal
  void testDeactivateGroup() {
    HotConnectService svc;
    QSignalSpy spy(&svc, &HotConnectService::groupDeactivated);
    int id = svc.createGroup("G1", {0});
    svc.activateGroup(id);
    QVERIFY(svc.deactivateGroup(id));
    QCOMPARE(spy.count(), 1);
    QVERIFY(!svc.isGroupActive(id));
    QCOMPARE(svc.activeGroupCount(), 0);
  }
  // Verify activating an already-active group succeeds
  void testActivateAlreadyActive() {
    HotConnectService svc;
    int id = svc.createGroup("G1", {0});
    svc.activateGroup(id);
    QVERIFY(svc.activateGroup(id));
  }
  // Verify deactivating an already-inactive group succeeds
  void testDeactivateAlreadyInactive() {
    HotConnectService svc;
    int id = svc.createGroup("G1", {0});
    QVERIFY(svc.deactivateGroup(id));
  }
  // Test group history records
  void testGroupHistory() {
    HotConnectService svc;
    int id = svc.createGroup("G1", {0});
    svc.activateGroup(id);
    svc.deactivateGroup(id);
    auto history = svc.groupHistory(id);
    QCOMPARE(history.size(), 2);
    QVERIFY(history[0].success);
    QVERIFY(history[1].success);
  }
  // Verify groupStateChanged signal with correct state
  void testGroupStateChangedSignal() {
    HotConnectService svc;
    QSignalSpy spy(&svc, &HotConnectService::groupStateChanged);
    int id = svc.createGroup("G1", {0});
    svc.activateGroup(id);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(1).value<HotConnectGroupState>(),
             HotConnectGroupState::Active);
  }
  // Test removing an active group deactivates it
  void testRemoveActiveGroup() {
    HotConnectService svc;
    int id = svc.createGroup("G1", {0});
    svc.activateGroup(id);
    QVERIFY(svc.removeGroup(id));
    QCOMPARE(svc.activeGroupCount(), 0);
  }
};

QTEST_MAIN(HotConnectServiceTest)
#include "hot_connect_service_test.moc"
