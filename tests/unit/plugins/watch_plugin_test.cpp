/// @brief WatchPlugin unit tests.
///
/// @details Tests the Watch workspace plugin's identity, UI construction,
/// table management, filter controls, auto-refresh configuration, and
/// row manipulation operations.
///
/// @par Test Coverage
///   - Plugin identity (id, displayName, displayNameZh, defaultOrder, visible)
///   - Widget construction and non-null checks
///   - Table accessor (12-column watch table)
///   - Filter input and signal emission
///   - Auto-refresh checkbox and interval combo
///   - Table population (ensureWatchTable, insert/remove/clear rows)
///   - Selection and row visibility
///   - Summary label updates
///
/// @par Test Dependencies
///   - Qt6::Test (QTest framework)
///   - Qt6::Widgets (for QTableWidget, QLineEdit, QCheckBox, QComboBox)
///   - WatchPlugin, ServiceContainer
///
/// @par Test Environment
///   - Requires QT_QPA_PLATFORM=offscreen for headless execution
///   - Creates a ServiceContainer and WatchPlugin in initTestCase()

// WatchPlugin unit tests.

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QtTest/QtTest>

#include "infra/EcatClient.h"
#include "plugins/watch/WatchPlugin.h"
#include "services/EventBus.h"
#include "services/ServiceContainer.h"

class TestWatchPlugin : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void identity();
    void widgetNotNull();
    void tableAccess();
    void filterControls();
    void autoRefreshControls();
    void ensureWatchTable();
    void insertAndRemoveRows();
    void clearWatch();
    void selection();
    void summary();
    void filterSignal();

private:
    ServiceContainer* container_ = nullptr;
    EcatClient* client_ = nullptr;
    WatchPlugin* plugin_ = nullptr;
};

void TestWatchPlugin::initTestCase() {
    client_ = new EcatClient(this);
    container_ = new ServiceContainer(client_, new EventBus(this), this);
    plugin_ = new WatchPlugin(container_, this);
}

void TestWatchPlugin::cleanupTestCase() {
    delete plugin_;
    plugin_ = nullptr;
    delete container_;
    container_ = nullptr;
}

void TestWatchPlugin::identity() {
    QCOMPARE(plugin_->id(), QString("watch"));
    QCOMPARE(plugin_->displayName(), QString("Watch"));
    QCOMPARE(plugin_->displayNameZh(), QString("监视"));
    QCOMPARE(plugin_->defaultOrder(), 30);
    QVERIFY(plugin_->visible());
}

void TestWatchPlugin::widgetNotNull() {
    QVERIFY(plugin_->widget() != nullptr);
}

void TestWatchPlugin::tableAccess() {
    QVERIFY(plugin_->watchTable() != nullptr);
    QCOMPARE(plugin_->watchTable()->columnCount(), 12);
}

void TestWatchPlugin::filterControls() {
    QVERIFY(plugin_->filterInput() != nullptr);
    QVERIFY(plugin_->filterInput()->placeholderText().contains("Filter"));
}

void TestWatchPlugin::autoRefreshControls() {
    QVERIFY(plugin_->autoRefreshCheckBox() != nullptr);
    QVERIFY(!plugin_->autoRefreshCheckBox()->isChecked());
    QVERIFY(plugin_->refreshIntervalCombo() != nullptr);
    QCOMPARE(plugin_->refreshIntervalCombo()->count(), 4);
}

void TestWatchPlugin::ensureWatchTable() {
    plugin_->ensureWatchTable();
    QCOMPARE(plugin_->watchTable()->columnCount(), 12);
}

void TestWatchPlugin::insertAndRemoveRows() {
    plugin_->clearWatch();
    QCOMPARE(plugin_->rowCount(), 0);

    plugin_->insertWatchRow(0, {"10:00:00", "1", "0x6040", "0x00", "0x0000"});
    QCOMPARE(plugin_->rowCount(), 1);

    plugin_->insertWatchRow(1, {"10:00:01", "2", "0x6041", "0x00", "0x0000"});
    QCOMPARE(plugin_->rowCount(), 2);

    QCOMPARE(plugin_->watchTable()->item(0, 1)->text(), QString("1"));
    QCOMPARE(plugin_->watchTable()->item(1, 2)->text(), QString("0x6041"));

    plugin_->removeWatchRow(0);
    QCOMPARE(plugin_->rowCount(), 1);
    QCOMPARE(plugin_->watchTable()->item(0, 1)->text(), QString("2"));

    plugin_->clearWatch();
}

void TestWatchPlugin::clearWatch() {
    plugin_->insertWatchRow(0, {"10:00:00", "1", "0x6040", "0x00", "0x0000"});
    plugin_->insertWatchRow(1, {"10:00:01", "2", "0x6041", "0x00", "0x0000"});
    QCOMPARE(plugin_->rowCount(), 2);

    plugin_->clearWatch();
    QCOMPARE(plugin_->rowCount(), 0);
}

void TestWatchPlugin::selection() {
    plugin_->clearWatch();
    plugin_->insertWatchRow(0, {"10:00:00", "1", "0x6040", "0x00", "0x0000"});
    plugin_->insertWatchRow(1, {"10:00:01", "2", "0x6041", "0x00", "0x0000"});

    plugin_->selectRow(1);
    QCOMPARE(plugin_->currentRow(), 1);

    plugin_->selectRow(0);
    QCOMPARE(plugin_->currentRow(), 0);

    plugin_->clearWatch();
}

void TestWatchPlugin::summary() {
    plugin_->setSummary("3 items | 1 changed");
    QCOMPARE(plugin_->summaryLabel()->text(), QString("3 items | 1 changed"));
}

void TestWatchPlugin::filterSignal() {
    QSignalSpy spy(plugin_, &WatchPlugin::filterChanged);
    plugin_->filterInput()->setText("test");
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QString("test"));
}

QTEST_MAIN(TestWatchPlugin)
#include "watch_plugin_test.moc"
