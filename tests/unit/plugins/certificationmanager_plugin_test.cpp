// CertificationManagerPluginTest — Tests for CertificationManagerPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Certificate, status, and renewal tables
//   - Add/remove certificates
//   - Certificate records fail closed without certification backend

#include "plugins/certificationmanager/CertificationManagerPlugin.h"
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTest>

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
        QCOMPARE(plugin.visible(), false);
    }

    // Verify widget is created
    void testWidgetCreation() {
        CertificationManagerPlugin plugin;
        QVERIFY(plugin.widget() != nullptr);
    }

    // Verify initial certificate and renewal counts
    void testInitialState() {
        CertificationManagerPlugin plugin;

        QCOMPARE(plugin.certificateCount(), 0);
        QCOMPARE(plugin.renewalCount(), 0);
    }

    // Verify certificate table structure
    void testCertificateTable() {
        CertificationManagerPlugin plugin;

        QTableWidget* table = plugin.certificateTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 0);
        QCOMPARE(table->columnCount(), 7);
    }

    // Verify status table structure
    void testStatusTable() {
        CertificationManagerPlugin plugin;

        QTableWidget* table = plugin.statusTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 0);
        QCOMPARE(table->columnCount(), 4);
    }

    // Verify renewal table structure
    void testRenewalTable() {
        CertificationManagerPlugin plugin;

        QTableWidget* table = plugin.renewalTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 0);
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
        c.status = "Unverified";
        c.serialNumber = "SN-NEW";

        plugin.addCertificate(c);
        QCOMPARE(plugin.certificateCount(), initial + 1);
        QCOMPARE(spy.count(), 1);
    }

    // Verify removing a certificate
    void testRemoveCertificate() {
        CertificationManagerPlugin plugin;
        CertificationManagerPlugin::Certificate c;
        c.id = "remove";
        c.name = "Remove Certificate";
        c.status = "Unverified";
        plugin.addCertificate(c);
        int initial = plugin.certificateCount();

        plugin.removeCertificate(0);
        QCOMPARE(plugin.certificateCount(), initial - 1);
    }

    void testUpdateCertificate() {
        CertificationManagerPlugin plugin;
        CertificationManagerPlugin::Certificate existing;
        existing.id = "cert1";
        existing.name = "Existing Certificate";
        existing.status = "Unverified";
        plugin.addCertificate(existing);

        CertificationManagerPlugin::Certificate c;
        c.id = "cert1";
        c.name = "Updated Certificate";
        c.issuer = "Updated Issuer";
        c.standard = "Updated Standard";
        c.issuedAt = QDateTime::currentDateTime();
        c.status = "Unverified";
        c.serialNumber = "SN-UPDATED";

        plugin.updateCertificate(0, c);
        QCOMPARE(plugin.certificateTable()->item(0, 1)->text(), QString("Updated Certificate"));
    }

    void testIsExpired() {
        CertificationManagerPlugin plugin;

        CertificationManagerPlugin::Certificate c;
        c.id = "expired";
        c.name = "Expired Certificate";
        c.expiresAt = QDateTime::currentDateTime().addDays(-1);
        c.status = "Unverified";
        plugin.addCertificate(c);
        QVERIFY(plugin.isExpired(0));
    }

    void testIsExpiringSoon() {
        CertificationManagerPlugin plugin;

        CertificationManagerPlugin::Certificate c;
        c.id = "soon";
        c.name = "Soon Certificate";
        c.expiresAt = QDateTime::currentDateTime().addDays(15);
        c.status = "Unverified";
        plugin.addCertificate(c);
        QVERIFY(plugin.isExpiringSoon(0, 30));
    }

    void testValidateCertificate() {
        CertificationManagerPlugin plugin;
        QSignalSpy expiredSpy(&plugin, &CertificationManagerPlugin::certificateExpired);

        CertificationManagerPlugin::Certificate c;
        c.id = "expired";
        c.name = "Expired Certificate";
        c.expiresAt = QDateTime::currentDateTime().addDays(-1);
        c.status = "Unverified";
        plugin.addCertificate(c);
        plugin.validateCertificate(0);
        QCOMPARE(expiredSpy.count(), 1);
    }

    void testValidateExpiring() {
        CertificationManagerPlugin plugin;
        QSignalSpy renewalSpy(&plugin, &CertificationManagerPlugin::renewalDue);

        CertificationManagerPlugin::Certificate c;
        c.id = "soon";
        c.name = "Soon Certificate";
        c.expiresAt = QDateTime::currentDateTime().addDays(15);
        c.status = "Unverified";
        plugin.addCertificate(c);
        plugin.validateCertificate(0);
        QCOMPARE(renewalSpy.count(), 1);
    }

    void testValidateDoesNotSynthesizeValid() {
        CertificationManagerPlugin plugin;
        CertificationManagerPlugin::Certificate c;
        c.id = "cert1";
        c.name = "Certificate";
        c.expiresAt = QDateTime::currentDateTime().addDays(365);
        c.status = "Unverified";
        plugin.addCertificate(c);

        plugin.validateCertificate(0);
        QCOMPARE(plugin.certificateTable()->item(0, 6)->text(), QString("Unverified"));
    }

    void testCheckRenewals() {
        CertificationManagerPlugin plugin;

        plugin.checkRenewals();
        QCOMPARE(plugin.renewalCount(), 0);
    }

    void testAddButtonDoesNotMintValidCertificate() {
        CertificationManagerPlugin plugin;
        auto buttons = plugin.widget()->findChildren<QPushButton*>();
        QPushButton* addButton = nullptr;
        for (auto* button : buttons) {
            if (button->text() == QStringLiteral("Add Certificate")) {
                addButton = button;
                break;
            }
        }
        QVERIFY(addButton != nullptr);
        addButton->click();
        QCOMPARE(plugin.certificateCount(), 1);
        QCOMPARE(plugin.certificateTable()->item(0, 6)->text(), QString("Unverified"));
        QVERIFY(plugin.certificateTable()->item(0, 5)->text().isEmpty());
    }

    void testStatusLabel() {
        CertificationManagerPlugin plugin;

        QLabel* label = plugin.statusLabel();
        QVERIFY(label != nullptr);
    }

    void testExportReport() {
        CertificationManagerPlugin plugin;
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        CertificationManagerPlugin::Certificate c;
        c.id = "cert_export";
        c.name = "Export Certificate";
        c.issuer = "Export Issuer";
        c.standard = "IEC 61508";
        c.issuedAt = QDateTime::currentDateTime();
        c.expiresAt = QDateTime::currentDateTime().addDays(10);
        c.status = "Unverified";
        c.serialNumber = "SN-EXPORT";
        plugin.addCertificate(c);
        plugin.checkRenewals();

        const QString path = dir.filePath("certification_report_test.txt");
        QVERIFY(plugin.exportReport(path));
        QVERIFY(QFile::exists(path));

        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        const QString text = QString::fromUtf8(file.readAll());
        QVERIFY(text.contains(QStringLiteral("Certification Report\n")));
        QVERIFY(text.contains(QStringLiteral("Total Certificates: 1\n")));
        QVERIFY(text.contains(QStringLiteral("Export Certificate [IEC 61508] - Unverified\n")));
        QVERIFY(text.contains(QStringLiteral("Serial: SN-EXPORT\n")));
        QVERIFY(text.contains(QStringLiteral("Export Certificate - ")));

        QTest::failOnWarning(QRegularExpression(QStringLiteral("QFSFileEngine::open: No file name specified")));
        QVERIFY(!plugin.exportReport(QString()));
        QVERIFY(!plugin.exportReport(dir.path()));
    }

    void testSourceDoesNotMintSyntheticCertificates() {
        QFile file(QStringLiteral(SOURCE_ROOT
                                  "/apps/ecat-studio/plugins/certificationmanager/CertificationManagerPlugin.cpp"));
        QVERIFY2(file.open(QIODevice::ReadOnly | QIODevice::Text), qPrintable(file.errorString()));
        const QString source = QString::fromUtf8(file.readAll());

        QVERIFY2(!source.contains(QStringLiteral("c.expiresAt = c.issuedAt.addYears(1)")),
                 "Certification manager UI must not mint default certificate expiry dates");
        QVERIFY2(!source.contains(QStringLiteral("c.status = \"Valid\"")),
                 "Certification manager UI must not mint valid certificates");
        QVERIFY2(!source.contains(QStringLiteral("cert.status = \"Valid\"")),
                 "Certification validation must not report valid without a backend");
    }
};

QTEST_MAIN(CertificationManagerPluginTest)
#include "certificationmanager_plugin_test.moc"
