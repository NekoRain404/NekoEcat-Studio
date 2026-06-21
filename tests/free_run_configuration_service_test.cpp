// FreeRunConfigurationServiceTest — Tests for FreeRunConfigurationService
//
// Test coverage:
//   - Default state
//   - Process data configuration
//   - Cycle time configuration
//   - Data mapping configuration
//   - Error handling configuration
//   - Apply configuration
//   - Signal emission

#include <QTest>
#include <QSignalSpy>
#include "services/FreeRunConfigurationService.h"

class FreeRunConfigurationServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testDefaultState() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    QVERIFY(!svc.isApplied());
    QCOMPARE(svc.processDataConfig().cycleTimeUs, 1000);
    QCOMPARE(svc.processDataConfig().watchdogTimeoutMs, 5000);
    QCOMPARE(svc.processDataConfig().syncMode, QStringLiteral("DC"));
  }

  void testConfigureProcessData() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &FreeRunConfigurationService::configurationChanged);
    ProcessDataConfig config;
    config.inputs = {0x6000, 0x6010};
    config.outputs = {0x7000, 0x7010};
    config.cycleTimeUs = 500;
    QVERIFY(svc.configureProcessData(config));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.processDataConfig().inputs.size(), 2);
    QCOMPARE(svc.processDataConfig().cycleTimeUs, 500);
  }

  void testConfigureProcessDataEmptyFails() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    ProcessDataConfig config;
    QVERIFY(!svc.configureProcessData(config));
  }

  void testConfigureProcessDataInvalidCycleTime() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    ProcessDataConfig config;
    config.inputs = {0x6000};
    config.cycleTimeUs = -1;
    QVERIFY(!svc.configureProcessData(config));
  }

  void testConfigureCycleTime() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &FreeRunConfigurationService::configurationChanged);
    QVERIFY(svc.configureCycleTime(2000));
    QCOMPARE(svc.processDataConfig().cycleTimeUs, 2000);
    QCOMPARE(spy.count(), 1);
  }

  void testConfigureCycleTimeInvalid() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    QVERIFY(!svc.configureCycleTime(0));
    QVERIFY(!svc.configureCycleTime(-100));
  }

  void testConfigureDataMapping() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &FreeRunConfigurationService::configurationChanged);
    DataMappingConfig config;
    config.inputOffsets = {0, 8, 16};
    config.outputOffsets = {0, 4};
    config.inputSizes = {8, 8, 4};
    config.outputSizes = {4, 4};
    config.autoMap = false;
    QVERIFY(svc.configureDataMapping(config));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.dataMappingConfig().inputOffsets.size(), 3);
  }

  void testConfigureDataMappingAutoMap() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    DataMappingConfig config;
    config.autoMap = true;
    QVERIFY(svc.configureDataMapping(config));
  }

  void testConfigureDataMappingEmptyFails() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    DataMappingConfig config;
    config.autoMap = false;
    QVERIFY(!svc.configureDataMapping(config));
  }

  void testConfigureErrorHandling() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &FreeRunConfigurationService::configurationChanged);
    ErrorHandlingConfig config;
    config.maxRetries = 5;
    config.retryDelayMs = 200;
    config.haltOnError = true;
    QVERIFY(svc.configureErrorHandling(config));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.errorHandlingConfig().maxRetries, 5);
    QVERIFY(svc.errorHandlingConfig().haltOnError);
  }

  void testConfigureErrorHandlingInvalid() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    ErrorHandlingConfig config;
    config.maxRetries = -1;
    QVERIFY(!svc.configureErrorHandling(config));
  }

  void testApplyConfiguration() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    QSignalSpy spy(&svc, &FreeRunConfigurationService::configurationApplied);
    ProcessDataConfig pdConfig;
    pdConfig.inputs = {0x6000};
    svc.configureProcessData(pdConfig);
    QVERIFY(svc.applyConfiguration());
    QVERIFY(svc.isApplied());
    QCOMPARE(spy.count(), 1);
  }

  void testApplyWithoutConfigFails() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    QVERIFY(!svc.applyConfiguration());
  }

  void testConfigChangeResetsApplied() {
    FreeRunConfigurationService svc(nullptr, nullptr);
    ProcessDataConfig pdConfig;
    pdConfig.inputs = {0x6000};
    svc.configureProcessData(pdConfig);
    svc.applyConfiguration();
    QVERIFY(svc.isApplied());

    svc.configureCycleTime(2000);
    QVERIFY(!svc.isApplied());
  }
};

QTEST_MAIN(FreeRunConfigurationServiceTest)
#include "free_run_configuration_service_test.moc"
