/// @brief OverviewPlugin unit tests.
///
/// @details Tests the Overview workspace plugin's identity, UI construction,
/// sub-tab layout, and table accessors. Verifies that the plugin correctly
/// implements the WorkspacePlugin interface and provides the expected
/// widget hierarchy.
///
/// @par Test Coverage
///   - Plugin identity (id, displayName, displayNameZh, defaultOrder, visible)
///   - Widget construction and non-null checks
///   - Sub-tab layout (4 tabs: Details, Brief, Workflow, Matrix)
///   - Table accessor methods (metric, identity, port, mailbox, etc.)
///   - Tab index mapping
///
/// @par Test Dependencies
///   - Qt6::Test (QTest framework)
///   - Qt6::Widgets (for QTabWidget, QTableWidget)
///   - OverviewPlugin, ServiceContainer
///
/// @par Test Environment
///   - Requires QT_QPA_PLATFORM=offscreen for headless execution
///   - Creates a ServiceContainer per test (no daemon required)

#include "infra/EcatClient.h"
#include "plugins/overview/OverviewPlugin.h"
#include "services/EventBus.h"
#include "services/ServiceContainer.h"
#include <QTableWidget>
#include <QTabWidget>
#include <QTest>

class OverviewPluginTest : public QObject {
    Q_OBJECT
private:
    EcatClient* client_ = nullptr;
    ServiceContainer* container_ = nullptr;

private slots:
    /// Test suite setup: creates a ServiceContainer for all tests.
    void initTestCase() {
        client_ = new EcatClient(this);
        container_ = new ServiceContainer(client_, new EventBus(this), this);
    }

    /// Verifies plugin identity: id="overview", displayName="Overview", displayNameZh="总览".
    void testIdentity() {
        OverviewPlugin p(container_);
        QCOMPARE(p.id(), QString("overview"));
        QCOMPARE(p.displayName(), QString("Overview"));
        QCOMPARE(p.displayNameZh(), QString("总览"));
    }

    /// Verifies defaultOrder() returns 5 (leftmost tab position).
    void testDefaultOrder() {
        OverviewPlugin p(container_);
        QCOMPARE(p.defaultOrder(), 5);
    }

    /// Verifies visible() returns true (always visible).
    void testVisible() {
        OverviewPlugin p(container_);
        QVERIFY(p.visible());
    }

    /// Verifies widget() returns a non-null QWidget.
    void testWidgetNotNull() {
        OverviewPlugin p(container_);
        QVERIFY(p.widget() != nullptr);
    }

    /// Verifies the overview tab widget has exactly 4 sub-tabs.
    void testOverviewTabsNotNull() {
        OverviewPlugin p(container_);
        QVERIFY(p.overviewTabs() != nullptr);
        QCOMPARE(p.overviewTabs()->count(), 4);
    }

    /// Verifies sub-tab index mapping: Details=0, Brief=1, Workflow=2, Matrix=3.
    void testTabIndices() {
        OverviewPlugin p(container_);
        QCOMPARE(p.detailsTabIndex(), 0);
        QCOMPARE(p.briefTabIndex(), 1);
        QCOMPARE(p.workflowTabIndex(), 2);
        QCOMPARE(p.matrixTabIndex(), 3);
    }

    /// Verifies metricTable() returns a non-null QTableWidget.
    void testMetricTableNotNull() {
        OverviewPlugin p(container_);
        QVERIFY(p.metricTable() != nullptr);
    }

    /// Verifies identityTable() returns a non-null QTableWidget.
    void testIdentityTableNotNull() {
        OverviewPlugin p(container_);
        QVERIFY(p.identityTable() != nullptr);
    }

    /// Verifies portTable() returns a non-null QTableWidget.
    void testPortTableNotNull() {
        OverviewPlugin p(container_);
        QVERIFY(p.portTable() != nullptr);
    }

    /// Verifies mailboxTable() returns a non-null QTableWidget.
    void testMailboxTableNotNull() {
        OverviewPlugin p(container_);
        QVERIFY(p.mailboxTable() != nullptr);
    }

    /// Verifies sessionBriefTable() returns a non-null QTableWidget.
    void testSessionBriefTableNotNull() {
        OverviewPlugin p(container_);
        QVERIFY(p.sessionBriefTable() != nullptr);
    }

    /// Verifies workflowTable() returns a non-null QTableWidget.
    void testWorkflowTableNotNull() {
        OverviewPlugin p(container_);
        QVERIFY(p.workflowTable() != nullptr);
    }

    /// Verifies slaveEvidenceMatrixTable() returns a non-null QTableWidget.
    void testSlaveEvidenceMatrixTableNotNull() {
        OverviewPlugin p(container_);
        QVERIFY(p.slaveEvidenceMatrixTable() != nullptr);
    }

    /// Verifies sessionBriefTable accepts data and preserves cell values.
    void testSessionBriefTableAcceptsData() {
        OverviewPlugin p(container_);
        auto* table = p.sessionBriefTable();
        table->setColumnCount(4);
        table->setHorizontalHeaderLabels({"Area", "Status", "Evidence", "Next"});
        table->setRowCount(2);
        table->setItem(0, 0, new QTableWidgetItem("Master"));
        table->setItem(0, 1, new QTableWidgetItem("Connected"));
        QCOMPARE(table->rowCount(), 2);
        QCOMPARE(table->item(0, 0)->text(), QString("Master"));
    }
};

QTEST_MAIN(OverviewPluginTest)
#include "overview_plugin_test.moc"
