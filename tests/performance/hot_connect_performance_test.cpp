#include <QTest>
#include <QElapsedTimer>
#include "services/HotConnectService.h"

class HotConnectPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testCreateGroupPerformance() {
    HotConnectService svc;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.createGroup(QString("Group%1").arg(i), {i, i + 1, i + 2});
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);
    QCOMPARE(svc.allGroups().size(), 1000);
  }

  void testActivateDeactivatePerformance() {
    HotConnectService svc;
    QVector<int> groups;
    for (int i = 0; i < 100; i++) {
      groups.append(svc.createGroup(QString("Group%1").arg(i), {i}));
    }
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.activateGroup(groups[i % 100]);
      svc.deactivateGroup(groups[i % 100]);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
  }

  void testGroupQueryPerformance() {
    HotConnectService svc;
    for (int i = 0; i < 1000; i++) {
      svc.createGroup(QString("Group%1").arg(i), {i});
    }
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 10000; i++) {
      svc.groupInfo(i % 1000);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 100);
  }

  void testHistoryPerformance() {
    HotConnectService svc;
    int groupId = svc.createGroup("TestGroup", {0, 1, 2});
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 1000; i++) {
      svc.activateGroup(groupId);
      svc.deactivateGroup(groupId);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 500);
    QCOMPARE(svc.groupHistory(groupId).size(), 500);
  }
};

QTEST_MAIN(HotConnectPerformanceTest)
#include "hot_connect_performance_test.moc"
