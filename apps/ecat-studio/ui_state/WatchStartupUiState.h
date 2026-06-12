#pragma once

// Summary text for the Watch and Startup SDO workspace panels.


#include "models/WatchStartupModel.h"

#include <QString>

class QTableWidgetItem;

struct WatchStartupDeltaTexts {
  QString missingWatch;
  QString pending;
  QString match;
  QString diff;
};

void clearWatchStartupDeltaCells(QTableWidgetItem *valueItem,
                                 QTableWidgetItem *deltaItem);
void applyWatchStartupDeltaCells(QTableWidgetItem *valueItem,
                                 QTableWidgetItem *deltaItem,
                                 WatchStartupDeltaState state,
                                 const QString &watchValue,
                                 const WatchStartupDeltaTexts &texts,
                                 bool lightTheme);
