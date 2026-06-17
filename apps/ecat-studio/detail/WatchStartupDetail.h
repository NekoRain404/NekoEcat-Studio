#pragma once

// Summary text for the Watch and Startup SDO workspace panels.


#include "models/WatchStartupModel.h"

#include <QString>

class QTableWidgetItem;

// Localized labels for the four delta states: missing, pending, match, diff.
struct WatchStartupDeltaTexts {
  QString missingWatch;
  QString pending;
  QString match;
  QString diff;
};

// Resets value and delta cells to empty with default styling.
void clearWatchStartupDeltaCells(QTableWidgetItem *valueItem,
                                 QTableWidgetItem *deltaItem);
// Applies themed colors and text based on the comparison result.
void applyWatchStartupDeltaCells(QTableWidgetItem *valueItem,
                                 QTableWidgetItem *deltaItem,
                                 WatchStartupDeltaState state,
                                 const QString &watchValue,
                                 const WatchStartupDeltaTexts &texts,
                                 bool lightTheme);
