#include "StudioTextHelpers.h"

#include <QRegularExpression>

QString normalizeHexText(QString text, int minimumDigits) {
  text = text.trimmed();
  if (text.isEmpty()) {
    return text;
  }
  QString digits = text;
  if (digits.startsWith("0x", Qt::CaseInsensitive)) {
    digits = digits.mid(2);
  }
  bool ok = false;
  const quint64 parsed = digits.toULongLong(&ok, 16);
  if (!ok) {
    return text.toLower();
  }
  return QString("0x%1")
      .arg(parsed, minimumDigits, 16, QLatin1Char('0'))
      .toLower();
}

QString normalizeComparableValue(QString text) {
  return text.trimmed().remove(' ').toLower();
}

QString plcIdentifier(QString text, const QString &fallback) {
  text = text.trimmed();
  if (text.isEmpty()) {
    text = fallback.trimmed();
  }
  text.replace('&', " and ");
  text.replace('+', " plus ");
  text.replace('%', " percent ");
  text.replace(QRegularExpression("[^A-Za-z0-9_]+"), "_");
  text.replace(QRegularExpression("_+"), "_");
  text = text.trimmed();
  while (text.startsWith('_')) {
    text.remove(0, 1);
  }
  while (text.endsWith('_')) {
    text.chop(1);
  }
  if (text.isEmpty()) {
    text = fallback.trimmed();
  }
  if (text.isEmpty()) {
    text = QStringLiteral("Var");
  }
  if (text.front().isDigit()) {
    text.prepend("V_");
  }
  return text;
}

QString capture(const QString &text, const QString &pattern) {
  const auto match =
      QRegularExpression(pattern, QRegularExpression::MultilineOption)
          .match(text);
  return match.hasMatch() ? match.captured(1).trimmed() : QString();
}
