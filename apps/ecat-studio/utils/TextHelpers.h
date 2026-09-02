#pragma once

// Text formatting helpers: hex normalization, value decoding, address display.


#include <QString>

// Reformat a hex string with zero-padded digits for consistent comparison.
QString normalizeHexText(QString text, int minimumDigits);
// Strip whitespace and lowercase for value comparison.
QString normalizeComparableValue(QString text);
// Convert display text to a PLC-safe identifier.
QString plcIdentifier(QString text, const QString& fallback);
// First capture group from a regex, or empty string.
QString capture(const QString& text, const QString& pattern);
