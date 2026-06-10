#pragma once

#include <QString>

struct EthercatStateEvidence {
  QString currentState;
  bool pdoLoaded = false;
  int watchValueRows = 0;
  int startupDiffs = 0;
  int freeRunRows = 0;
  int mapIssues = 0;
  bool consistencyOk = false;
};

QString recommendedEthercatState(const EthercatStateEvidence &evidence);
