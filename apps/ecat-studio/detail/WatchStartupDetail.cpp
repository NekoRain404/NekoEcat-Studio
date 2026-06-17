// Summary text for the Watch and Startup SDO workspace panels.
#include "detail/WatchStartupDetail.h"

#include <QBrush>
#include <QColor>
#include <QTableWidgetItem>

// Resets both the value and delta cells to empty with default styling.
void clearWatchStartupDeltaCells(QTableWidgetItem *valueItem,
                                 QTableWidgetItem *deltaItem) {
  if (valueItem) {
    valueItem->setText(QString());
    valueItem->setBackground(QBrush());
    valueItem->setForeground(QBrush());
  }
  if (deltaItem) {
    deltaItem->setText(QString());
    deltaItem->setBackground(QBrush());
    deltaItem->setForeground(QBrush());
  }
}

// Applies theme-aware colors and text to the value/delta cells based on the comparison result.
void applyWatchStartupDeltaCells(QTableWidgetItem *valueItem,
                                 QTableWidgetItem *deltaItem,
                                 WatchStartupDeltaState state,
                                 const QString &watchValue,
                                 const WatchStartupDeltaTexts &texts,
                                 bool lightTheme) {
  if (valueItem) {
    valueItem->setText(watchValue);
  }
  if (!deltaItem) {
    return;
  }

  if (state == WatchStartupDeltaState::MissingWatch) {
    deltaItem->setText(texts.missingWatch);
    deltaItem->setForeground(lightTheme ? QColor("#64748b")
                                        : QColor("#b9c6d6"));
    return;
  }

  if (state == WatchStartupDeltaState::Pending) {
    deltaItem->setText(texts.pending);
    deltaItem->setBackground(lightTheme ? QColor("#eef2f7")
                                        : QColor("#1a2230"));
    deltaItem->setForeground(lightTheme ? QColor("#64748b")
                                        : QColor("#b9c6d6"));
    return;
  }

  const bool same = state == WatchStartupDeltaState::Match;
  deltaItem->setText(same ? texts.match : texts.diff);
  deltaItem->setBackground(
      same ? (lightTheme ? QColor("#dcfce7") : QColor("#12351f"))
           : (lightTheme ? QColor("#fee2e2") : QColor("#3a1218")));
  deltaItem->setForeground(
      same ? (lightTheme ? QColor("#166534") : QColor("#86efac"))
           : (lightTheme ? QColor("#991b1b") : QColor("#fca5a5")));
}
