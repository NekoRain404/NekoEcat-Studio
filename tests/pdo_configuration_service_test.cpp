#include <QCoreApplication>
#include <QTest>
#include <QSignalSpy>

#include "PdoConfigurationService.h"

class PdoConfigurationServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testConfigurePdoMapping() {
    PdoConfigurationService svc;
    PdoMappingConfig cfg;
    cfg.index = "0x6000";
    cfg.subIndex = "0x01";
    cfg.name = "ActualPosition";
    cfg.bitSize = 32;
    cfg.dataType = "INT32";
    cfg.direction = PdoConfigDirection::Input;

    QVERIFY(svc.configurePdoMapping(0, cfg));
    auto mappings = svc.pdoMappings(0);
    QCOMPARE(mappings.size(), 1);
    QCOMPARE(mappings[0].index, QStringLiteral("0x6000"));
    QCOMPARE(mappings[0].bitSize, 32);
  }

  void testConfigurePdoMappingInvalidIndex() {
    PdoConfigurationService svc;
    PdoMappingConfig cfg;
    cfg.index = "";
    cfg.bitSize = 16;

    QSignalSpy spy(&svc, &PdoConfigurationService::configurationError);
    QVERIFY(!svc.configurePdoMapping(0, cfg));
    QCOMPARE(spy.count(), 1);
  }

  void testConfigurePdoMappingInvalidBitSize() {
    PdoConfigurationService svc;
    PdoMappingConfig cfg;
    cfg.index = "0x6000";
    cfg.bitSize = 0;

    QSignalSpy spy(&svc, &PdoConfigurationService::configurationError);
    QVERIFY(!svc.configurePdoMapping(0, cfg));
    QCOMPARE(spy.count(), 1);
  }

  void testConfigurePdoMappingUpsert() {
    PdoConfigurationService svc;
    PdoMappingConfig cfg;
    cfg.index = "0x6000";
    cfg.subIndex = "0x01";
    cfg.bitSize = 16;
    cfg.dataType = "INT16";

    svc.configurePdoMapping(0, cfg);
    cfg.bitSize = 32;
    cfg.dataType = "INT32";
    svc.configurePdoMapping(0, cfg);

    auto mappings = svc.pdoMappings(0);
    QCOMPARE(mappings.size(), 1);
    QCOMPARE(mappings[0].bitSize, 32);
  }

  void testConfigurePdoAssignment() {
    PdoConfigurationService svc;
    PdoAssignmentConfig cfg;
    cfg.smIndex = 3;
    cfg.pdoIndices = {"0x6000", "0x6001"};

    QVERIFY(svc.configurePdoAssignment(0, cfg));
    auto assignments = svc.pdoAssignments(0);
    QCOMPARE(assignments.size(), 1);
    QCOMPARE(assignments[0].smIndex, 3);
    QCOMPARE(assignments[0].pdoIndices.size(), 2);
  }

  void testConfigurePdoAssignmentInvalidSm() {
    PdoConfigurationService svc;
    PdoAssignmentConfig cfg;
    cfg.smIndex = 9;
    cfg.pdoIndices = {"0x6000"};

    QSignalSpy spy(&svc, &PdoConfigurationService::configurationError);
    QVERIFY(!svc.configurePdoAssignment(0, cfg));
    QCOMPARE(spy.count(), 1);
  }

  void testConfigurePdoAssignmentEmptyIndices() {
    PdoConfigurationService svc;
    PdoAssignmentConfig cfg;
    cfg.smIndex = 3;

    QSignalSpy spy(&svc, &PdoConfigurationService::configurationError);
    QVERIFY(!svc.configurePdoAssignment(0, cfg));
    QCOMPARE(spy.count(), 1);
  }

  void testConfigureSyncManager() {
    PdoConfigurationService svc;
    PdoSyncManagerConfig cfg;
    cfg.smIndex = 3;
    cfg.direction = PdoConfigDirection::Input;
    cfg.startAddress = 0x1000;
    cfg.length = 128;
    cfg.enable = true;

    QVERIFY(svc.configureSyncManager(0, cfg));
    auto sms = svc.syncManagers(0);
    QCOMPARE(sms.size(), 1);
    QCOMPARE(sms[0].smIndex, 3);
    QCOMPARE(sms[0].length, 128);
  }

  void testConfigureSyncManagerInvalidIndex() {
    PdoConfigurationService svc;
    PdoSyncManagerConfig cfg;
    cfg.smIndex = 8;

    QSignalSpy spy(&svc, &PdoConfigurationService::configurationError);
    QVERIFY(!svc.configureSyncManager(0, cfg));
    QCOMPARE(spy.count(), 1);
  }

  void testConfigureDcSync() {
    PdoConfigurationService svc;
    DcSyncConfig cfg;
    cfg.assignActivate = true;
    cfg.sync0CycleTime = 1000000;

    QVERIFY(svc.configureDcSync(0, cfg));
    auto dc = svc.dcSyncConfig(0);
    QVERIFY(dc.assignActivate);
    QCOMPARE(dc.sync0CycleTime, 1000000);
  }

  void testApplyConfiguration() {
    PdoConfigurationService svc;
    PdoMappingConfig mapping;
    mapping.index = "0x6000";
    mapping.subIndex = "0x01";
    mapping.bitSize = 32;
    mapping.dataType = "INT32";

    svc.configurePdoMapping(0, mapping);

    QSignalSpy spy(&svc, &PdoConfigurationService::configurationApplied);
    QSignalSpy errorSpy(&svc, &PdoConfigurationService::configurationError);
    QVERIFY(!svc.applyConfiguration(0));
    QCOMPARE(spy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
  }

  void testApplyConfigurationDoesNotReportHardwareSuccessOffline() {
    PdoConfigurationService svc;
    PdoMappingConfig mapping;
    mapping.index = "0x6000";
    mapping.subIndex = "0x01";
    mapping.bitSize = 32;
    mapping.dataType = "INT32";
    QVERIFY(svc.configurePdoMapping(0, mapping));

    QSignalSpy appliedSpy(&svc, &PdoConfigurationService::configurationApplied);
    QSignalSpy errorSpy(&svc, &PdoConfigurationService::configurationError);
    QVERIFY(!svc.applyConfiguration(0));
    QCOMPARE(appliedSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);

    auto status = svc.configurationStatus(0);
    QVERIFY(!status.lastApplied.isValid());
    QVERIFY(!status.lastError.isEmpty());
  }

  void testApplyConfigurationNoMapping() {
    PdoConfigurationService svc;

    QSignalSpy spy(&svc, &PdoConfigurationService::configurationError);
    QVERIFY(!svc.applyConfiguration(0));
    QCOMPARE(spy.count(), 1);
  }

  void testConfigurationStatus() {
    PdoConfigurationService svc;
    PdoMappingConfig mapping;
    mapping.index = "0x6000";
    mapping.subIndex = "0x01";
    mapping.bitSize = 16;

    svc.configurePdoMapping(0, mapping);

    PdoAssignmentConfig assignment;
    assignment.smIndex = 3;
    assignment.pdoIndices = {"0x6000"};
    svc.configurePdoAssignment(0, assignment);

    PdoSyncManagerConfig sm;
    sm.smIndex = 3;
    svc.configureSyncManager(0, sm);

    DcSyncConfig dc;
    dc.assignActivate = true;
    svc.configureDcSync(0, dc);

    auto status = svc.configurationStatus(0);
    QVERIFY(status.mappingConfigured);
    QVERIFY(status.assignmentConfigured);
    QVERIFY(status.syncManagerConfigured);
    QVERIFY(status.dcSyncConfigured);
  }

  void testMultiplePositions() {
    PdoConfigurationService svc;

    PdoMappingConfig m0;
    m0.index = "0x6000";
    m0.bitSize = 16;
    svc.configurePdoMapping(0, m0);

    PdoMappingConfig m1;
    m1.index = "0x6040";
    m1.bitSize = 16;
    svc.configurePdoMapping(1, m1);

    QCOMPARE(svc.pdoMappings(0).size(), 1);
    QCOMPARE(svc.pdoMappings(1).size(), 1);
    QCOMPARE(svc.pdoMappings(0)[0].index, QStringLiteral("0x6000"));
    QCOMPARE(svc.pdoMappings(1)[0].index, QStringLiteral("0x6040"));
  }

  void testApplyConfigurationTimestamp() {
    PdoConfigurationService svc;
    PdoMappingConfig mapping;
    mapping.index = "0x6000";
    mapping.bitSize = 16;
    svc.configurePdoMapping(0, mapping);

    QVERIFY(!svc.applyConfiguration(0));
    auto status = svc.configurationStatus(0);
    QVERIFY(!status.lastApplied.isValid());
    QVERIFY(!status.lastError.isEmpty());
  }
};

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  PdoConfigurationServiceTest t;
  return QTest::qExec(&t, argc, argv);
}

#include "pdo_configuration_service_test.moc"
