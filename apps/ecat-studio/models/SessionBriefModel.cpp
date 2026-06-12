#include "SessionBriefModel.h"

namespace {

SessionBriefStatus targetStatus(const SessionBriefInput &input) {
  return input.connected && input.hasSlaves && input.hasSelectedSlave
             ? SessionBriefStatus::Ready
             : SessionBriefStatus::Action;
}

SessionBriefStatus gateStatus(const SessionBriefInput &input) {
  if (!input.hasConsistencyCheck) {
    return SessionBriefStatus::Action;
  }
  if (input.consistencyErrors > 0) {
    return SessionBriefStatus::Error;
  }
  if (input.consistencyWarnings > 0) {
    return SessionBriefStatus::Warning;
  }
  return SessionBriefStatus::Ready;
}

SessionBriefStatus mapStatus(const SessionBriefInput &input) {
  if (!input.hasSelectedSlave || !input.hasSdoRows || !input.hasPdoRows) {
    return SessionBriefStatus::Action;
  }
  return input.hasFailedOdEvidence ? SessionBriefStatus::Warning
                                   : SessionBriefStatus::Ready;
}

SessionBriefStatus currentSdoStatus(const SessionBriefInput &input) {
  if (!input.currentSdoComplete) {
    return SessionBriefStatus::Action;
  }
  if (input.currentSdoEvidenceGroups > 1 ||
      (input.currentSdoWriteDiffers && !input.currentSdoWriteMatches)) {
    return SessionBriefStatus::Warning;
  }
  return input.currentSdoHasEvidence ? SessionBriefStatus::Ready
                                     : SessionBriefStatus::Action;
}

SessionBriefStatus runtimeStatus(const SessionBriefInput &input) {
  if (input.startupDiffs > 0) {
    return SessionBriefStatus::Warning;
  }
  if (!input.hasWatchRows || (!input.freeRunEnabled && !input.hasFreeRunRows)) {
    return SessionBriefStatus::Action;
  }
  return SessionBriefStatus::Ready;
}

SessionBriefStatus nextStatus(const SessionBriefInput &input) {
  return input.nextWorkflowStep >= 0 ? SessionBriefStatus::Action
                                     : SessionBriefStatus::Ready;
}

QString actionKey(SessionBriefRowKind kind) {
  switch (kind) {
  case SessionBriefRowKind::Target:
    return QStringLiteral("target");
  case SessionBriefRowKind::Gate:
    return QStringLiteral("gate");
  case SessionBriefRowKind::Map:
    return QStringLiteral("map");
  case SessionBriefRowKind::CurrentSdo:
    return QStringLiteral("currentSdo");
  case SessionBriefRowKind::RuntimeEvidence:
    return QStringLiteral("runtime");
  case SessionBriefRowKind::Next:
    return QStringLiteral("next");
  }
  return QString();
}

} // namespace

QString sessionBriefStatusKey(SessionBriefStatus status) {
  switch (status) {
  case SessionBriefStatus::Ready:
    return QStringLiteral("ready");
  case SessionBriefStatus::Action:
    return QStringLiteral("action");
  case SessionBriefStatus::Warning:
    return QStringLiteral("warning");
  case SessionBriefStatus::Error:
    return QStringLiteral("error");
  case SessionBriefStatus::Info:
    return QStringLiteral("info");
  }
  return QStringLiteral("info");
}

QVector<SessionBriefRow> buildSessionBriefRows(const SessionBriefInput &input) {
  return {
      {SessionBriefRowKind::Target, actionKey(SessionBriefRowKind::Target),
       targetStatus(input)},
      {SessionBriefRowKind::Gate, actionKey(SessionBriefRowKind::Gate),
       gateStatus(input)},
      {SessionBriefRowKind::Map, actionKey(SessionBriefRowKind::Map),
       mapStatus(input)},
      {SessionBriefRowKind::CurrentSdo,
       actionKey(SessionBriefRowKind::CurrentSdo), currentSdoStatus(input)},
      {SessionBriefRowKind::RuntimeEvidence,
       actionKey(SessionBriefRowKind::RuntimeEvidence), runtimeStatus(input)},
      {SessionBriefRowKind::Next, actionKey(SessionBriefRowKind::Next),
       nextStatus(input)},
  };
}
