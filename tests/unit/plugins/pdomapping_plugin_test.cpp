// PdoMappingEditorPluginTest — Tests for PDO Mapping Editor Plugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Canvas sync manager setup
//   - PDO tree structure
//   - Add/remove PDO entries
//   - Validation
//   - Export/import
//   - Property table
//   - Validator logic
#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QLabel>
#include "plugins/pdomapping/PdoMappingEditorPlugin.h"
#include "plugins/pdomapping/PdoMappingCanvas.h"
#include "plugins/pdomapping/PdoMappingValidator.h"
#include "services/PdoMappingService.h"

class PdoMappingEditorPluginTest : public QObject {
  Q_OBJECT
private slots:
  void testPluginIdentity() {
    PdoMappingEditorPlugin plugin(nullptr);
    QCOMPARE(plugin.id(), QString("pdomapping"));
    QCOMPARE(plugin.displayName(), QString("PDO Mapping"));
    QCOMPARE(plugin.displayNameZh(), QString("PDO 映射"));
    QCOMPARE(plugin.defaultOrder(), 25);
    QCOMPARE(plugin.visible(), true);
  }

  void testWidgetCreation() {
    PdoMappingEditorPlugin plugin(nullptr);
    QVERIFY(plugin.widget() != nullptr);
  }

  void testCanvasExists() {
    PdoMappingEditorPlugin plugin(nullptr);
    plugin.widget();
    QVERIFY(plugin.canvas() != nullptr);
  }

  void testCanvasInitialSyncManagers() {
    PdoMappingEditorPlugin plugin(nullptr);
    plugin.widget();
    auto sms = plugin.canvas()->syncManagers();
    QCOMPARE(sms.size(), 4);
    QCOMPARE(sms[0].index, 0);
    QCOMPARE(sms[2].index, 2);
    QCOMPARE(sms[3].index, 3);
  }

  void testCanvasSampleDataEntries() {
    PdoMappingEditorPlugin plugin(nullptr);
    plugin.widget();
    auto sms = plugin.canvas()->syncManagers();
    QCOMPARE(sms[2].entries.size(), 4);
    QCOMPARE(sms[3].entries.size(), 4);
    QCOMPARE(sms[2].entries[0].index, QString("0x6040"));
    QCOMPARE(sms[2].entries[0].name, QString("Control Word"));
    QCOMPARE(sms[3].entries[0].name, QString("Status Word"));
  }

  void testPdoTreeCreation() {
    PdoMappingEditorPlugin plugin(nullptr);
    plugin.widget();
    QTreeWidget *tree = plugin.pdoTree();
    QVERIFY(tree != nullptr);
    QCOMPARE(tree->topLevelItemCount(), 4);
  }

  void testPdoTreeSmItemCount() {
    PdoMappingEditorPlugin plugin(nullptr);
    plugin.widget();
    QTreeWidget *tree = plugin.pdoTree();
    QCOMPARE(tree->topLevelItem(2)->childCount(), 4);
    QCOMPARE(tree->topLevelItem(3)->childCount(), 4);
  }

  void testAddPdoEntry() {
    PdoMappingEditorPlugin plugin(nullptr);
    plugin.widget();
    QSignalSpy spy(&plugin, &PdoMappingEditorPlugin::mappingChanged);
    int initialCount = plugin.pdoEntryCount(2);

    plugin.addPdoEntry(2, "0x6060", "0x00", "Modes of Operation", "INT8", 8, false);
    QCOMPARE(plugin.pdoEntryCount(2), initialCount + 1);
    QCOMPARE(spy.count(), 1);
  }

  void testRemovePdoEntry() {
    PdoMappingEditorPlugin plugin(nullptr);
    plugin.widget();
    int initialCount = plugin.pdoEntryCount(2);

    plugin.removePdoEntry(2, 0);
    QCOMPARE(plugin.pdoEntryCount(2), initialCount - 1);
  }

