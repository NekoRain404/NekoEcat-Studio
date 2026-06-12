#pragma once

// Text formatting helpers: hex normalization, value decoding, address display.


#include <QString>

QString normalizeHexText(QString text, int minimumDigits);
QString normalizeComparableValue(QString text);
QString plcIdentifier(QString text, const QString &fallback);
QString capture(const QString &text, const QString &pattern);
