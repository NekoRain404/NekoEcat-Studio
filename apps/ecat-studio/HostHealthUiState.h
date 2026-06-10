#pragma once

#include <QJsonArray>
#include <QList>
#include <QString>
#include <QStringList>

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

struct HostHealthUiState {
  QStringList headers;
  QList<QStringList> rows;
  QStringList colorKeys;
  QString summary;
  int errors = 0;
  int warnings = 0;
  int infos = 0;
};

QString hostHealthColorKey(const QString &level);
HostHealthUiState buildHostHealthUiState(const QJsonArray &checks,
                                         const HostHealthTexts &texts);
