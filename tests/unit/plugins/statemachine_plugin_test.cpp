// StateMachinePluginTest — Tests for State Machine Plugin
//
// Test coverage:
//   - Plugin identity (id, display names, order, visibility)
//   - Widget and table/label accessors
//   - Row population and summary/detail labels
//   - Tooltip setting
//   - Row count and current row tracking
// StateMachinePluginTest — Tests for StateMachinePlugin
//
// Test coverage:
//   - Plugin identity, order, visibility, and widget creation
//   - Table, summary label, and detail label accessors
//   - Row data population and summary/detail text
//   - Summary and detail tooltips
//   - Row count and current row selection

#include <QApplication>
#include <QLabel>
#include <QTableWidget>
#include <QTest>

#include "infra/EcatClient.h"
#include "plugins/statemachine/StateMachinePlugin.h"
#include "services/EventBus.h"
#include "services/ServiceContainer.h"

class StateMachinePluginTest : public QObject {
    Q_OBJECT
private:
    EcatClient* client_ = nullptr;
    ServiceContainer* container_ = nullptr;

private slots:
    void init() {
        client_ = new EcatClient(this);
        container_ = new ServiceContainer(client_, new EventBus(this), this);
    }
    void cleanup() {
        delete container_;
        container_ = nullptr;
    }

    // Verify plugin ID, display name, and Chinese display name
    void testIdentity() {
        StateMachinePlugin p(container_);
        QCOMPARE(p.id(), QString("statemachine"));
        QCOMPARE(p.displayName(), QString("State Machine"));
        QCOMPARE(p.displayNameZh(), QString("状态机"));
    }

    // Verify default order value
    void testDefaultOrder() {
        StateMachinePlugin p(container_);
        QCOMPARE(p.defaultOrder(), 60);
    }

    // Verify plugin is visible
    void testVisible() {
        StateMachinePlugin p(container_);
        QVERIFY(p.visible());
    }

    // Verify widget is created
    void testWidgetNotNull() {
        StateMachinePlugin p(container_);
        QVERIFY(p.widget() != nullptr);
    }

    // Verify table accessor returns non-null
    void testTableAccessor() {
        StateMachinePlugin p(container_);
        QVERIFY(p.table() != nullptr);
    }

    // Verify summary label accessor returns non-null
    void testSummaryLabelAccessor() {
        StateMachinePlugin p(container_);
        QVERIFY(p.summaryLabel() != nullptr);
    }

    // Verify detail label accessor returns non-null
    void testDetailLabelAccessor() {
        StateMachinePlugin p(container_);
        QVERIFY(p.detailLabel() != nullptr);
    }

    // Test setting rows populates table correctly
    void testSetRows() {
        StateMachinePlugin p(container_);
        QStringList headers = {"Slave", "Name", "Current", "Recommended"};
        QList<QStringList> rows = {
            {"1", "EL1008", "OP", ""},
            {"2", "EL2004", "SAFEOP", "OP"},
        };
        p.setRows(headers, rows);
        QCOMPARE(p.table()->rowCount(), 2);
        QCOMPARE(p.table()->columnCount(), 4);
        QCOMPARE(p.table()->item(0, 0)->text(), QString("1"));
        QCOMPARE(p.table()->item(1, 2)->text(), QString("SAFEOP"));
    }

    // Test setting summary text and severity property
    void testSetSummary() {
        StateMachinePlugin p(container_);
        p.setSummary("Slaves 3 | OP 2 SAFEOP 1", "warning");
        QCOMPARE(p.summaryLabel()->text(), QString("Slaves 3 | OP 2 SAFEOP 1"));
        QCOMPARE(p.summaryLabel()->property("severity").toString(), QString("warning"));
    }

    // Test setting detail text and severity property
    void testSetDetail() {
        StateMachinePlugin p(container_);
        p.setDetail("Selected: #1 EL1008 OP", "ok");
        QCOMPARE(p.detailLabel()->text(), QString("Selected: #1 EL1008 OP"));
        QCOMPARE(p.detailLabel()->property("severity").toString(), QString("ok"));
    }

    // Test setting summary tooltip
    void testSetSummaryToolTip() {
        StateMachinePlugin p(container_);
        p.setSummaryToolTip("tooltip text");
        QCOMPARE(p.summaryLabel()->toolTip(), QString("tooltip text"));
    }

    // Test setting detail tooltip
    void testSetDetailToolTip() {
        StateMachinePlugin p(container_);
        p.setDetailToolTip("detail tip");
        QCOMPARE(p.detailLabel()->toolTip(), QString("detail tip"));
    }

    // Verify empty table has zero rows
    void testRowCountEmpty() {
        StateMachinePlugin p(container_);
        QCOMPARE(p.rowCount(), 0);
    }

    // Verify row count after setting rows
    void testRowCountAfterSet() {
        StateMachinePlugin p(container_);
        p.setRows({"A"}, {{"1"}, {"2"}, {"3"}});
        QCOMPARE(p.rowCount(), 3);
    }

    // Verify no selection returns -1
    void testCurrentRowNoSelection() {
        StateMachinePlugin p(container_);
        QCOMPARE(p.currentRow(), -1);
    }
};

QTEST_MAIN(StateMachinePluginTest)
#include "statemachine_plugin_test.moc"
