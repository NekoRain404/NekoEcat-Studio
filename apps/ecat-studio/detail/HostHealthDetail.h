#pragma once

// Host health check result rows for the diagnostics overview.


#include <QJsonArray>
#include <QList>
#include <QString>
#include <QStringList>

// Localized strings for host health check headers and summary labels.
struct HostHealthTexts {
  QString levelHeader;
  QString checkHeader;
  QString resultHeader;
  QString actionHeader;
  QString commandHeader;
  QString detailHeader;
  QString unchecked;
  QString needsAttention;
  QString usableWithWarnings;
  QString ready;
  QString warningLabel;
  QString okLabel;
};

// Resolved host health state with rows, counts, color keys, and summary.
struct HostHealthDetail {
  QStringList headers;
  QList<QStringList> rows;
  QStringList colorKeys;
  QString summary;
  int errors = 0;
  int warnings = 0;
  int infos = 0;
};

QString hostHealthColorKey(const QString &level);
HostHealthDetail buildHostHealthDetail(const QJsonArray &checks,
                                         const HostHealthTexts &texts);
