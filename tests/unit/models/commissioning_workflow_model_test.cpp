// CommissioningWorkflowModelTest — Tests for CommissioningWorkflowModel
//
// Test coverage:
//   - Ready workflow state
//   - Next step ordering logic
//   - Step state transitions
//   - Workflow input validation

#include "models/CommissioningWorkflowModel.h"

#include <QCoreApplication>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString& message) {
    std::cerr << message.toStdString() << '\n';
    std::exit(1);
}

void expectEqual(int actual, int expected, const QString& message) {
    if (actual != expected) {
        fail(QString("%1: expected %2, got %3").arg(message).arg(expected).arg(actual));
    }
}

void expectStatus(CommissioningWorkflowStatus actual, CommissioningWorkflowStatus expected, const QString& message) {
    if (actual != expected) {
        fail(message);
    }
}

CommissioningWorkflowInput readyInput() {
    CommissioningWorkflowInput input;
    input.connected = true;
    input.hasSlaves = true;
    input.hasSelectedSlave = true;
    input.hasSdoRows = true;
    input.hasPdoRows = true;
    input.hasWatchRows = true;
    input.hasConsistencyCheck = true;
    input.freeRunEnabled = true;
    return input;
}

// Verify ready input produces all-ready workflow steps
void testReadyWorkflow() {
    const QVector<CommissioningWorkflowStepState> states = buildCommissioningWorkflowStepStates(readyInput());
    expectEqual(states.size(), 10, "workflow step count");
    for (const auto& state : states) {
        expectStatus(state.status, CommissioningWorkflowStatus::Ready, "ready input produces ready step");
    }
    expectEqual(nextCommissioningWorkflowStepIndex(readyInput()), -1, "ready workflow has no next step");
}

// Verify next step ordering based on input state
void testNextStepOrdering() {
    CommissioningWorkflowInput input;
    expectEqual(nextCommissioningWorkflowStepIndex(input), 0, "offline runtime is first next step");

    input = readyInput();
    input.hasSdoRows = false;
    expectEqual(nextCommissioningWorkflowStepIndex(input), 3, "missing OD is next before OD evidence review");

    input = readyInput();
    input.hasFailedOdEvidence = true;
    expectEqual(nextCommissioningWorkflowStepIndex(input), 4, "failed OD evidence is next after loaded OD");

    input = readyInput();
    input.hasStartupWatchDiffs = true;
    expectEqual(nextCommissioningWorkflowStepIndex(input), 7, "Startup diffs are next before consistency gate");

    input = readyInput();
    input.hasConsistencyCheck = false;
    expectEqual(nextCommissioningWorkflowStepIndex(input), 8, "missing consistency check is next before process image");

    input = readyInput();
    input.freeRunEnabled = false;
    input.hasFreeRunRows = false;
    expectEqual(nextCommissioningWorkflowStepIndex(input), 9, "missing process image is last next step");
}

// Verify step status lookup for specific steps
void testStepStatusLookup() {
    CommissioningWorkflowInput input = readyInput();
    input.hasPdoRows = false;
    const QVector<CommissioningWorkflowStepState> states = buildCommissioningWorkflowStepStates(input);
    expectStatus(commissioningWorkflowStepStatus(states, CommissioningWorkflowStep::ReviewPdoMap),
                 CommissioningWorkflowStatus::Action, "missing PDO map is actionable when slave is selected");
    expectStatus(commissioningWorkflowStepStatus(states, CommissioningWorkflowStep::MonitorWatch),
                 CommissioningWorkflowStatus::Ready, "watch step remains ready");
}

// Verify step index mapping and order count
void testStepIndexMapping() {
    expectEqual(commissioningWorkflowStepOrder().size(), 10, "workflow step order count");
    expectEqual(commissioningWorkflowStepIndex(CommissioningWorkflowStep::ConnectRuntime), 0, "connect runtime index");
    expectEqual(commissioningWorkflowStepIndex(CommissioningWorkflowStep::ValidateProcessImage), 9,
                "process image index");
    expectEqual(commissioningWorkflowStepIndex(commissioningWorkflowStepForIndex(7)), 7,
                "row index maps through Startup diffs step");
}

} // namespace

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    testReadyWorkflow();
    testNextStepOrdering();
    testStepStatusLookup();
    testStepIndexMapping();
    return 0;
}
