// Unit tests for workspace widget structs — allocation and lifecycle.
#include "WorkspaceWidgets.h"

#include <QtTest/QtTest>

class WorkspaceWidgetsTest : public QObject {
    Q_OBJECT

private slots:
    void sessionWidgetsDefaultNull();
    void watchWidgetsDefaultNull();
    void sdoWidgetsDefaultNull();
    void freeRunWidgetsDefaultNull();
    void consistencyWidgetsDefaultNull();
    void diagnosticsWidgetsDefaultNull();
    void workflowWidgetsDefaultNull();
    void ioVarWidgetsDefaultNull();
    void stateMachineWidgetsDefaultNull();
    void slaveEvidenceWidgetsDefaultNull();
    void bookmarkWidgetsDefaultNull();
    void canAllocateAndDelete();
};

void WorkspaceWidgetsTest::sessionWidgetsDefaultNull()
{
    SessionWorkspaceWidgets w;
    QVERIFY(w.sessionBriefTable == nullptr);
    QVERIFY(w.sessionBriefCopyButton == nullptr);
}

void WorkspaceWidgetsTest::watchWidgetsDefaultNull()
{
    WatchWorkspaceWidgets w;
    QVERIFY(w.watchTable == nullptr);
    QVERIFY(w.watchFilter == nullptr);
    QVERIFY(w.watchAutoRefresh == nullptr);
    QVERIFY(w.watchChangedOnly == nullptr);
    QVERIFY(w.watchScopeFilter == nullptr);
    QVERIFY(w.watchRefreshInterval == nullptr);
    QVERIFY(w.watchSummaryLabel == nullptr);
    QVERIFY(w.watchDetailLabel == nullptr);
    QVERIFY(w.startupWatchDiffsOnly == nullptr);
    QVERIFY(w.startupWatchSummaryLabel == nullptr);
    QVERIFY(w.startupSdoDetailLabel == nullptr);
}

void WorkspaceWidgetsTest::sdoWidgetsDefaultNull()
{
    SdoWorkspaceWidgets w;
    QVERIFY(w.sdoTable == nullptr);
    QVERIFY(w.pdoTable == nullptr);
    QVERIFY(w.sdoFilter == nullptr);
    QVERIFY(w.pdoFilter == nullptr);
    QVERIFY(w.sdoSummaryLabel == nullptr);
    QVERIFY(w.pdoSummaryLabel == nullptr);
    QVERIFY(w.pdoDetailLabel == nullptr);
}

void WorkspaceWidgetsTest::freeRunWidgetsDefaultNull()
{
    FreeRunWorkspaceWidgets w;
    QVERIFY(w.freeRunEntryTable == nullptr);
    QVERIFY(w.freeRunTable == nullptr);
    QVERIFY(w.freeRunFilter == nullptr);
    QVERIFY(w.freeRunChangedOnly == nullptr);
    QVERIFY(w.freeRunEntrySummaryLabel == nullptr);
    QVERIFY(w.freeRunEntryDetailLabel == nullptr);
}

void WorkspaceWidgetsTest::consistencyWidgetsDefaultNull()
{
    ConsistencyWorkspaceWidgets w;
    QVERIFY(w.consistencyTable == nullptr);
    QVERIFY(w.consistencyFilter == nullptr);
    QVERIFY(w.consistencyScopeFilter == nullptr);
    QVERIFY(w.consistencySummaryLabel == nullptr);
    QVERIFY(w.consistencyDetailLabel == nullptr);
}

void WorkspaceWidgetsTest::diagnosticsWidgetsDefaultNull()
{
    DiagnosticsWorkspaceWidgets w;
    QVERIFY(w.diagnosticsTable == nullptr);
    QVERIFY(w.diagnosticsFilter == nullptr);
    QVERIFY(w.diagnosticsLevelFilter == nullptr);
    QVERIFY(w.hostHealthSummaryLabel == nullptr);
    QVERIFY(w.diagnosticsSummaryLabel == nullptr);
    QVERIFY(w.topologyBaselineLabel == nullptr);
}

void WorkspaceWidgetsTest::workflowWidgetsDefaultNull()
{
    WorkflowWorkspaceWidgets w;
    QVERIFY(w.workflowTable == nullptr);
    QVERIFY(w.workflowSummaryLabel == nullptr);
    QVERIFY(w.workflowStepDetailLabel == nullptr);
    QVERIFY(w.workflowScopeFilter == nullptr);
    QVERIFY(w.workflowFilter == nullptr);
    QVERIFY(w.workflowReviewButton == nullptr);
    QVERIFY(w.workflowReviewNextButton == nullptr);
    QVERIFY(w.workflowStepCopyButton == nullptr);
}

void WorkspaceWidgetsTest::ioVarWidgetsDefaultNull()
{
    IoVariableWorkspaceWidgets w;
    QVERIFY(w.ioVariableTable == nullptr);
    QVERIFY(w.ioVariableFilter == nullptr);
    QVERIFY(w.ioVariableScopeFilter == nullptr);
    QVERIFY(w.ioVariableSummaryLabel == nullptr);
    QVERIFY(w.ioVariableDetailLabel == nullptr);
}

void WorkspaceWidgetsTest::stateMachineWidgetsDefaultNull()
{
    StateMachineWorkspaceWidgets w;
    QVERIFY(w.stateMachineTable == nullptr);
    QVERIFY(w.stateMachineSummaryLabel == nullptr);
    QVERIFY(w.stateMachineDetailLabel == nullptr);
}

void WorkspaceWidgetsTest::slaveEvidenceWidgetsDefaultNull()
{
    SlaveEvidenceWorkspaceWidgets w;
    QVERIFY(w.slaveEvidenceMatrixTable == nullptr);
    QVERIFY(w.slaveEvidenceMatrixSummaryLabel == nullptr);
    QVERIFY(w.slaveEvidenceMatrixReviewButton == nullptr);
    QVERIFY(w.slaveEvidenceMatrixReviewNextButton == nullptr);
    QVERIFY(w.slaveEvidenceMatrixCopyButton == nullptr);
    QVERIFY(w.slaveEvidenceMatrixTriageButtons.isEmpty());
    QVERIFY(w.slaveEvidenceMatrixFilter == nullptr);
    QVERIFY(w.slaveEvidenceMatrixScopeFilter == nullptr);
}

void WorkspaceWidgetsTest::bookmarkWidgetsDefaultNull()
{
    BookmarkWorkspaceWidgets w;
    QVERIFY(w.objectBookmarkTable == nullptr);
}

void WorkspaceWidgetsTest::canAllocateAndDelete()
{
    // Verify structs can be heap-allocated and deleted without crash.
    auto *s = new SessionWorkspaceWidgets;
    auto *w = new WatchWorkspaceWidgets;
    auto *sd = new SdoWorkspaceWidgets;
    auto *f = new FreeRunWorkspaceWidgets;
    auto *c = new ConsistencyWorkspaceWidgets;
    auto *d = new DiagnosticsWorkspaceWidgets;
    auto *wf = new WorkflowWorkspaceWidgets;
    auto *io = new IoVariableWorkspaceWidgets;
    auto *sm = new StateMachineWorkspaceWidgets;
    auto *se = new SlaveEvidenceWorkspaceWidgets;
    auto *b = new BookmarkWorkspaceWidgets;
    delete s;
    delete w;
    delete sd;
    delete f;
    delete c;
    delete d;
    delete wf;
    delete io;
    delete sm;
    delete se;
    delete b;
    QVERIFY(true); // reached without crash
}

QTEST_MAIN(WorkspaceWidgetsTest)
#include "workspace_widgets_test.moc"
