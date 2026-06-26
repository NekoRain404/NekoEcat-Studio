// EtherCATCloudServiceTest — Tests for EtherCATCloudService
//
// Test coverage:
//   - Cloud connection, sync, and backup fail closed without a cloud backend
//   - Rejected cloud requests do not emit synthetic connection, sync, or backup signals
//   - Cloud monitoring stays disconnected while config can be retained for later backend use

#include <QTest>
#include <QSignalSpy>
#include "services/EtherCATCloudService.h"

class EtherCATCloudServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Initial state is disconnected with zero counts
  void testInitialState() {
    EtherCATCloudService svc(nullptr);
    QVERIFY(!svc.isConnected());
    CloudStatus st = svc.status();
    QVERIFY(!st.connected);
    QVERIFY(!st.syncing);
    QVERIFY(!st.backing);
    QCOMPARE(st.lastSyncTime, 0);
    QCOMPARE(st.lastBackupTime, 0);
    QCOMPARE(st.recordCount, 0);
    QVERIFY(st.error.isEmpty());
  }

  // Connect with valid config fails closed without backend
  void testConnectToCloud() {
    EtherCATCloudService svc(nullptr);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    cfg.region = QStringLiteral("us-east-1");
    QSignalSpy spy(&svc, &EtherCATCloudService::cloudConnected);
    bool ok = svc.connectToCloud(cfg);
    QVERIFY(!ok);
    QVERIFY(!svc.isConnected());
    QCOMPARE(spy.count(), 0);
  }

  // Connect with invalid config (missing endpoint or key)
  void testConnectToCloudInvalid() {
    EtherCATCloudService svc(nullptr);
    CloudConfig cfg;
    cfg.endpoint = QString();
    cfg.apiKey = QStringLiteral("key123");
    QVERIFY(!svc.connectToCloud(cfg));
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QString();
    QVERIFY(!svc.connectToCloud(cfg));
  }

  // Sync does not synthesize uploaded record counts without backend
  void testSyncToCloud() {
    EtherCATCloudService svc(nullptr);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);
    QSignalSpy spy(&svc, &EtherCATCloudService::cloudSynced);
    bool ok = svc.syncToCloud();
    QVERIFY(!ok);
    QCOMPARE(svc.status().recordCount, 0);
    QCOMPARE(svc.status().lastSyncTime, 0);
    QCOMPARE(spy.count(), 0);
  }

  // Sync fails when not connected
  void testSyncToCloudNotConnected() {
    EtherCATCloudService svc(nullptr);
    QVERIFY(!svc.syncToCloud());
  }

  // Backup does not synthesize success without backend
  void testBackupToCloud() {
    EtherCATCloudService svc(nullptr);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);
    QSignalSpy spy(&svc, &EtherCATCloudService::cloudBackupCompleted);
    bool ok = svc.backupToCloud();
    QVERIFY(!ok);
    QCOMPARE(svc.status().lastBackupTime, 0);
    QCOMPARE(spy.count(), 0);
  }

  // Backup fails when not connected
  void testBackupToCloudNotConnected() {
    EtherCATCloudService svc(nullptr);
    QVERIFY(!svc.backupToCloud());
  }

  // Monitor cloud remains disconnected without backend
  void testMonitorCloud() {
    EtherCATCloudService svc(nullptr);
    CloudStatus st = svc.monitorCloud();
    QVERIFY(!st.connected);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);
    CloudStatus st2 = svc.monitorCloud();
    QVERIFY(!st2.connected);
  }

  // Config persists after connection
  void testConfig() {
    EtherCATCloudService svc(nullptr);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    cfg.region = QStringLiteral("eu-west-1");
    cfg.bucket = QStringLiteral("ethercat-data");
    QVERIFY(!svc.connectToCloud(cfg));
    CloudConfig stored = svc.config();
    QCOMPARE(stored.endpoint, cfg.endpoint);
    QCOMPARE(stored.apiKey, cfg.apiKey);
    QCOMPARE(stored.region, cfg.region);
    QCOMPARE(stored.bucket, cfg.bucket);
  }

  // cloudConnected signal fires on connect
  void testCloudConnectedSignal() {
    EtherCATCloudService svc(nullptr);
    QSignalSpy spy(&svc, &EtherCATCloudService::cloudConnected);
    QVERIFY(spy.isValid());
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);
    QCOMPARE(spy.count(), 0);
  }

  // cloudSynced signal is not emitted without backend
  void testCloudSyncedSignal() {
    EtherCATCloudService svc(nullptr);
    QSignalSpy spy(&svc, &EtherCATCloudService::cloudSynced);
    QVERIFY(spy.isValid());
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);
    svc.syncToCloud();
    QCOMPARE(spy.count(), 0);
  }

  // cloudBackupCompleted signal is not emitted without backend
  void testCloudBackupCompletedSignal() {
    EtherCATCloudService svc(nullptr);
    QSignalSpy spy(&svc, &EtherCATCloudService::cloudBackupCompleted);
    QVERIFY(spy.isValid());
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);
    svc.backupToCloud();
    QCOMPARE(spy.count(), 0);
  }
};

QTEST_MAIN(EtherCATCloudServiceTest)
#include "ethercat_cloud_service_test.moc"
