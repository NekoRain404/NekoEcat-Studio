// WorkspaceBoundaryTypesTest — Tests for WorkspaceBoundaryDetail types
//
// Test coverage:
//   - WorkspaceBoundaryKind enum values
//   - WorkspaceBoundaryCounts default initialization
//   - WorkspaceBoundaryCounts field assignment
//   - WorkspaceBoundaryDetail default initialization
//   - WorkspaceBoundaryTexts default initialization
//   - buildWorkspaceBoundaryDetail for each boundary kind

#include "detail/WorkspaceBoundaryDetail.h"

#include <QtTest/QtTest>

class WorkspaceBoundaryTypesTest : public QObject {
    Q_OBJECT

private slots:
    void boundaryKindEnumValues();
    void boundaryCountsDefaultZero();
    void boundaryCountsAssignment();
    void boundaryDetailDefaultState();
    void boundaryTextsDefaultEmpty();
    void buildOverviewBoundary();
    void buildObjectDictionaryBoundary();
    void buildPdoMapBoundary();
    void buildWatchBoundary();
    void buildStartupSdoBoundary();
    void buildFreeRunBoundary();
    void buildIoVariablesBoundary();
    void buildConsistencyBoundary();
    void buildStateMachineBoundary();
    void buildDiagnosticsBoundary();
    void buildRtTestBoundary();
    void buildEsiBoundary();
    void buildNotesBoundary();
    void buildRawEvidenceBoundary();
    void buildWithNonZeroCounts();
};

void WorkspaceBoundaryTypesTest::boundaryKindEnumValues() {
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::Overview), 0);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::ObjectDictionary), 1);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::PdoMap), 2);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::Watch), 3);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::StartupSdo), 4);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::FreeRun), 5);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::IoVariables), 6);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::Consistency), 7);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::StateMachine), 8);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::Diagnostics), 9);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::RtTest), 10);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::Esi), 11);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::Notes), 12);
    QCOMPARE(static_cast<int>(WorkspaceBoundaryKind::RawEvidence), 13);
}

void WorkspaceBoundaryTypesTest::boundaryCountsDefaultZero() {
    WorkspaceBoundaryCounts c;
    QCOMPARE(c.matrixP0, 0);
    QCOMPARE(c.matrixP1, 0);
    QCOMPARE(c.matrixP2, 0);
    QCOMPARE(c.matrixP3, 0);
}

void WorkspaceBoundaryTypesTest::boundaryCountsAssignment() {
    WorkspaceBoundaryCounts c;
    c.matrixP0 = 3;
    c.matrixP1 = 5;
    c.matrixP2 = 1;
    c.matrixP3 = 7;
    QCOMPARE(c.matrixP0, 3);
    QCOMPARE(c.matrixP1, 5);
    QCOMPARE(c.matrixP2, 1);
    QCOMPARE(c.matrixP3, 7);
}

void WorkspaceBoundaryTypesTest::boundaryDetailDefaultState() {
    WorkspaceBoundaryDetail d;
    QVERIFY(d.label.isEmpty());
    QVERIFY(d.severityKey.isEmpty());
    QVERIFY(d.details.isEmpty());
    QVERIFY(d.tooltip.isEmpty());
}

