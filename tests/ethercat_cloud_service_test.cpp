// EtherCATCloudServiceTest — Tests for EtherCATCloudService
//
// Test coverage:
//   - Cloud connection (valid, invalid config)
//   - Sync and backup operations (connected + not connected)
//   - Cloud monitoring and config persistence
//   - Signal emission for connection, sync, and backup events

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

  // Connect with valid config and verify signal
  void testConnectToCloud() {
    EtherCATCloudService svc(nullptr);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    cfg.region = QStringLiteral("us-east-1");
    QSignalSpy spy(&svc, &EtherCATCloudService::cloudConnected);
    bool ok = svc.connectToCloud(cfg);
    QVERIFY(ok);
    QVERIFY(svc.isConnected());
    QCOMPARE(spy.count(), 1);
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

  // Sync data to cloud and verify record count
  void testSyncToCloud() {
    EtherCATCloudService svc(nullptr);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);
    QSignalSpy spy(&svc, &EtherCATCloudService::cloudSynced);
    bool ok = svc.syncToCloud();
    QVERIFY(ok);
    QVERIFY(svc.status().recordCount > 0);
    QCOMPARE(spy.count(), 1);
  }

  // Sync fails when not connected
  void testSyncToCloudNotConnected() {
    EtherCATCloudService svc(nullptr);
    QVERIFY(!svc.syncToCloud());
  }

  // Backup to cloud and verify signal
  void testBackupToCloud() {
    EtherCATCloudService svc(nullptr);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);
    QSignalSpy spy(&svc, &EtherCATCloudService::cloudBackupCompleted);
    bool ok = svc.backupToCloud();
    QVERIFY(ok);
    QCOMPARE(spy.count(), 1);
  }

  // Backup fails when not connected
  void testBackupToCloudNotConnected() {
    EtherCATCloudService svc(nullptr);
    QVERIFY(!svc.backupToCloud());
  }

  // Monitor cloud reflects connection state
  void testMonitorCloud() {
    EtherCATCloudService svc(nullptr);
    CloudStatus st = svc.monitorCloud();
    QVERIFY(!st.connected);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);
    CloudStatus st2 = svc.monitorCloud();
    QVERIFY(st2.connected);
  }

  // Config persists after connection
  void testConfig() {
    EtherCATCloudService svc(nullptr);
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    cfg.region = QStringLiteral("eu-west-1");
    cfg.bucket = QStringLiteral("ethercat-data");
    svc.connectToCloud(cfg);
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
    QCOMPARE(spy.count(), 1);
  }

  // cloudSynced signal carries record count
  void testCloudSyncedSignal() {
    EtherCATCloudService svc(nullptr);
    QSignalSpy spy(&svc, &EtherCATCloudService::cloudSynced);
    QVERIFY(spy.isValid());
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);
    svc.syncToCloud();
    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QVERIFY(args.at(0).toInt() > 0);
  }

  // cloudBackupCompleted signal carries success flag
  void testCloudBackupCompletedSignal() {
    EtherCATCloudService svc(nullptr);
    QSignalSpy spy(&svc, &EtherCATCloudService::cloudBackupCompleted);
    QVERIFY(spy.isValid());
    CloudConfig cfg;
    cfg.endpoint = QStringLiteral("https://cloud.example.com");
    cfg.apiKey = QStringLiteral("key123");
    svc.connectToCloud(cfg);
    svc.backupToCloud();
    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QVERIFY(args.at(0).toBool());
  }
};

QTEST_MAIN(EtherCATCloudServiceTest)
#include "ethercat_cloud_service_test.moc"
