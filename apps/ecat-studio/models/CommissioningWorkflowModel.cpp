// Commissioning workflow step model with status, scope, and severity.
#include "CommissioningWorkflowModel.h"

// Canonical step sequence for the commissioning wizard; drives ordering everywhere.
const QVector<CommissioningWorkflowStep> &commissioningWorkflowStepOrder() {
  static const QVector<CommissioningWorkflowStep> order = {
      CommissioningWorkflowStep::ConnectRuntime,
      CommissioningWorkflowStep::ScanTopology,
      CommissioningWorkflowStep::SelectSlave,
      CommissioningWorkflowStep::InspectObjectDictionary,
      CommissioningWorkflowStep::ReviewObjectDictionaryEvidence,
      CommissioningWorkflowStep::ReviewPdoMap,
      CommissioningWorkflowStep::MonitorWatch,
      CommissioningWorkflowStep::ReviewStartupDiffs,
      CommissioningWorkflowStep::RunConsistencyGate,
      CommissioningWorkflowStep::ValidateProcessImage,
  };
  return order;
}

// Derives tri-state status from done/ready boolean flags.
CommissioningWorkflowStatus commissioningWorkflowStatus(bool done, bool ready) {
  if (done) {
    return CommissioningWorkflowStatus::Ready;
  }
  return ready ? CommissioningWorkflowStatus::Action
               : CommissioningWorkflowStatus::Blocked;
}

// Evaluates each workflow step against current session inputs to determine its status.
QVector<CommissioningWorkflowStepState>
buildCommissioningWorkflowStepStates(const CommissioningWorkflowInput &input) {
  QVector<CommissioningWorkflowStepState> states;
  const QVector<CommissioningWorkflowStep> &order =
      commissioningWorkflowStepOrder();
  states.reserve(order.size());
  for (const CommissioningWorkflowStep step : order) {
    switch (step) {
    case CommissioningWorkflowStep::ConnectRuntime:
      states.append({step, commissioningWorkflowStatus(input.connected, true)});
      break;
    case CommissioningWorkflowStep::ScanTopology:
      states.append({step, commissioningWorkflowStatus(input.hasSlaves,
                                                       input.connected)});
      break;
    case CommissioningWorkflowStep::SelectSlave:
      states.append({step, commissioningWorkflowStatus(input.hasSelectedSlave,
                                                       input.hasSlaves)});
      break;
    case CommissioningWorkflowStep::InspectObjectDictionary:
      states.append({step, commissioningWorkflowStatus(
                               input.hasSdoRows, input.hasSelectedSlave)});
      break;
    case CommissioningWorkflowStep::ReviewObjectDictionaryEvidence:
      states.append({step, commissioningWorkflowStatus(
                               input.hasSdoRows && !input.hasFailedOdEvidence,
                               input.hasSdoRows)});
      break;
    case CommissioningWorkflowStep::ReviewPdoMap:
      states.append({step, commissioningWorkflowStatus(
                               input.hasPdoRows, input.hasSelectedSlave)});
      break;
    case CommissioningWorkflowStep::MonitorWatch:
      states.append({step, commissioningWorkflowStatus(
                               input.hasWatchRows, input.hasSelectedSlave)});
      break;
    case CommissioningWorkflowStep::ReviewStartupDiffs:
      states.append(
          {step, commissioningWorkflowStatus(input.hasWatchRows &&
                                                 !input.hasStartupWatchDiffs,
                                             input.hasWatchRows)});
      break;
    case CommissioningWorkflowStep::RunConsistencyGate:
      states.append(
          {step,
           commissioningWorkflowStatus(
               input.hasConsistencyCheck && !input.hasConsistencyBlockingIssues,
               input.hasWatchRows && !input.hasStartupWatchDiffs)});
      break;
    case CommissioningWorkflowStep::ValidateProcessImage:
      states.append({step, commissioningWorkflowStatus(
                               input.freeRunEnabled || input.hasFreeRunRows,
                               input.connected && input.hasSelectedSlave)});
      break;
    }
  }
  return states;
}

// Finds the first non-ready step so the UI can focus the user on the next action.
int nextCommissioningWorkflowStepIndex(
    const CommissioningWorkflowInput &input) {
  const QVector<CommissioningWorkflowStepState> states =
      buildCommissioningWorkflowStepStates(input);
  for (int row = 0; row < states.size(); ++row) {
    if (states.at(row).status != CommissioningWorkflowStatus::Ready) {
      return row;
    }
  }
  return -1;
}

// Looks up a specific step's status in the precomputed state list.
CommissioningWorkflowStatus commissioningWorkflowStepStatus(
    const QVector<CommissioningWorkflowStepState> &states,
    CommissioningWorkflowStep step) {
  for (const auto &state : states) {
    if (state.step == step) {
      return state.status;
    }
  }
  return CommissioningWorkflowStatus::Blocked;
}

// Safe index-to-enum conversion with a sensible fallback.
CommissioningWorkflowStep commissioningWorkflowStepForIndex(int index) {
  const QVector<CommissioningWorkflowStep> &order =
      commissioningWorkflowStepOrder();
  if (index >= 0 && index < order.size()) {
    return order.at(index);
  }
  return CommissioningWorkflowStep::ConnectRuntime;
}

// Reverse lookup: returns the ordinal position of a step enum in the canonical order.
int commissioningWorkflowStepIndex(CommissioningWorkflowStep step) {
  const QVector<CommissioningWorkflowStep> &order =
      commissioningWorkflowStepOrder();
  return order.indexOf(step);
}