  void testPropertyTable() {
    PdoMappingEditorPlugin plugin(nullptr);
    plugin.widget();
    QTableWidget *table = plugin.propertyTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 2);
  }

  void testValidationPanel() {
    PdoMappingEditorPlugin plugin(nullptr);
    plugin.widget();
    QTextEdit *panel = plugin.validationPanel();
    QVERIFY(panel != nullptr);
    QVERIFY(panel->isReadOnly());
  }

  void testValidateMapping() {
    PdoMappingEditorPlugin plugin(nullptr);
    plugin.widget();
    QSignalSpy spy(&plugin, &PdoMappingEditorPlugin::validationCompleted);

    plugin.validateMapping();
    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    bool valid = args.at(0).toBool();
    int errors = args.at(1).toInt();
    QVERIFY(valid);
    QCOMPARE(errors, 0);
  }

  void testValidateWithOversizeEntry() {
    PdoMappingEditorPlugin plugin(nullptr);
    plugin.widget();
    plugin.addPdoEntry(2, "0x7000", "0x00", "Huge Entry", "OCTET_STRING", 1500 * 8 + 1, false);
    plugin.validateMapping();
    QVERIFY(plugin.hasErrors());
    QVERIFY(plugin.errorCount() > 0);
  }

  void testSlavePosition() {
    PdoMappingEditorPlugin plugin(nullptr);
    QCOMPARE(plugin.slavePosition(), 0);
    plugin.setSlavePosition(2);
    QCOMPARE(plugin.slavePosition(), 2);
  }

  void testCanvasSelection() {
    PdoMappingCanvas canvas;
    QVector<SyncManagerBlock> sms;
    SyncManagerBlock sm;
    sm.index = 0;
    sm.name = "Test";
    sm.direction = PdoEntryDirection::Input;
    PdoCanvasEntry entry;
    entry.index = "0x1000";
    entry.subIndex = "0x00";
    entry.name = "Device Type";
    entry.dataType = "UINT32";
    entry.bitSize = 32;
    entry.direction = PdoEntryDirection::Input;
    sm.entries.append(entry);
    sms.append(sm);
    canvas.setSyncManagers(sms);

    canvas.setSelectedEntry(0, 0);
    QCOMPARE(canvas.selectedSmIndex(), 0);
    QCOMPARE(canvas.selectedEntryIndex(), 0);

    canvas.clearSelection();
    QCOMPARE(canvas.selectedSmIndex(), -1);
    QCOMPARE(canvas.selectedEntryIndex(), -1);
  }

  void testCanvasSizeHint() {
    PdoMappingCanvas canvas;
    QSize hint = canvas.sizeHint();
    QVERIFY(hint.width() >= 600);
    QVERIFY(hint.height() >= 400);
  }

  void testCanvasErrorHighlight() {
    PdoMappingCanvas canvas;
    QVector<SyncManagerBlock> sms;
    SyncManagerBlock sm;
    sm.index = 0;
    sm.name = "Test";
    sm.direction = PdoEntryDirection::Input;
    PdoCanvasEntry entry;
    entry.index = "0x1000";
    entry.subIndex = "0x00";
    entry.name = "Test Entry";
    entry.dataType = "UINT16";
    entry.bitSize = 16;
    entry.direction = PdoEntryDirection::Input;
    sm.entries.append(entry);
    sms.append(sm);
    canvas.setSyncManagers(sms);

    canvas.setErrorHighlight(0, 0, true);
    QVERIFY(canvas.syncManagers()[0].entries[0].hasError);

    canvas.clearAllErrors();
    QVERIFY(!canvas.syncManagers()[0].entries[0].hasError);
  }

  void testValidatorPass() {
    QVector<SyncManagerBlock> sms;
    SyncManagerBlock sm2;
    sm2.index = 2;
    sm2.direction = PdoEntryDirection::Output;
    PdoCanvasEntry e1;
    e1.index = "0x6040";
    e1.subIndex = "0x00";
    e1.name = "Control Word";
    e1.bitSize = 16;
    e1.direction = PdoEntryDirection::Output;
    sm2.entries.append(e1);
    sms.append(sm2);

    SyncManagerBlock sm3;
    sm3.index = 3;
    sm3.direction = PdoEntryDirection::Input;
    PdoCanvasEntry e2;
    e2.index = "0x6041";
    e2.subIndex = "0x00";
    e2.name = "Status Word";
    e2.bitSize = 16;
    e2.direction = PdoEntryDirection::Input;
    sm3.entries.append(e2);
    sms.append(sm3);

    auto report = PdoMappingValidator::validate(sms);
    QVERIFY(report.valid);
    QCOMPARE(report.errors.size(), 0);
    QCOMPARE(report.totalInputBits, 16);
    QCOMPARE(report.totalOutputBits, 16);
  }

  void testValidatorDuplicateEntry() {
    QVector<SyncManagerBlock> sms;
    SyncManagerBlock sm;
    sm.index = 2;
    sm.direction = PdoEntryDirection::Output;
    PdoCanvasEntry e1;
    e1.index = "0x6040";
    e1.subIndex = "0x00";
    e1.name = "Entry A";
    e1.bitSize = 16;
    e1.direction = PdoEntryDirection::Output;
    PdoCanvasEntry e2;
    e2.index = "0x6040";
    e2.subIndex = "0x00";
    e2.name = "Entry B";
    e2.bitSize = 16;
    e2.direction = PdoEntryDirection::Output;
    sm.entries.append(e1);
    sm.entries.append(e2);
    sms.append(sm);

    auto report = PdoMappingValidator::validate(sms);
    QVERIFY(!report.valid);
    QVERIFY(report.errors.size() >= 1);
    QCOMPARE(report.errors[0].type, PdoValidationError::Type::DuplicateEntry);
  }

  void testValidatorSizeExceeded() {
    QVector<SyncManagerBlock> sms;
    SyncManagerBlock sm;
    sm.index = 2;
    sm.direction = PdoEntryDirection::Output;
    PdoCanvasEntry e1;
    e1.index = "0x6040";
    e1.subIndex = "0x00";
    e1.name = "Huge";
    e1.bitSize = 1500 * 8 + 1;
    e1.direction = PdoEntryDirection::Output;
    sm.entries.append(e1);
    sms.append(sm);

    auto report = PdoMappingValidator::validate(sms);
    QVERIFY(!report.valid);
    bool foundSizeError = false;
    for (const auto &err : report.errors) {
      if (err.type == PdoValidationError::Type::SizeExceeded) {
        foundSizeError = true;
        break;
      }
    }
    QVERIFY(foundSizeError);
  }

  void testValidatorMissingInput() {
    QVector<SyncManagerBlock> sms;
    SyncManagerBlock sm;
    sm.index = 2;
    sm.direction = PdoEntryDirection::Output;
    PdoCanvasEntry e1;
    e1.index = "0x6040";
    e1.subIndex = "0x00";
    e1.name = "Control Word";
    e1.bitSize = 16;
    e1.direction = PdoEntryDirection::Output;
    sm.entries.append(e1);
    sms.append(sm);

    auto report = PdoMappingValidator::validate(sms);
    QVERIFY(!report.valid);
    bool foundMissing = false;
    for (const auto &err : report.errors) {
      if (err.type == PdoValidationError::Type::MissingRequired) {
        foundMissing = true;
        break;
      }
    }
    QVERIFY(foundMissing);
  }

  void testMappingLayoutMethods() {
    PdoMappingService service;

    PdoMapping m1;
    m1.index = "0x6040";
    m1.subIndex = "0x00";
    m1.name = "Control Word";
    m1.dataType = "UINT16";
    m1.bitSize = 16;
    m1.direction = PdoDirection::Output;
    m1.slavePosition = 0;
    service.configureMapping(0, m1);

    PdoMapping m2;
    m2.index = "0x6041";
    m2.subIndex = "0x00";
    m2.name = "Status Word";
    m2.dataType = "UINT16";
    m2.bitSize = 16;
    m2.direction = PdoDirection::Input;
    m2.slavePosition = 0;
    service.configureMapping(0, m2);

    MappingLayout layout = service.getMappingLayout(0);
    QCOMPARE(layout.syncManagers.size(), 2);
    QVERIFY(layout.totalSize > 0);

    auto vr = service.validateMappingLayout(layout);
    QVERIFY(vr.valid);
  }

  void testApplyMappingLayout() {
    PdoMappingService service;
    QSignalSpy spy(&service, &PdoMappingService::mappingLayoutChanged);

    MappingLayout layout;
    SyncManagerLayout sm;
    sm.smIndex = 2;
    sm.direction = PdoDirection::Output;
    PdoEntryLayout entry;
    entry.index = "0x6040";
    entry.subIndex = "0x00";
    entry.name = "Control Word";
    entry.dataType = "UINT16";
    entry.bitSize = 16;
    entry.direction = PdoDirection::Output;
    entry.enabled = true;
    sm.pdoEntries.append(entry);
    layout.syncManagers.append(sm);
    layout.totalSize = 16;

    bool ok = service.applyMappingLayout(0, layout);
    QVERIFY(ok);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(service.currentMappings(0).size(), 1);
  }
};

QTEST_MAIN(PdoMappingEditorPluginTest)
#include "pdomapping_plugin_test.moc"
