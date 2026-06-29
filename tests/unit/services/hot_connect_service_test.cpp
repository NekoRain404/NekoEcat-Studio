// HotConnectServiceTest — Tests for HotConnectService
//
// Test coverage:
//   - Initial state (empty groups)
//   - Group creation and removal
//   - Group activation/deactivation fail closed without a live backend
//   - Group history is not synthesized from offline activation attempts
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
  // Verify activation cannot be simulated without a live backend.
  void testActivateGroupFailsClosedWithoutBackend() {
    HotConnectService svc;
    QSignalSpy activatedSpy(&svc, &HotConnectService::groupActivated);
    QSignalSpy stateSpy(&svc, &HotConnectService::groupStateChanged);
    int id = svc.createGroup("G1", {0});
    QVERIFY(!svc.activateGroup(id));
    QCOMPARE(activatedSpy.count(), 0);
    QCOMPARE(stateSpy.count(), 0);
    QVERIFY(!svc.isGroupActive(id));
    QCOMPARE(svc.activeGroupCount(), 0);
    QCOMPARE(svc.groupHistory(id).size(), 0);
  }

  // Verify deactivation cannot be simulated without a live backend.
  void testDeactivateGroupFailsClosedWithoutBackend() {
    HotConnectService svc;
    QSignalSpy deactivatedSpy(&svc, &HotConnectService::groupDeactivated);
    QSignalSpy stateSpy(&svc, &HotConnectService::groupStateChanged);
    int id = svc.createGroup("G1", {0});
    svc.activateGroup(id);
    QVERIFY(!svc.deactivateGroup(id));
    QCOMPARE(deactivatedSpy.count(), 0);
    QCOMPARE(stateSpy.count(), 0);
    QVERIFY(!svc.isGroupActive(id));
    QCOMPARE(svc.activeGroupCount(), 0);
  }

  // Verify repeated activation attempts still fail closed.
  void testActivateRepeatedAttemptFailsClosed() {
    HotConnectService svc;
    int id = svc.createGroup("G1", {0});
    QVERIFY(!svc.activateGroup(id));
    QVERIFY(!svc.activateGroup(id));
  }

  // Verify deactivating an inactive group fails closed without a backend.
  void testDeactivateAlreadyInactiveFailsClosed() {
    HotConnectService svc;
    int id = svc.createGroup("G1", {0});
    QVERIFY(!svc.deactivateGroup(id));
  }

  // Verify offline activation/deactivation attempts do not synthesize history.
  void testGroupHistoryNotSynthesizedOffline() {
    HotConnectService svc;
    int id = svc.createGroup("G1", {0});
    svc.activateGroup(id);
    svc.deactivateGroup(id);
    auto history = svc.groupHistory(id);
    QCOMPARE(history.size(), 0);
  }

  // Verify offline activation does not emit a state change signal.
  void testGroupStateChangedSignalNotEmittedOffline() {
    HotConnectService svc;
    QSignalSpy spy(&svc, &HotConnectService::groupStateChanged);
    int id = svc.createGroup("G1", {0});
    svc.activateGroup(id);
    QCOMPARE(spy.count(), 0);
  }

  // Removing a group remains a local draft operation.
  void testRemoveGroupAfterFailedActivation() {
    HotConnectService svc;
    int id = svc.createGroup("G1", {0});
    svc.activateGroup(id);
    QVERIFY(svc.removeGroup(id));
    QCOMPARE(svc.activeGroupCount(), 0);
  }
};

QTEST_MAIN(HotConnectServiceTest)
#include "hot_connect_service_test.moc"
