// IoVariablePluginTest — Tests for IoVariablePlugin
//
// Test coverage:
//   - Plugin identity, order, visibility, and widget creation
//   - Table, filter, and summary/detail label accessors
//   - Row management and scope filter options

#include <QTest>
#include <QApplication>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>

#include "plugins/iovariable/IoVariablePlugin.h"
#include "services/ServiceContainer.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class IoVariablePluginTest : public QObject {
  Q_OBJECT
private:
  EcatClient *client_ = nullptr;
  ServiceContainer *container_ = nullptr;

private slots:
  void init() {
    client_ = new EcatClient(this);
    container_ = new ServiceContainer(client_, new EventBus(this), this);
  }
  void cleanup() {
    delete container_;
    container_ = nullptr;
  }

  // Verify plugin id, display names
  void testIdentity() {
    IoVariablePlugin p(container_);
    QCOMPARE(p.id(), QString("iovariable"));
    QCOMPARE(p.displayName(), QString("I/O Variables"));
    QCOMPARE(p.displayNameZh(), QString("I/O 变量"));
  }

  // Plugin has expected default order
  // Verify default order is 40
  void testDefaultOrder() {
    IoVariablePlugin p(container_);
    QCOMPARE(p.defaultOrder(), 40);
  }

  // Plugin is visible by default
  // Verify plugin is visible
  void testVisible() {
    IoVariablePlugin p(container_);
    QVERIFY(p.visible());
  }

  // Widget is created and not null
  // Check widget is created
  void testWidgetNotNull() {
    IoVariablePlugin p(container_);
    QVERIFY(p.widget() != nullptr);
  }

  // I/O variable table accessor returns non-null
  // Check I/O variable table accessor
  void testTableAccessor() {
    IoVariablePlugin p(container_);
    QVERIFY(p.ioVariableTable() != nullptr);
  }

  // Filter line edit accessor returns non-null
  // Check filter line edit accessor
  void testFilterAccessor() {
    IoVariablePlugin p(container_);
    QVERIFY(p.ioVariableFilter() != nullptr);
  }

  // Scope filter combo box accessor returns non-null
  // Check scope filter combo box accessor
  void testScopeFilterAccessor() {
    IoVariablePlugin p(container_);
    QVERIFY(p.ioVariableScopeFilter() != nullptr);
  }

  // Summary label accessor returns non-null
  // Check summary label accessor
  void testSummaryLabelAccessor() {
    IoVariablePlugin p(container_);
    QVERIFY(p.ioVariableSummaryLabel() != nullptr);
  }

  // Detail label accessor returns non-null
  // Check detail label accessor
  void testDetailLabelAccessor() {
    IoVariablePlugin p(container_);
    QVERIFY(p.ioVariableDetailLabel() != nullptr);
  }

  // Set rows populates table with correct data
  // Test populating table with headers and rows
  void testSetRows() {
    IoVariablePlugin p(container_);
    QStringList headers = {"Slave", "Dir", "Symbol", "Index", "Sub"};
    QList<QStringList> rows = {
        {"1", "Rx Output", "0x6000:01", "0x6000", "01"},
        {"2", "Tx Input", "0x7000:01", "0x7000", "01"},
    };
    p.setRows(headers, rows);
    QCOMPARE(p.ioVariableTable()->rowCount(), 2);
    QCOMPARE(p.ioVariableTable()->columnCount(), 5);
    QCOMPARE(p.ioVariableTable()->item(0, 0)->text(), QString("1"));
    QCOMPARE(p.ioVariableTable()->item(1, 2)->text(), QString("0x7000:01"));
  }

  // Set summary updates label text and severity property
  // Test setting summary label with severity
  void testSetSummary() {
    IoVariablePlugin p(container_);
    p.setSummary("100/100 | All | process 50 | watch 30 | startup diff 5 | missing 2 | changed 10 | plc issues 3", "warning");
    QCOMPARE(p.ioVariableSummaryLabel()->text(), QString("100/100 | All | process 50 | watch 30 | startup diff 5 | missing 2 | changed 10 | plc issues 3"));
    QCOMPARE(p.ioVariableSummaryLabel()->property("severity").toString(),
             QString("warning"));
  }

  // Set detail updates label text and severity property
  // Test setting detail label with severity
  void testSetDetail() {
    IoVariablePlugin p(container_);
    p.setDetail("#1 0x6000:01 | Rx Output | Slave 1 | Value: 0x1234", "ok");
    QCOMPARE(p.ioVariableDetailLabel()->text(), QString("#1 0x6000:01 | Rx Output | Slave 1 | Value: 0x1234"));
    QCOMPARE(p.ioVariableDetailLabel()->property("severity").toString(), QString("ok"));
  }

  // Set summary tooltip updates label tooltip
  // Test setting summary tooltip
  void testSetSummaryToolTip() {
    IoVariablePlugin p(container_);
    p.setSummaryToolTip("tooltip text");
    QCOMPARE(p.ioVariableSummaryLabel()->toolTip(), QString("tooltip text"));
  }

  // Set detail tooltip updates label tooltip
  // Test setting detail tooltip
  void testSetDetailToolTip() {
    IoVariablePlugin p(container_);
    p.setDetailToolTip("detail tip");
    QCOMPARE(p.ioVariableDetailLabel()->toolTip(), QString("detail tip"));
  }

  // Row count is zero on empty plugin
  // Verify row count is zero initially
  void testRowCountEmpty() {
    IoVariablePlugin p(container_);
    QCOMPARE(p.rowCount(), 0);
  }

  // Row count reflects setRows call
  // Verify row count after setting rows
  void testRowCountAfterSet() {
    IoVariablePlugin p(container_);
    p.setRows({"A"}, {{"1"}, {"2"}, {"3"}});
    QCOMPARE(p.rowCount(), 3);
  }

  // Current row is -1 with no selection
  // Verify current row is -1 with no selection
  void testCurrentRowNoSelection() {
    IoVariablePlugin p(container_);
    QCOMPARE(p.currentRow(), -1);
  }

  // Set current cell updates current row
  // Test setting current cell selection
  void testSetCurrentCell() {
    IoVariablePlugin p(container_);
    p.setRows({"A", "B"}, {{"1", "2"}, {"3", "4"}});
    p.setCurrentCell(1, 0);
    QCOMPARE(p.currentRow(), 1);
  }

  // Row hidden state can be toggled per row
  // Test hiding and showing rows
  void testRowHidden() {
    IoVariablePlugin p(container_);
    p.setRows({"A"}, {{"1"}, {"2"}, {"3"}});
    p.setRowHidden(1, true);
    QVERIFY(p.isRowHidden(1));
    QVERIFY(!p.isRowHidden(0));
    QVERIFY(!p.isRowHidden(2));
  }

  // Scope filter combo has expected options
  // Verify scope filter combo box has correct options
  void testScopeFilterOptions() {
    IoVariablePlugin p(container_);
    QComboBox *combo = p.ioVariableScopeFilter();
    QCOMPARE(combo->count(), 5);
    QCOMPARE(combo->itemText(0), QString("All"));
    QCOMPARE(combo->itemText(1), QString("Process"));
    QCOMPARE(combo->itemText(2), QString("Watch"));
    QCOMPARE(combo->itemText(3), QString("Startup"));
    QCOMPARE(combo->itemText(4), QString("PLC Issues"));
  }

  // Filter has placeholder text and clear button
  // Verify filter has placeholder text and clear button
  void testFilterPlaceholder() {
    IoVariablePlugin p(container_);
    QLineEdit *filter = p.ioVariableFilter();
    QVERIFY(!filter->placeholderText().isEmpty());
    QVERIFY(filter->isClearButtonEnabled());
  }
};

QTEST_MAIN(IoVariablePluginTest)
#include "iovariable_plugin_test.moc"
