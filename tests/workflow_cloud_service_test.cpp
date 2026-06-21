#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowCloudService.h"

class WorkflowCloudServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testConnectToCloud() {
    WorkflowCloudService svc;
    QSignalSpy spy(&svc, &WorkflowCloudService::cloudConnected);
    WfCloudConfig config;
    config.endpoint = "https://cloud.example.com";
    config.apiKey = "key123";
    QVERIFY(svc.connectToCloud(config));
    QVERIFY(svc.isConnected());
    QCOMPARE(spy.count(), 1);
  }

  void testConnectEmptyEndpoint() {
    WorkflowCloudService svc;
    WfCloudConfig config;
    config.endpoint = "";
    config.apiKey = "key123";
    QVERIFY(!svc.connectToCloud(config));
    QVERIFY(!svc.isConnected());
  }

  void testConnectEmptyApiKey() {
    WorkflowCloudService svc;
    WfCloudConfig config;
    config.endpoint = "https://cloud.example.com";
    config.apiKey = "";
    QVERIFY(!svc.connectToCloud(config));
  }

  void testSyncToCloud() {
    WorkflowCloudService svc;
    WfCloudConfig config;
    config.endpoint = "https://cloud.example.com";
    config.apiKey = "key123";
    svc.connectToCloud(config);
    QSignalSpy spy(&svc, &WorkflowCloudService::cloudSynced);
    QVERIFY(svc.syncToCloud());
    QCOMPARE(svc.status().recordCount, 10);
    QCOMPARE(spy.count(), 1);
  }

  void testSyncNotConnected() {
    WorkflowCloudService svc;
    QVERIFY(!svc.syncToCloud());
  }

  void testBackupToCloud() {
    WorkflowCloudService svc;
    WfCloudConfig config;
    config.endpoint = "https://cloud.example.com";
    config.apiKey = "key123";
    svc.connectToCloud(config);
    QSignalSpy spy(&svc, &WorkflowCloudService::cloudBackupCompleted);
    QVERIFY(svc.backupToCloud());
    QCOMPARE(spy.count(), 1);
  }

  void testBackupNotConnected() {
    WorkflowCloudService svc;
    QVERIFY(!svc.backupToCloud());
  }

  void testMonitorCloud() {
    WorkflowCloudService svc;
    WfCloudConfig config;
    config.endpoint = "https://cloud.example.com";
    config.apiKey = "key123";
    svc.connectToCloud(config);
    auto status = svc.monitorCloud();
    QVERIFY(status.connected);
    QCOMPARE(status.recordCount, 0);
  }

  void testMultipleSyncs() {
    WorkflowCloudService svc;
    WfCloudConfig config;
    config.endpoint = "https://cloud.example.com";
    config.apiKey = "key123";
    svc.connectToCloud(config);
    svc.syncToCloud();
    svc.syncToCloud();
    svc.syncToCloud();
    QCOMPARE(svc.status().recordCount, 30);
  }
};

QTEST_MAIN(WorkflowCloudServiceTest)
#include "workflow_cloud_service_test.moc"
