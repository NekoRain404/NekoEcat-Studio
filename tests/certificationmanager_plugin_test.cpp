// CertificationManagerPluginTest — Tests for CertificationManagerPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Certificate, status, and renewal tables
//   - Add/remove certificates
//   - Certificate renewal tracking

#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QLabel>
#include "plugins/certificationmanager/CertificationManagerPlugin.h"

class CertificationManagerPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, and default order
  void testPluginIdentity() {
    CertificationManagerPlugin plugin;

    QCOMPARE(plugin.id(), QString("certificationmanager"));
    QCOMPARE(plugin.displayName(), QString("Certification Manager"));
    QCOMPARE(plugin.displayNameZh(), QString("认证管理器"));
    QCOMPARE(plugin.defaultOrder(), 290);
    QCOMPARE(plugin.visible(), true);
  }

  // Verify widget is created
  void testWidgetCreation() {
    CertificationManagerPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify initial certificate and renewal counts
  void testInitialState() {
    CertificationManagerPlugin plugin;

    QCOMPARE(plugin.certificateCount(), 3);
    QCOMPARE(plugin.renewalCount(), 2);
  }

  // Verify certificate table structure
  void testCertificateTable() {
    CertificationManagerPlugin plugin;

    QTableWidget *table = plugin.certificateTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 7);
  }

  // Verify status table structure
  void testStatusTable() {
    CertificationManagerPlugin plugin;

    QTableWidget *table = plugin.statusTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->columnCount(), 4);
  }

  // Verify renewal table structure
  void testRenewalTable() {
    CertificationManagerPlugin plugin;

    QTableWidget *table = plugin.renewalTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 2);
    QCOMPARE(table->columnCount(), 4);
  }

  // Verify adding a certificate with signal
  void testAddCertificate() {
    CertificationManagerPlugin plugin;
    QSignalSpy spy(&plugin, &CertificationManagerPlugin::certificateAdded);
    int initial = plugin.certificateCount();

    CertificationManagerPlugin::Certificate c;
    c.id = "cert_new";
    c.name = "New Certificate";
    c.issuer = "Test Issuer";
    c.standard = "ISO 9001";
    c.issuedAt = QDateTime::currentDateTime();
    c.expiresAt = c.issuedAt.addYears(1);
    c.status = "Valid";
    c.serialNumber = "SN-NEW";

    plugin.addCertificate(c);
    QCOMPARE(plugin.certificateCount(), initial + 1);
    QCOMPARE(spy.count(), 1);
  }

  // Verify removing a certificate
  void testRemoveCertificate() {
    CertificationManagerPlugin plugin;
    int initial = plugin.certificateCount();

    plugin.removeCertificate(0);
    QCOMPARE(plugin.certificateCount(), initial - 1);
  }

  void testUpdateCertificate() {
    CertificationManagerPlugin plugin;

    CertificationManagerPlugin::Certificate c;
    c.id = "cert1";
    c.name = "Updated Certificate";
    c.issuer = "Updated Issuer";
    c.standard = "Updated Standard";
    c.issuedAt = QDateTime::currentDateTime();
    c.expiresAt = c.issuedAt.addYears(2);
    c.status = "Valid";
    c.serialNumber = "SN-UPDATED";

    plugin.updateCertificate(0, c);
    QCOMPARE(plugin.certificateTable()->item(0, 1)->text(),
             QString("Updated Certificate"));
  }

  void testIsExpired() {
    CertificationManagerPlugin plugin;

    QVERIFY(plugin.isExpired(2));
    QVERIFY(!plugin.isExpired(0));
  }

  void testIsExpiringSoon() {
    CertificationManagerPlugin plugin;

    QVERIFY(plugin.isExpiringSoon(1, 30));
    QVERIFY(!plugin.isExpiringSoon(0, 30));
  }

  void testValidateCertificate() {
    CertificationManagerPlugin plugin;
    QSignalSpy expiredSpy(&plugin,
                          &CertificationManagerPlugin::certificateExpired);

    plugin.validateCertificate(2);
    QCOMPARE(expiredSpy.count(), 1);
  }

  void testValidateExpiring() {
    CertificationManagerPlugin plugin;
    QSignalSpy renewalSpy(&plugin, &CertificationManagerPlugin::renewalDue);

    plugin.validateCertificate(1);
    QCOMPARE(renewalSpy.count(), 1);
  }

  void testValidateValid() {
    CertificationManagerPlugin plugin;

    plugin.validateCertificate(0);
    QCOMPARE(plugin.certificateTable()->item(0, 6)->text(),
             QString("Valid"));
  }

  void testCheckRenewals() {
    CertificationManagerPlugin plugin;

    plugin.checkRenewals();
    QVERIFY(plugin.renewalCount() >= 1);
  }

  void testStatusLabel() {
    CertificationManagerPlugin plugin;

    QLabel *label = plugin.statusLabel();
    QVERIFY(label != nullptr);
  }

  void testExportReport() {
    CertificationManagerPlugin plugin;

    QString path =
        QDir::temp().absoluteFilePath("certification_report_test.txt");
    plugin.exportReport(path);
    QVERIFY(QFile::exists(path));
    QFile::remove(path);
  }
};

QTEST_MAIN(CertificationManagerPluginTest)
#include "certificationmanager_plugin_test.moc"