void WorkspaceBoundaryTypesTest::boundaryTextsDefaultEmpty() {
    WorkspaceBoundaryTexts t;
    QVERIFY(t.workspacePattern.isEmpty());
    QVERIFY(t.overviewLabel.isEmpty());
    QVERIFY(t.objectDictionaryLabel.isEmpty());
    QVERIFY(t.pdoMapLabel.isEmpty());
    QVERIFY(t.watchLabel.isEmpty());
    QVERIFY(t.startupSdoLabel.isEmpty());
    QVERIFY(t.freeRunLabel.isEmpty());
    QVERIFY(t.ioVariablesLabel.isEmpty());
    QVERIFY(t.consistencyLabel.isEmpty());
    QVERIFY(t.stateMachineLabel.isEmpty());
    QVERIFY(t.diagnosticsLabel.isEmpty());
    QVERIFY(t.rtTestLabel.isEmpty());
    QVERIFY(t.esiLabel.isEmpty());
    QVERIFY(t.notesLabel.isEmpty());
    QVERIFY(t.rawEvidenceLabel.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildOverviewBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.overviewLabel = QStringLiteral("Overview");
    texts.overviewMixedActions = QStringLiteral("mixed actions");
    texts.overviewLocalEvidence = QStringLiteral("local evidence");
    texts.overviewMatrixPattern = QStringLiteral("P0:%1 P1:%2 P2:%3 P3:%4");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d =
        buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::Overview, "Overview", counts, texts);
    QVERIFY(!d.label.isEmpty());
    QVERIFY(!d.severityKey.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildObjectDictionaryBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.objectDictionaryLabel = QStringLiteral("Object Dictionary");
    texts.objectDictionaryLocalFill = QStringLiteral("local fill");
    texts.objectDictionaryOnlineAccess = QStringLiteral("online");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d =
        buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::ObjectDictionary, "OD", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildPdoMapBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.pdoMapLabel = QStringLiteral("PDO Map");
    texts.pdoMapLoadedLocal = QStringLiteral("loaded");
    texts.pdoMapLocalFill = QStringLiteral("local");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d = buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::PdoMap, "PDO", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildWatchBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.watchLabel = QStringLiteral("Watch");
    texts.watchReadsOnline = QStringLiteral("online");
    texts.watchStartupLocal = QStringLiteral("local");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d = buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::Watch, "Watch", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildStartupSdoBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.startupSdoLabel = QStringLiteral("Startup SDO");
    texts.startupSdoLocalEditing = QStringLiteral("local");
    texts.startupSdoOnlineApply = QStringLiteral("online");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d =
        buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::StartupSdo, "Startup", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildFreeRunBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.freeRunLabel = QStringLiteral("Free Run");
    texts.freeRunProcessData = QStringLiteral("process data");
    texts.freeRunLocalFiltering = QStringLiteral("local");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d = buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::FreeRun, "FreeRun", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildIoVariablesBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.ioVariablesLabel = QStringLiteral("I/O Variables");
    texts.ioVariablesMergedEvidence = QStringLiteral("merged");
    texts.ioVariablesLocalEditing = QStringLiteral("local");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d = buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::IoVariables, "IO", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildConsistencyBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.consistencyLabel = QStringLiteral("Consistency");
    texts.consistencyLoadedEvidence = QStringLiteral("loaded");
    texts.consistencyLocalNavigation = QStringLiteral("local");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d =
        buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::Consistency, "Consistency", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildStateMachineBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.stateMachineLabel = QStringLiteral("State Machine");
    texts.stateMachineOnlineRequests = QStringLiteral("online");
    texts.stateMachineConfirmation = QStringLiteral("confirm");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d = buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::StateMachine, "SM", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildDiagnosticsBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.diagnosticsLabel = QStringLiteral("Diagnostics");
    texts.diagnosticsHostOnly = QStringLiteral("host");
    texts.diagnosticsHostCheck = QStringLiteral("check");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d = buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::Diagnostics, "Diag", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildRtTestBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.rtTestLabel = QStringLiteral("RT Test");
    texts.rtTestOnlineCycle = QStringLiteral("online");
    texts.rtTestLocalStats = QStringLiteral("local");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d = buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::RtTest, "RT", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildEsiBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.esiLabel = QStringLiteral("ESI");
    texts.esiFileEvidence = QStringLiteral("file");
    texts.esiImportAction = QStringLiteral("import");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d = buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::Esi, "ESI", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildNotesBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.notesLabel = QStringLiteral("Notes");
    texts.notesLocalRecords = QStringLiteral("local");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d = buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::Notes, "Notes", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildRawEvidenceBoundary() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.rawEvidenceLabel = QStringLiteral("Raw Evidence");
    texts.rawEvidenceCachedOutput = QStringLiteral("cached");

    WorkspaceBoundaryCounts counts;
    WorkspaceBoundaryDetail d = buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::RawEvidence, "Raw", counts, texts);
    QVERIFY(!d.label.isEmpty());
}

void WorkspaceBoundaryTypesTest::buildWithNonZeroCounts() {
    WorkspaceBoundaryTexts texts;
    texts.workspacePattern = QStringLiteral("Workspace: %1");
    texts.overviewLabel = QStringLiteral("Overview");
    texts.overviewMixedActions = QStringLiteral("mixed");
    texts.overviewLocalEvidence = QStringLiteral("local");
    texts.overviewMatrixPattern = QStringLiteral("P0:%1 P1:%2 P2:%3 P3:%4");

    WorkspaceBoundaryCounts counts;
    counts.matrixP0 = 2;
    counts.matrixP1 = 3;
    counts.matrixP2 = 1;
    counts.matrixP3 = 0;
    WorkspaceBoundaryDetail d =
        buildWorkspaceBoundaryDetail(WorkspaceBoundaryKind::Overview, "Overview", counts, texts);
    QVERIFY(!d.label.isEmpty());
    QVERIFY(!d.severityKey.isEmpty());
    QVERIFY(!d.details.isEmpty());
}

QTEST_MAIN(WorkspaceBoundaryTypesTest)
#include "workspace_boundary_types_test.moc"
