// MainWindowSession.h — Session brief and commissioning workflow (partial class fragment).
// Included inside the MainWindow class body in MainWindow.h.
//
// Manages the commissioning session lifecycle. The session brief summarizes
// the current commissioning state, while the workflow steps guide the user
// through each phase (topology discovery, SDO verification, I/O mapping,
// consistency checks, etc.). Also covers the slave evidence matrix -- a per-
// slave triage view that aggregates evidence from multiple sources and tracks
// which issues have been reviewed.

// ── Session Brief ─────────────────────────────────────────────
void updateSessionBrief();
void openSessionBriefRow(int row);
void updateSessionBriefCopyButton();
bool copySessionBriefRowDigest(int row);

// ── Commissioning Workflow ────────────────────────────────────
void filterCommissioningWorkflow();
void reviewFirstCommissioningWorkflowIssue();
void reviewNextCommissioningWorkflowIssue();
void updateWorkflowStepCopyButton();
void updateWorkflowStepDetail();
bool copyWorkflowStepDigest(int row);

// ── Slave Evidence Matrix ─────────────────────────────────────
void updateSlaveEvidenceMatrix();
void filterSlaveEvidenceMatrix();
void openSlaveEvidenceMatrixRow(int row);
void reviewFirstSlaveEvidenceMatrixIssue();
void reviewNextSlaveEvidenceMatrixIssue();
bool copySlaveEvidenceMatrixRowDigest(int row);
int slaveEvidenceMatrixRowForPosition(int position) const;
CommissioningWorkflowInput commissioningWorkflowInput() const;
void updateSlaveEvidenceMatrixTriageButtons();

// ── Commissioning State ───────────────────────────────────────
void updateCommissioningWorkflow();
int nextCommissioningWorkflowStep() const;
void runNextCommissioningWorkflowStep();
void runCommissioningWorkflowStep(int row);

// ── Evidence & Snapshots ──────────────────────────────────────
void prepareSelectedSlaveSnapshot();
void beginSelectedSlaveOnlineLoad(int position);
