// DomainServiceTest — Tests for DomainService
//
// Test coverage:
//   - Domain creation (single and multiple)
//   - PDO entry registration
//   - Invalid domain and parameter rejection
//   - Domain processing and data retrieval
//   - Domain info defaults

// DomainServiceTest — Tests for DomainService
//
// Test coverage:
//   - Domain creation (single and multiple)
//   - PDO entry registration (valid and invalid)
//   - Domain processing (empty, with entries, invalid)
//   - Domain data access
//   - Domain info defaults

#include <QTest>
#include <QSignalSpy>
#include "services/DomainService.h"

class DomainServiceTest : public QObject {
  Q_OBJECT
private slots:
  // Verify creating a domain returns index 0
  void testCreateDomain() {
    DomainService svc;
    int idx = svc.createDomain();
    QCOMPARE(idx, 0);
    QCOMPARE(svc.domains().size(), 1);
  }

  // Verify creating multiple domains returns sequential indices
  // Verify creating multiple domains returns sequential indices
  void testCreateMultipleDomains() {
    DomainService svc;
    int a = svc.createDomain();
    int b = svc.createDomain();
    QCOMPARE(a, 0);
    QCOMPARE(b, 1);
    QCOMPARE(svc.domains().size(), 2);
  }

  // Verify registering a PDO entry updates entry count and data size
  // Verify registering a PDO entry updates entry count and data size
  void testRegisterPdoEntry() {
    DomainService svc;
    int d = svc.createDomain();
    QVERIFY(svc.registerPdoEntry(d, 0, 0x6000, 1));
    DomainInfo info = svc.domainInfo(d);
    QCOMPARE(info.pdoEntryCount, 1);
    QCOMPARE(info.dataSize, 4);
  }

  // Verify registering multiple entries accumulates data size
  // Verify registering multiple entries accumulates count and size
  void testRegisterMultipleEntries() {
    DomainService svc;
    int d = svc.createDomain();
    svc.registerPdoEntry(d, 0, 0x6000, 1);
    svc.registerPdoEntry(d, 0, 0x6000, 2);
    DomainInfo info = svc.domainInfo(d);
    QCOMPARE(info.pdoEntryCount, 2);
    QCOMPARE(info.dataSize, 8);
  }

  // Verify registering on invalid domain emits error
  // Verify registering on invalid domain emits error signal
  void testRegisterOnInvalidDomain() {
    DomainService svc;
    QSignalSpy spy(&svc, &DomainService::error);
    QVERIFY(!svc.registerPdoEntry(99, 0, 0x6000, 1));
    QCOMPARE(spy.count(), 1);
  }

  // Verify registering with invalid params emits error
  // Verify registering with invalid params emits error signal
  void testRegisterInvalidParams() {
    DomainService svc;
    int d = svc.createDomain();
    QSignalSpy spy(&svc, &DomainService::error);
    QVERIFY(!svc.registerPdoEntry(d, -1, 0x6000, 1));
    QCOMPARE(spy.count(), 1);
  }

  // Verify processing a domain updates working counter
  // Verify processing a domain updates working counter
  void testProcessDomain() {
    DomainService svc;
    int d = svc.createDomain();
    svc.registerPdoEntry(d, 0, 0x6000, 1);
    QSignalSpy spy(&svc, &DomainService::domainProcessed);
    QVERIFY(svc.processDomain(d));
    QCOMPARE(spy.count(), 1);
    DomainInfo info = svc.domainInfo(d);
    QCOMPARE(info.workingCounter, 1);
  }

  // Verify processing an empty domain succeeds with zero counter
  // Verify processing an empty domain succeeds with zero counter
  void testProcessEmptyDomain() {
    DomainService svc;
    int d = svc.createDomain();
    QVERIFY(svc.processDomain(d));
    DomainInfo info = svc.domainInfo(d);
    QCOMPARE(info.workingCounter, 0);
  }

  // Verify processing an invalid domain fails
  // Verify processing an invalid domain returns false
  void testProcessInvalidDomain() {
    DomainService svc;
    QVERIFY(!svc.processDomain(99));
  }

  // Verify domainData returns correct size after processing
  // Verify domain data returns correct size after processing
  void testDomainData() {
    DomainService svc;
    int d = svc.createDomain();
    svc.registerPdoEntry(d, 0, 0x6000, 1);
    svc.processDomain(d);
    QByteArray data = svc.domainData(d);
    QCOMPARE(data.size(), 4);
  }

  // Verify domainData returns empty before processing
  // Verify domain data is empty before processing
  void testDomainDataBeforeProcess() {
    DomainService svc;
    int d = svc.createDomain();
    QByteArray data = svc.domainData(d);
    QCOMPARE(data.size(), 0);
  }

  // Verify domain info has correct defaults
  // Verify domain info has correct default values
  void testDomainInfoDefaults() {
    DomainService svc;
    int d = svc.createDomain();
    DomainInfo info = svc.domainInfo(d);
    QCOMPARE(info.domainIndex, d);
    QCOMPARE(info.pdoEntryCount, 0);
    QCOMPARE(info.dataSize, 0);
    QCOMPARE(info.workingCounter, 0);
  }
};

QTEST_MAIN(DomainServiceTest)
#include "domain_service_test.moc"
