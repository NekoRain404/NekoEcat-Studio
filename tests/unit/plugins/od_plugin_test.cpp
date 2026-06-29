// OdPluginTest — Tests for OdPlugin
//
// Test coverage:
//   - Plugin identity (id, display names, order, visibility)
//   - SDO table and filter accessors
//   - SDO inspector widget accessors
//   - Object bookmark table and detail label
//   - SDO history table
//   - SDO target panel and trail updates
//   - SDO type combo values

#include <QTest>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include "plugins/od/OdPlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class OdPluginTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  ServiceContainer *container_ = nullptr;

private slots:
  void initTestCase() {
    client_ = new EcatClient(this);
    container_ = new ServiceContainer(client_, new EventBus(this), this);
  }

  // Verify plugin id, display names
  void testIdentity() {
    OdPlugin p(container_);
    QCOMPARE(p.id(), QString("od"));
    QCOMPARE(p.displayName(), QString("Object Dictionary"));
    QCOMPARE(p.displayNameZh(), QString("对象字典"));
  }

  // Plugin has expected default order
  // Verify default order is 20
  void testDefaultOrder() {
    OdPlugin p(container_);
    QCOMPARE(p.defaultOrder(), 20);
  }

  // Plugin is visible by default
  // Verify plugin is visible
  void testVisible() {
    OdPlugin p(container_);
    QVERIFY(p.visible());
  }

  // Widget is created and not null
  // Check widget is created
  void testWidgetNotNull() {
    OdPlugin p(container_);
    QVERIFY(p.widget() != nullptr);
  }

  // SDO table has correct column count
  // Check SDO table has 9 columns
  void testSdoTableNotNull() {
    OdPlugin p(container_);
    QVERIFY(p.sdoTable() != nullptr);
    QCOMPARE(p.sdoTable()->columnCount(), 9);
  }

  // SDO filter line edit is created
  // Check SDO filter is created
  void testSdoFilterNotNull() {
    OdPlugin p(container_);
    QVERIFY(p.sdoFilter() != nullptr);
  }

  // SDO object filter combo is created
  // Check SDO object filter is created
  void testSdoObjectFilterNotNull() {
    OdPlugin p(container_);
    QVERIFY(p.sdoObjectFilter() != nullptr);
  }

  // SDO access filter combo is created
  // Check SDO access filter is created
  void testSdoAccessFilterNotNull() {
    OdPlugin p(container_);
    QVERIFY(p.sdoAccessFilter() != nullptr);
  }

  // SDO summary label is created
  // Check SDO summary label is created
  void testSdoSummaryLabelNotNull() {
    OdPlugin p(container_);
    QVERIFY(p.sdoSummaryLabel() != nullptr);
  }

  // All SDO inspector widgets are created
  // Check all SDO inspector widget accessors
  void testSdoInspectorWidgets() {
    OdPlugin p(container_);
    QVERIFY(p.sdoIndex() != nullptr);
    QVERIFY(p.sdoSubIndex() != nullptr);
    QVERIFY(p.sdoType() != nullptr);
    QVERIFY(p.sdoValue() != nullptr);
    QVERIFY(p.sdoWriteValue() != nullptr);
    QVERIFY(p.sdoInspectorLabel() != nullptr);
    QVERIFY(p.sdoTargetTable() != nullptr);
    QVERIFY(p.useSdoValueButton() != nullptr);
  }

  // SDO target table has correct column count
  // Verify SDO target table has 3 columns
  void testSdoTargetTableColumnCount() {
    OdPlugin p(container_);
    QCOMPARE(p.sdoTargetTable()->columnCount(), 3);
  }

  // SDO target trail table has correct column count
  // Check SDO target trail table has 9 columns
  void testSdoTargetTrailTableNotNull() {
    OdPlugin p(container_);
    QVERIFY(p.sdoTargetTrailTable() != nullptr);
    QCOMPARE(p.sdoTargetTrailTable()->columnCount(), 9);
  }

  // SDO target trail detail label is created
  // Check SDO target trail detail label is created
  void testSdoTargetTrailDetailLabelNotNull() {
    OdPlugin p(container_);
    QVERIFY(p.sdoTargetTrailDetailLabel() != nullptr);
  }

  // Object bookmark table has correct column count
  // Check object bookmark table has 10 columns
  void testObjectBookmarkTableNotNull() {
    OdPlugin p(container_);
    QVERIFY(p.objectBookmarkTable() != nullptr);
    QCOMPARE(p.objectBookmarkTable()->columnCount(), 10);
  }

  // Object bookmark detail label is created
  // Check object bookmark detail label is created
  void testObjectBookmarkDetailLabelNotNull() {
    OdPlugin p(container_);
    QVERIFY(p.objectBookmarkDetailLabel() != nullptr);
  }

  // SDO history table has correct column count
  // Check SDO history table has 9 columns
  void testSdoHistoryTableNotNull() {
    OdPlugin p(container_);
    QVERIFY(p.sdoHistoryTable() != nullptr);
    QCOMPARE(p.sdoHistoryTable()->columnCount(), 9);
  }

  // Update SDO table summary populates label
  // Test updating SDO table summary label
  void testUpdateSdoTableSummary() {
    OdPlugin p(container_);
    p.updateSdoTableSummary(100, 50, 30, 5, 40);
    QVERIFY(p.sdoSummaryLabel() != nullptr);
    QVERIFY(!p.sdoSummaryLabel()->text().isEmpty());
  }

  // Update SDO inspector label sets text and severity
  // Test updating SDO inspector label
  void testUpdateSdoInspectorLabel() {
    OdPlugin p(container_);
    p.updateSdoInspectorLabel("Ready - Master localhost", "ready");
    QCOMPARE(p.sdoInspectorLabel()->text(),
             QString("Ready - Master localhost"));
  }

  // Update SDO target trail row detail sets label text
  // Test updating SDO target trail row detail
  void testUpdateSdoTargetTrailRowDetail() {
    OdPlugin p(container_);
    p.updateSdoTargetTrailRowDetail("No selection", "info", "tooltip");
    QCOMPARE(p.sdoTargetTrailDetailLabel()->text(),
             QString("No selection"));
  }

  // Update object bookmark row detail sets label text
  // Test updating object bookmark row detail
  void testUpdateObjectBookmarkRowDetail() {
    OdPlugin p(container_);
    p.updateObjectBookmarkRowDetail("No selection", "info", "tooltip");
    QCOMPARE(p.objectBookmarkDetailLabel()->text(),
             QString("No selection"));
  }

  // SDO type combo has expected type values
  // Verify SDO type combo has expected values
  void testSdoTypeComboValues() {
    OdPlugin p(container_);
    QVERIFY(p.sdoType()->count() > 0);
    QVERIFY(p.sdoType()->findText("uint16") >= 0);
    QVERIFY(p.sdoType()->findText("int32") >= 0);
    QVERIFY(p.sdoType()->findText("bool") >= 0);
  }

  // Update SDO target panel rows populates table
  // Test updating SDO target panel rows
  void testUpdateSdoTargetPanelRows() {
    OdPlugin p(container_);
    QList<QPair<QString, QString>> rows;
    rows << qMakePair(QString("Target"), QString("#1 0x6040:0x00"));
    rows << qMakePair(QString("Read Value"), QString("0x000F"));
    QMap<QString, QString> colors;
    colors["Target"] = "#22c55e";
    QMap<QString, QString> actions;
    actions["Target"] = "Focus OD";
    p.updateSdoTargetPanelRows(rows, colors, actions);
    QCOMPARE(p.sdoTargetTable()->rowCount(), 2);
  }

  // Update SDO table evidence row sets value and status
  // Test updating SDO table evidence row
  void testUpdateSdoTableEvidenceRow() {
    OdPlugin p(container_);
    auto *table = p.sdoTable();
    table->setRowCount(1);
    table->setItem(0, 0, new QTableWidgetItem("CoE Object"));
    table->setItem(0, 1, new QTableWidgetItem("0x6040"));
    table->setItem(0, 2, new QTableWidgetItem("0x00"));
    p.updateSdoTableEvidenceRow(0, "0x000F", "Complete", "12:00:00",
                                 QColor("#22c55e"), QColor("#eef2ff"));
    QCOMPARE(table->item(0, 7)->text(), QString("0x000F"));
    QVERIFY(table->item(0, 8)->text().contains("Complete"));
  }

  // SDO value text edit is read-only
  // Verify SDO value field is read-only
  void testSdoValueIsReadOnly() {
    OdPlugin p(container_);
    QVERIFY(p.sdoValue()->isReadOnly());
  }
};

QTEST_MAIN(OdPluginTest)
#include "od_plugin_test.moc"
