#pragma once

// Session summary row model: slave counts, issues, topology, and runtime health.


#include <QString>
#include <QVector>

// Type of summary row in the session brief panel
enum class SessionBriefRowKind {
  Target,
  Gate,
  Map,
  CurrentSdo,
  RuntimeEvidence,
  Next,
};

// Visual status for a summary row
enum class SessionBriefStatus {
  Ready,
  Action,
  Warning,
  Error,
  Info,
};

// Aggregated state from all workspaces for summary generation
struct SessionBriefInput {
  bool connected = false;
  bool hasSlaves = false;
  bool hasSelectedSlave = false;
  bool hasSdoRows = false;
  bool hasPdoRows = false;
  bool hasFailedOdEvidence = false;
  bool hasConsistencyCheck = false;
  int consistencyErrors = 0;
  int consistencyWarnings = 0;
  bool currentSdoComplete = false;
  int currentSdoEvidenceGroups = 0;
  bool currentSdoHasEvidence = false;
  bool currentSdoWriteDiffers = false;
  bool currentSdoWriteMatches = false;
  bool hasWatchRows = false;
  bool freeRunEnabled = false;
  bool hasFreeRunRows = false;
  int startupDiffs = 0;
  int nextWorkflowStep = -1;
};

// A single row in the session brief summary
struct SessionBriefRow {
  SessionBriefRowKind kind = SessionBriefRowKind::Target;
  QString actionKey;
  SessionBriefStatus status = SessionBriefStatus::Info;
};

QVector<SessionBriefRow> buildSessionBriefRows(const SessionBriefInput &input);
QString sessionBriefStatusKey(SessionBriefStatus status);
