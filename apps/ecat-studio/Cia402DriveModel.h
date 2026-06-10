#pragma once

#include <QString>

struct Cia402ControlwordRecommendation {
  QString label;
  QString value;
  QString reason;
};

Cia402ControlwordRecommendation
recommendedCia402ControlwordFromStatus(const QString &decodedStatusword);

bool isCia402Object(const QString &index, const QString &mode = QString());
QString decodeCia402Value(const QString &index, const QString &value);
