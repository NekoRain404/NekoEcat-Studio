// FreeRunPluginTest — Tests for FreeRunPlugin
//
// Test coverage:
//   - Plugin identity (id, display names, order, visibility)
//   - UI widget and accessor creation
//   - Entry table population with headers and rows
//   - Summary and detail label display
//   - Free-run state toggle
//   - Last status tracking
//   - Entry names and values hash maps
//   - Open charts state

#include <QApplication>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QTest>

#include "infra/EcatClient.h"
#include "plugins/freerun/FreeRunPlugin.h"
#include "services/EventBus.h"
#include "services/ServiceContainer.h"

class FreeRunPluginTest : public QObject {
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

    // Verify plugin id, display names
    void testIdentity() {
        FreeRunPlugin p(container_);
        QCOMPARE(p.id(), QString("freerun"));
        QCOMPARE(p.displayName(), QString("Free Run"));
        QCOMPARE(p.displayNameZh(), QString("自由运行"));
    }

    // Verify default tab order
    void testDefaultOrder() {
        FreeRunPlugin p(container_);
        QCOMPARE(p.defaultOrder(), 35);
    }

    // Verify plugin is visible
    void testVisible() {
        FreeRunPlugin p(container_);
        QVERIFY(p.visible());
    }

    // Check widget is created
    void testWidgetNotNull() {
        FreeRunPlugin p(container_);
        QVERIFY(p.widget() != nullptr);
    }

    // Check entry table accessor
    void testEntryTableAccessor() {
        FreeRunPlugin p(container_);
        QVERIFY(p.entryTable() != nullptr);
    }

    // Check filter widget accessor
    void testFilterAccessor() {
        FreeRunPlugin p(container_);
        QVERIFY(p.filter() != nullptr);
    }

    // Check changed-only checkbox accessor
    void testChangedOnlyAccessor() {
        FreeRunPlugin p(container_);
        QVERIFY(p.changedOnly() != nullptr);
    }

    // Check summary label accessor
    void testSummaryLabelAccessor() {
        FreeRunPlugin p(container_);
        QVERIFY(p.summaryLabel() != nullptr);
    }

    // Check detail label accessor
    void testDetailLabelAccessor() {
        FreeRunPlugin p(container_);
        QVERIFY(p.detailLabel() != nullptr);
    }

    // Test setting entry table rows and headers
    void testSetEntryRows() {
        FreeRunPlugin p(container_);
        QStringList headers = {"Name", "Index", "Value"};
        QList<QStringList> rows = {
            {"Counter", "0x6000:01", "42"},
            {"Status", "0x6001:01", "0xFF"},
        };
        p.setEntryRows(headers, rows);
        QCOMPARE(p.entryTable()->rowCount(), 2);
        QCOMPARE(p.entryTable()->columnCount(), 3);
        QCOMPARE(p.entryTable()->item(0, 0)->text(), QString("Counter"));
        QCOMPARE(p.entryTable()->item(1, 2)->text(), QString("0xFF"));
    }

    // Test summary label text update
    void testSetSummary() {
        FreeRunPlugin p(container_);
        p.setSummary("Entries 5 | Running");
        QCOMPARE(p.summaryLabel()->text(), QString("Entries 5 | Running"));
    }

    // Test detail label text update
    void testSetDetail() {
        FreeRunPlugin p(container_);
        p.setDetail("Selected: Counter 0x6000:01 = 42");
        QCOMPARE(p.detailLabel()->text(), QString("Selected: Counter 0x6000:01 = 42"));
    }

    // Test free-run enabled state toggle
    void testFreeRunState() {
        FreeRunPlugin p(container_);
        QVERIFY(!p.freeRunEnabled());
        p.setFreeRunEnabled(true);
        QVERIFY(p.freeRunEnabled());
    }

    // Test last status string tracking
    void testLastStatus() {
        FreeRunPlugin p(container_);
        QCOMPARE(p.lastStatus(), QString("Stopped"));
        p.setLastStatus("Running");
        QCOMPARE(p.lastStatus(), QString("Running"));
    }

    // Test entry names hash map
    void testEntryNames() {
        FreeRunPlugin p(container_);
        QHash<QString, QString> names;
        names["0"] = "Counter";
        names["1"] = "Status";
        p.setEntryNames(names);
        QCOMPARE(p.entryName(0), QString("Counter"));
        QCOMPARE(p.entryName(1), QString("Status"));
        QCOMPARE(p.entryName(2), QString());
    }

    // Test entry values hash map
    void testEntryValues() {
        FreeRunPlugin p(container_);
        QHash<QString, QString> values;
        values["0"] = "42";
        values["1"] = "0xFF";
        p.setEntryValues(values);
        QCOMPARE(p.entryValue(0), QString("42"));
        QCOMPARE(p.entryValue(1), QString("0xFF"));
        QCOMPARE(p.entryValue(2), QString());
    }

    // Verify open charts list is initially empty
    void testOpenChartsEmpty() {
        FreeRunPlugin p(container_);
        QVERIFY(p.openCharts().isEmpty());
    }
};

QTEST_MAIN(FreeRunPluginTest)
#include "freerun_plugin_test.moc"
