// SessionPluginTest — Tests for SessionPlugin
//
// Test coverage:
//   - Plugin identity, order, visibility, and widget creation
//   - Session brief table, copy button, and summary label accessors
//   - Session brief data update and row selection
//   - Copy button enable/disable state
//   - Row activated, copy requested, navigate, and diagnostics signals

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTest>

#include "infra/EcatClient.h"
#include "plugins/session/SessionPlugin.h"
#include "services/EventBus.h"
#include "services/ServiceContainer.h"

class SessionPluginTest : public QObject {
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
        SessionPlugin p(container_);
        QCOMPARE(p.id(), QString("session"));
        QCOMPARE(p.displayName(), QString("Session"));
        QCOMPARE(p.displayNameZh(), QString("\u4f1a\u8bdd"));
    }

    // Verify default order value
    void testDefaultOrder() {
        SessionPlugin p(container_);
        QCOMPARE(p.defaultOrder(), 80);
    }

    // Verify plugin is visible
    void testVisible() {
        SessionPlugin p(container_);
        QVERIFY(p.visible());
    }

    // Verify widget is created
    void testWidgetNotNull() {
        SessionPlugin p(container_);
        QVERIFY(p.widget() != nullptr);
    }

    // Verify session brief table accessor returns non-null
    void testTableAccessor() {
        SessionPlugin p(container_);
        QVERIFY(p.sessionBriefTable() != nullptr);
    }

    // Verify copy button accessor returns non-null
    void testCopyButtonAccessor() {
        SessionPlugin p(container_);
        QVERIFY(p.sessionBriefCopyButton() != nullptr);
    }

    // Verify summary label accessor returns non-null
    void testSummaryLabelAccessor() {
        SessionPlugin p(container_);
        QVERIFY(p.sessionBriefSummaryLabel() != nullptr);
    }

    // Test updating session brief populates table correctly
    void testUpdateSessionBrief() {
        SessionPlugin p(container_);
        QStringList headers = {"Area", "Status", "Evidence", "Next"};
        QList<QStringList> rows = {
            {"Target", "connected", "3 slaves", "Select slave"},
            {"Gate", "ready", "0 errors", "Proceed"},
        };
        QList<QColor> colors = {QColor("#22c55e"), QColor("#22c55e")};
        p.updateSessionBrief(headers, rows, colors);
        QCOMPARE(p.sessionBriefTable()->rowCount(), 2);
        QCOMPARE(p.sessionBriefTable()->columnCount(), 4);
        QCOMPARE(p.sessionBriefTable()->item(0, 0)->text(), QString("Target"));
        QCOMPARE(p.sessionBriefTable()->item(1, 1)->text(), QString("ready"));
    }

    // Verify empty table has zero rows
    void testRowCountEmpty() {
        SessionPlugin p(container_);
        QCOMPARE(p.sessionBriefTable()->rowCount(), 0);
    }

    // Verify no selection returns -1
    void testCurrentRowNoSelection() {
        SessionPlugin p(container_);
        QCOMPARE(p.sessionBriefTable()->currentRow(), -1);
    }

    // Verify copy button is disabled when table is empty
    void testCopyButtonDisabledWhenEmpty() {
        SessionPlugin p(container_);
        QVERIFY(!p.sessionBriefCopyButton()->isEnabled());
    }

    // Verify copy button is enabled after data and selection
    void testCopyButtonEnabledAfterUpdate() {
        SessionPlugin p(container_);
        QStringList headers = {"Area", "Status", "Evidence", "Next"};
        QList<QStringList> rows = {{"Target", "ok", "yes", "next"}};
        QList<QColor> colors = {QColor("#22c55e")};
        p.updateSessionBrief(headers, rows, colors);
        p.sessionBriefTable()->selectRow(0);
        QVERIFY(p.sessionBriefCopyButton()->isEnabled());
    }

    // Test current row area returns correct text
    void testCurrentRowArea() {
        SessionPlugin p(container_);
        QStringList headers = {"Area", "Status", "Evidence", "Next"};
        QList<QStringList> rows = {{"Map", "ok", "yes", "next"}};
        QList<QColor> colors = {QColor("#22c55e")};
        p.updateSessionBrief(headers, rows, colors);
        p.sessionBriefTable()->selectRow(0);
        QCOMPARE(p.currentRowArea(), QString("Map"));
    }

    // Test session brief row activated signal
    void testSessionBriefRowActivatedSignal() {
        SessionPlugin p(container_);
        QStringList headers = {"Area", "Status", "Evidence", "Next"};
        QList<QStringList> rows = {{"Target", "ok", "yes", "next"}};
        QList<QColor> colors = {QColor("#22c55e")};
        p.updateSessionBrief(headers, rows, colors);

        QSignalSpy spy(&p, &SessionPlugin::sessionBriefRowActivated);
        p.sessionBriefTable()->selectRow(0);
        QCOMPARE(spy.count(), 0);
    }

    // Test copy button click emits row copy requested signal
    void testSessionBriefRowCopyRequestedSignal() {
        SessionPlugin p(container_);
        QStringList headers = {"Area", "Status", "Evidence", "Next"};
        QList<QStringList> rows = {{"Target", "ok", "yes", "next"}};
        QList<QColor> colors = {QColor("#22c55e")};
        p.updateSessionBrief(headers, rows, colors);

        QSignalSpy spy(&p, &SessionPlugin::sessionBriefRowCopyRequested);
        p.sessionBriefTable()->selectRow(0);
        p.sessionBriefCopyButton()->click();
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toInt(), 0);
    }

    // Test requestNavigate signal is emitted
    void testRequestNavigateSignal() {
        SessionPlugin p(container_);
        QSignalSpy spy(&p, &WorkspacePlugin::requestNavigate);
        emit p.requestNavigate("target");
        QCOMPARE(spy.count(), 1);
    }

    // Test updateDiagnostics signal is emitted
    void testUpdateDiagnosticsSignal() {
        SessionPlugin p(container_);
        QSignalSpy spy(&p, &WorkspacePlugin::updateDiagnostics);
        emit p.updateDiagnostics("Info", "Session", "test message");
        QCOMPARE(spy.count(), 1);
    }
};

QTEST_MAIN(SessionPluginTest)
#include "session_plugin_test.moc"
