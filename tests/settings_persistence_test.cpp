// Unit tests for AppSettings and MasterProfile — settings persistence.
#include "SettingsDialog.h"

#include <QtTest/QtTest>

class SettingsPersistenceTest : public QObject {
    Q_OBJECT

private slots:
    void masterProfileDefaults();
    void appSettingsDefaults();
    void masterProfileFields();
    void appSettingsMultipleMasters();
};

void SettingsPersistenceTest::masterProfileDefaults()
{
    MasterProfile p;
    QCOMPARE(p.name, QString("Master 0"));
    QCOMPARE(p.target, QString("0"));
}

void SettingsPersistenceTest::appSettingsDefaults()
{
    AppSettings s;
    QCOMPARE(s.theme, QString("Dark"));
    QCOMPARE(s.language, QString("English"));
    QCOMPARE(s.scale, 1.0);
    QCOMPARE(s.masters.size(), 1);
    QCOMPARE(s.activeMaster, QString("0"));
}

void SettingsPersistenceTest::masterProfileFields()
{
    MasterProfile p;
    p.name = "Custom Master";
    p.target = "3";
    QCOMPARE(p.name, QString("Custom Master"));
    QCOMPARE(p.target, QString("3"));
}

void SettingsPersistenceTest::appSettingsMultipleMasters()
{
    AppSettings s;
    MasterProfile p1;
    p1.name = "Master A";
    p1.target = "0";
    MasterProfile p2;
    p2.name = "Master B";
    p2.target = "1";
    s.masters = {p1, p2};
    s.activeMaster = "1";
    QCOMPARE(s.masters.size(), 2);
    QCOMPARE(s.masters[0].name, QString("Master A"));
    QCOMPARE(s.masters[1].name, QString("Master B"));
    QCOMPARE(s.activeMaster, QString("1"));
}

QTEST_MAIN(SettingsPersistenceTest)
#include "settings_persistence_test.moc"
