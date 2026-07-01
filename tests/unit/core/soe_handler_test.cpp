// Tests for SoEHandler — IDN validation and parameter checking.
//
// These tests exercise validation logic that does not require live hardware.
// Actual soe_read/soe_write invocations shell out to the ethercat CLI, which
// is covered by integration tests against real slaves.

#include <QTest>
#include <QJsonObject>

#include "handlers/SoEHandler.h"

class SoeHandlerTest : public QObject {
    Q_OBJECT
private slots:
    // String-form IDNs (S-x-yyyy / P-x-yyyy) are accepted.
    void testValidStringIdn() {
        SoEHandler h;
        QString err;
        QVERIFY(h.validateIdn("P-0-0150", &err));
        QVERIFY(h.validateIdn("S-0-1000", &err));
        QVERIFY(h.validateIdn("p-7-99999", &err));
    }

    // Numeric IDNs (decimal and hex) are accepted.
    void testValidNumericIdn() {
        SoEHandler h;
        QString err;
        QVERIFY(h.validateIdn("336", &err));
        QVERIFY(h.validateIdn("0x0150", &err));
    }

    // Malformed IDNs are rejected.
    void testInvalidIdn() {
        SoEHandler h;
        QString err;
        QVERIFY(!h.validateIdn("", &err));
        QVERIFY(!h.validateIdn("X-0-100", &err));
        QVERIFY(!h.validateIdn("P-9-100", &err));   // param set out of range
        QVERIFY(!h.validateIdn("notanidn", &err));
    }

    // Read with missing position fails.
    void testReadMissingPosition() {
        SoEHandler h;
        QJsonObject resp = h.handleSoeRead("1", QJsonObject{{"idn", "P-0-0150"}});
        QVERIFY(!resp.value("ok").toBool());
    }

    // Read with invalid IDN fails.
    void testReadInvalidIdn() {
        SoEHandler h;
        QJsonObject resp = h.handleSoeRead("1", QJsonObject{
            {"position", 0}, {"idn", "bad-idn"}});
        QVERIFY(!resp.value("ok").toBool());
    }

    // Write with missing value fails.
    void testWriteMissingValue() {
        SoEHandler h;
        QJsonObject resp = h.handleSoeWrite("1", QJsonObject{
            {"position", 0}, {"idn", "P-0-0150"}});
        QVERIFY(!resp.value("ok").toBool());
    }

    // Drive number out of range fails.
    void testInvalidDrive() {
        SoEHandler h;
        QJsonObject resp = h.handleSoeRead("1", QJsonObject{
            {"position", 0}, {"idn", "P-0-0150"}, {"drive", 9}});
        QVERIFY(!resp.value("ok").toBool());
    }
};

QTEST_MAIN(SoeHandlerTest)
#include "soe_handler_test.moc"
