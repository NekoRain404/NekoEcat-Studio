// WorkspaceWidgetsTest — Tests for Workspace Widget Structs
//
// Test coverage:
//   - Session widgets default null state
//   - Watch widgets default null state
//   - SDO widgets default null state
//   - Free Run widgets default null state
//   - Consistency widgets default null state
//   - Diagnostics widgets default null state
//   - Workflow widgets default null state
//   - I/O Variable widgets default null state
//   - State Machine widgets default null state
//   - Slave Evidence widgets default null state
//   - Bookmark widgets default null state
//   - Widget allocation and deletion lifecycle
#include "WorkspaceWidgets.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QtTest/QtTest>

class WorkspaceWidgetsTest : public QObject {
    Q_OBJECT

private slots:
    // SessionWorkspaceWidgets pointers default to null
    void sessionWidgetsDefaultNull();
    // WatchWorkspaceWidgets pointers default to null
    void watchWidgetsDefaultNull();
    // SdoWorkspaceWidgets pointers default to null
    void sdoWidgetsDefaultNull();
    // FreeRunWorkspaceWidgets pointers default to null
    void freeRunWidgetsDefaultNull();
    // ConsistencyWorkspaceWidgets pointers default to null
    void consistencyWidgetsDefaultNull();
    // DiagnosticsWorkspaceWidgets pointers default to null
    void diagnosticsWidgetsDefaultNull();
    // WorkflowWorkspaceWidgets pointers default to null
    void workflowWidgetsDefaultNull();
    // IoVarWorkspaceWidgets pointers default to null
    void ioVarWidgetsDefaultNull();
    // StateMachineWorkspaceWidgets pointers default to null
    void stateMachineWidgetsDefaultNull();
    // SlaveEvidenceWorkspaceWidgets pointers default to null
    void slaveEvidenceWidgetsDefaultNull();
    // BookmarkWorkspaceWidgets pointers default to null
    void bookmarkWidgetsDefaultNull();
    // SdoInspectorWidgets pointers default to null
    void sdoInspectorWidgetsDefaultNull();
    // RawTextWidgets pointers default to null
    void rawTextWidgetsDefaultNull();
    // Allocate widgets and verify non-null, then delete
    void canAllocateAndDelete();
    // Assign real QWidget pointers and verify they persist
    void widgetPointerValidity();
    // SlaveEvidence triage buttons vector manipulation
    void slaveEvidenceTriageButtonsVector();
    // Copy struct preserves pointer values
    void structCopyPreservesPointers();
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

void WorkspaceWidgetsTest::sdoInspectorWidgetsDefaultNull()
{
    SdoInspectorWidgets w;
    QVERIFY(w.sdoInspectorLabel == nullptr);
    QVERIFY(w.sdoTargetTable == nullptr);
    QVERIFY(w.sdoIndex == nullptr);
    QVERIFY(w.sdoSubIndex == nullptr);
    QVERIFY(w.sdoValue == nullptr);
    QVERIFY(w.sdoWriteValue == nullptr);
    QVERIFY(w.useSdoValueButton == nullptr);
    QVERIFY(w.sdoType == nullptr);
}

void WorkspaceWidgetsTest::rawTextWidgetsDefaultNull()
{
    RawTextWidgets w;
    QVERIFY(w.masterText == nullptr);
    QVERIFY(w.infoText == nullptr);
    QVERIFY(w.pdoText == nullptr);
    QVERIFY(w.sdoText == nullptr);
    QVERIFY(w.xmlText == nullptr);
    QVERIFY(w.logText == nullptr);
    QVERIFY(w.projectNotes == nullptr);
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

void WorkspaceWidgetsTest::widgetPointerValidity()
{
    SdoWorkspaceWidgets w;
    QTableWidget table;
    QLineEdit filter;
    QLabel summary;
    w.sdoTable = &table;
    w.sdoFilter = &filter;
    w.sdoSummaryLabel = &summary;
    QVERIFY(w.sdoTable != nullptr);
    QVERIFY(w.sdoFilter != nullptr);
    QVERIFY(w.sdoSummaryLabel != nullptr);
    QCOMPARE(w.sdoTable, &table);
    QCOMPARE(w.sdoFilter, &filter);
    QCOMPARE(w.sdoSummaryLabel, &summary);
    QVERIFY(w.pdoTable == nullptr);
    QVERIFY(w.pdoFilter == nullptr);

    WatchWorkspaceWidgets ww;
    QCheckBox autoRefresh;
    QComboBox scopeFilter;
    ww.watchAutoRefresh = &autoRefresh;
    ww.watchScopeFilter = &scopeFilter;
    QVERIFY(ww.watchAutoRefresh != nullptr);
    QVERIFY(ww.watchScopeFilter != nullptr);
    QCOMPARE(ww.watchAutoRefresh, &autoRefresh);
    QCOMPARE(ww.watchScopeFilter, &scopeFilter);
}

void WorkspaceWidgetsTest::slaveEvidenceTriageButtonsVector()
{
    SlaveEvidenceWorkspaceWidgets w;
    QVERIFY(w.slaveEvidenceMatrixTriageButtons.isEmpty());
    QCOMPARE(w.slaveEvidenceMatrixTriageButtons.size(), 0);

    QPushButton b1, b2, b3;
    w.slaveEvidenceMatrixTriageButtons.append(&b1);
    w.slaveEvidenceMatrixTriageButtons.append(&b2);
    w.slaveEvidenceMatrixTriageButtons.append(&b3);
    QCOMPARE(w.slaveEvidenceMatrixTriageButtons.size(), 3);
    QCOMPARE(w.slaveEvidenceMatrixTriageButtons.at(0), &b1);
    QCOMPARE(w.slaveEvidenceMatrixTriageButtons.at(1), &b2);
    QCOMPARE(w.slaveEvidenceMatrixTriageButtons.at(2), &b3);

    w.slaveEvidenceMatrixTriageButtons.removeFirst();
    QCOMPARE(w.slaveEvidenceMatrixTriageButtons.size(), 2);
    QCOMPARE(w.slaveEvidenceMatrixTriageButtons.first(), &b2);

    w.slaveEvidenceMatrixTriageButtons.clear();
    QVERIFY(w.slaveEvidenceMatrixTriageButtons.isEmpty());
}

void WorkspaceWidgetsTest::structCopyPreservesPointers()
{
    IoVariableWorkspaceWidgets w;
    QTableWidget table;
    QLineEdit filter;
    QLabel detail;
    w.ioVariableTable = &table;
    w.ioVariableFilter = &filter;
    w.ioVariableDetailLabel = &detail;

    IoVariableWorkspaceWidgets copy = w;
    QCOMPARE(copy.ioVariableTable, w.ioVariableTable);
    QCOMPARE(copy.ioVariableFilter, w.ioVariableFilter);
    QCOMPARE(copy.ioVariableDetailLabel, w.ioVariableDetailLabel);
    QCOMPARE(copy.ioVariableScopeFilter, nullptr);
    QCOMPARE(copy.ioVariableSummaryLabel, nullptr);
}

QTEST_MAIN(WorkspaceWidgetsTest)
#include "workspace_widgets_test.moc"
