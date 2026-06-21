// EtherCATConfigServiceTest — Tests for EtherCATConfigService
//
// Test coverage:
//   - Profile management (set, save, load, delete)
//   - Profile validation (empty + valid)
//   - Parameter management (add, remove, update)
//   - Export and import of configuration profiles

#include <QTest>
#include "services/EtherCATConfigService.h"

class EtherCATConfigServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Default state has no profile or saved profiles
  void testDefaultState() {
    EtherCATConfigService svc;
    QVERIFY(svc.currentProfile().name.isEmpty());
    QVERIFY(svc.savedProfiles().isEmpty());
  }

  // Set current profile and verify name
  void testSetCurrentProfile() {
    EtherCATConfigService svc;
    ConfigProfile p;
    p.name = "test";
    p.description = "Test profile";
    svc.setCurrentProfile(p);
    QCOMPARE(svc.currentProfile().name, QStringLiteral("test"));
  }

  // Empty profile fails validation
  void testValidateEmptyProfile() {
    EtherCATConfigService svc;
    ConfigProfile p;
    auto result = svc.validateProfile(p);
    QVERIFY(!result.valid);
    QVERIFY(!result.errors.isEmpty());
  }

  // Valid profile with parameter passes validation
  void testValidateValidProfile() {
    EtherCATConfigService svc;
    ConfigProfile p;
    p.name = "valid";
    p.description = "A valid profile";
    ConfigParameter param;
    param.name = "cycle_time";
    param.value = "1000";
    p.parameters.append(param);
    auto result = svc.validateProfile(p);
    QVERIFY(result.valid);
    QVERIFY(result.errors.isEmpty());
  }

  // Save and load profile round-trip
  void testSaveLoadProfile() {
    EtherCATConfigService svc;
    ConfigProfile p;
    p.name = "saved";
    p.description = "To be saved";
    svc.setCurrentProfile(p);
    QVERIFY(svc.saveProfile("saved"));
    QCOMPARE(svc.savedProfiles().size(), 1);
    QVERIFY(svc.loadProfile("saved"));
    QCOMPARE(svc.currentProfile().name, QStringLiteral("saved"));
  }

  // Delete profile removes it from saved list
  void testDeleteProfile() {
    EtherCATConfigService svc;
    ConfigProfile p;
    p.name = "to_delete";
    svc.setCurrentProfile(p);
    svc.saveProfile("to_delete");
    QCOMPARE(svc.savedProfiles().size(), 1);
    QVERIFY(svc.deleteProfile("to_delete"));
    QCOMPARE(svc.savedProfiles().size(), 0);
  }

  // Add and remove parameter from current profile
  void testAddRemoveParameter() {
    EtherCATConfigService svc;
    ConfigParameter param;
    param.name = "cycle_time";
    param.value = "1000";
    svc.addParameter(param);
    QCOMPARE(svc.currentProfile().parameters.size(), 1);
    svc.removeParameter("cycle_time");
    QCOMPARE(svc.currentProfile().parameters.size(), 0);
  }

  // Update parameter value in current profile
  void testUpdateParameter() {
    EtherCATConfigService svc;
    ConfigParameter param;
    param.name = "cycle_time";
    param.value = "1000";
    svc.addParameter(param);
    svc.updateParameter("cycle_time", "2000");
    QCOMPARE(svc.currentProfile().parameters.first().value, QStringLiteral("2000"));
  }

  // Export and import profile round-trip
  void testExportImport() {
    EtherCATConfigService svc;
    QVERIFY(svc.exportProfile("test", "/tmp/test.ecatcfg"));
    QVERIFY(svc.importProfile("/tmp/test.ecatcfg"));
  }
};

QTEST_MAIN(EtherCATConfigServiceTest)
#include "ethercat_config_service_test.moc"
