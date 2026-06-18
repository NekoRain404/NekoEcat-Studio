#pragma once

// Commissioning workflow step model with status, scope, and severity.


#include <QVector>

// Workflow step status: Ready (can proceed), Action (needs work), Blocked (prerequisites missing)
enum class CommissioningWorkflowStatus {
  Ready,
  Action,
  Blocked,
};

// Ordered steps in the EtherCAT commissioning workflow
enum class CommissioningWorkflowStep {
  ConnectRuntime = 0,
  ScanTopology = 1,
  SelectSlave = 2,
  InspectObjectDictionary = 3,
  ReviewObjectDictionaryEvidence = 4,
  ReviewPdoMap = 5,
  MonitorWatch = 6,
  ReviewStartupDiffs = 7,
  RunConsistencyGate = 8,
  ValidateProcessImage = 9,
};

// Snapshot of current system state used to evaluate workflow progress
struct CommissioningWorkflowInput {
  bool connected = false;
  bool hasSlaves = false;
  bool hasSelectedSlave = false;
  bool hasSdoRows = false;
  bool hasFailedOdEvidence = false;
  bool hasPdoRows = false;
  bool hasWatchRows = false;
  bool hasStartupWatchDiffs = false;
  bool hasConsistencyCheck = false;
  bool hasConsistencyBlockingIssues = false;
  bool freeRunEnabled = false;
  bool hasFreeRunRows = false;
};

// Result of evaluating a single workflow step
struct CommissioningWorkflowStepState {
  CommissioningWorkflowStep step = CommissioningWorkflowStep::ConnectRuntime;
  CommissioningWorkflowStatus status = CommissioningWorkflowStatus::Blocked;
};

const QVector<CommissioningWorkflowStep> &commissioningWorkflowStepOrder();
CommissioningWorkflowStatus commissioningWorkflowStatus(bool done, bool ready);
QVector<CommissioningWorkflowStepState>
buildCommissioningWorkflowStepStates(const CommissioningWorkflowInput &input);
int nextCommissioningWorkflowStepIndex(const CommissioningWorkflowInput &input);
CommissioningWorkflowStatus commissioningWorkflowStepStatus(
    const QVector<CommissioningWorkflowStepState> &states,
    CommissioningWorkflowStep step);
CommissioningWorkflowStep commissioningWorkflowStepForIndex(int index);
int commissioningWorkflowStepIndex(CommissioningWorkflowStep step);
