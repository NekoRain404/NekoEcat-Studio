#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include "services/PdoMappingService.h"

class PdoMappingServiceTest : public QObject {
  Q_OBJECT

private slots:
  void testConfigureMapping() {
    PdoMappingService svc;
    QSignalSpy spy(&svc, &PdoMappingService::mappingChanged);

    PdoMapping mapping;
    mapping.index = "0x6000";
    mapping.subIndex = "0x01";
    mapping.name = "Actual Position";
    mapping.dataType = "INT32";
    mapping.bitSize = 32;
    mapping.slavePosition = 0;

    QVERIFY(svc.configureMapping(0, mapping));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.currentMappings(0).size(), 1);
  }

  void testRejectInvalidMappingAddressAndPosition() {
    PdoMappingService svc;
    QSignalSpy errorSpy(&svc, &PdoMappingService::error);
    QSignalSpy changedSpy(&svc, &PdoMappingService::mappingChanged);

    PdoMapping mapping;
    mapping.index = "6000";
    mapping.subIndex = "0x01";
    mapping.bitSize = 16;
    mapping.slavePosition = 0;
    QVERIFY(!svc.configureMapping(0, mapping));

    mapping.index = "0x6000";
    mapping.subIndex = "1";
    QVERIFY(!svc.configureMapping(0, mapping));

    mapping.subIndex = "0x01";
    mapping.slavePosition = 1;
    QVERIFY(!svc.configureMapping(0, mapping));

    mapping.slavePosition = -1;
    QVERIFY(!svc.configureMapping(-1, mapping));

    QCOMPARE(svc.currentMappings(0).size(), 0);
    QCOMPARE(svc.currentMappings(-1).size(), 0);
    QCOMPARE(changedSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 4);
  }

  void testRejectImportWithInvalidMappings() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.filePath("invalid.json");

    QJsonObject invalid;
    invalid[QStringLiteral("index")] = QStringLiteral("6000");
    invalid[QStringLiteral("subIndex")] = QStringLiteral("0x01");
    invalid[QStringLiteral("bitSize")] = 16;

    QJsonObject root;
    root[QStringLiteral("position")] = 0;
    root[QStringLiteral("mappings")] = QJsonArray{invalid};

    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly));
    QVERIFY(file.write(QJsonDocument(root).toJson()) > 0);
    file.close();

    PdoMappingService svc;
    QSignalSpy errorSpy(&svc, &PdoMappingService::error);
    QVERIFY(!svc.importMapping(0, path));
    QCOMPARE(svc.currentMappings(0).size(), 0);
    QCOMPARE(errorSpy.count(), 1);
  }

  void testRejectInvalidLayoutEntries() {
    PdoMappingService svc;
    QSignalSpy errorSpy(&svc, &PdoMappingService::error);
    QSignalSpy layoutSpy(&svc, &PdoMappingService::mappingLayoutChanged);

    MappingLayout layout;
    SyncManagerLayout sm;
    sm.smIndex = 3;
    sm.size = 16;

    PdoEntryLayout entry;
    entry.index = "0x6000";
    entry.subIndex = "0x01";
    entry.bitSize = 0;
    sm.pdoEntries.append(entry);
    layout.syncManagers.append(sm);
    layout.totalSize = 16;

    QVERIFY(!svc.applyMappingLayout(0, layout));
    QCOMPARE(svc.currentMappings(0).size(), 0);
    QCOMPARE(layoutSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
  }
};

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  PdoMappingServiceTest t;
  return QTest::qExec(&t, argc, argv);
}

#include "pdo_mapping_service_test.moc"
