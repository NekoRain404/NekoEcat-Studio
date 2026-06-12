#pragma once

// Per-slave evidence row model for the commissioning evidence matrix.


#include <QString>
#include <QVector>

enum class SlaveEvidencePriority {
  Fault,
  Risk,
  Action,
  Ready,
};

enum class SlaveEvidenceRiskKind {
  IdentityMissing,
  OdMissing,
  PdoMissing,
  WatchMissing,
  ProcessMissing,
  StartupDiff,
  PdoMapIssue,
  TopologyBaselineIssue,
  DriveFault,
};

enum class SlaveEvidenceNextAction {
  ReviewOd,
  LoadPdo,
  AddWatch,
  ReviewStartup,
  ValidateProcess,
  ReviewRisk,
  Ready,
};

enum class SlaveEvidenceRouteTarget {
  Overview,
  ObjectDictionary,
  PdoMap,
  Watch,
  Startup,
  Process,
  StateMachine,
};

struct SlaveEvidenceInput {
  int position = -1;
  QString name;
  QString state;
  int identityRows = 0;
  int odRows = 0;
  int pdoRows = 0;
  int watchRows = 0;
  int watchValueRows = 0;
  int startupRows = 0;
  int startupDiffs = 0;
  int processRows = 0;
  int mapIssues = 0;
  bool topologyIssue = false;
  bool driveFault = false;
  QString driveStatusword;
  QString driveModeDisplay;
  QString driveErrorCode;
  QString driveEvidence;
};

struct SlaveEvidenceRisk {
  SlaveEvidenceRiskKind kind = SlaveEvidenceRiskKind::IdentityMissing;
  int count = 0;
};

struct SlaveEvidenceRow {
  int position = -1;
  QString name;
  QString state;
  int readiness = 0;
  int maxReadiness = 6;
  int identityRows = 0;
  int odRows = 0;
  int pdoRows = 0;
  int watchRows = 0;
  int watchValueRows = 0;
  int startupRows = 0;
  int startupDiffs = 0;
  int processRows = 0;
  int mapIssues = 0;
  QString driveStatusword;
  QString driveModeDisplay;
  QString driveErrorCode;
  QString driveEvidence;
  QVector<SlaveEvidenceRisk> risks;
  SlaveEvidenceNextAction nextAction = SlaveEvidenceNextAction::ReviewOd;
  SlaveEvidencePriority priority = SlaveEvidencePriority::Action;
};

struct SlaveEvidenceMatrix {
  QVector<SlaveEvidenceRow> rows;
  int readyRows = 0;
  int actionRows = 0;
  int riskRows = 0;
  int evidenceGaps = 0;
};

SlaveEvidenceMatrix
buildSlaveEvidenceMatrix(const QVector<SlaveEvidenceInput> &inputs);
int slaveEvidencePriorityRank(SlaveEvidencePriority priority);
int slaveEvidenceReadinessPercent(const SlaveEvidenceRow &row);
SlaveEvidenceRouteTarget slaveEvidenceRouteTarget(const SlaveEvidenceRow &row);
