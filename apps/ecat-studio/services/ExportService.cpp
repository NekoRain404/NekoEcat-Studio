#include "ExportService.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QTextStream>

// ExportService.cpp — Exports Qt widget data to CSV, JSON, and plain text files
//
// Implementation notes:
//   - CSV export supports configurable delimiter, line endings, and string quoting
//   - JSON export converts QTableWidget rows to array-of-objects with header keys
//   - Text export dumps QPlainTextEdit content as-is to file

ExportService::ExportService(QObject *parent) : QObject(parent) {}

bool ExportService::exportToCsv(QTableWidget *table, const QString &path) {
  if (!table) {
    emit exportFailed(QStringLiteral("Null table widget"));
    return false;
  }

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    emit exportFailed(QStringLiteral("Cannot open file: %1").arg(path));
    return false;
  }

  QTextStream out(&file);
  const int rows = table->rowCount();
  const int cols = table->columnCount();

  if (options_.includeHeaders) {
    for (int c = 0; c < cols; ++c) {
      if (c > 0) out << options_.delimiter;
      auto *item = table->horizontalHeaderItem(c);
      QString text = item ? item->text() : QString();
      if (options_.quoteStrings)
        out << "\"" << text.replace("\"", "\"\"") << "\"";
      else
        out << text;
    }
    out << options_.lineEnding;
  }

  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (c > 0) out << options_.delimiter;
      auto *item = table->item(r, c);
      QString text = item ? item->text() : QString();
      if (options_.quoteStrings)
        out << "\"" << text.replace("\"", "\"\"") << "\"";
      else
        out << text;
    }
    out << options_.lineEnding;
  }

  file.close();
  emit exportCompleted(path);
  return true;
}

bool ExportService::exportToJson(QTableWidget *table, const QString &path) {
  if (!table) {
    emit exportFailed(QStringLiteral("Null table widget"));
    return false;
  }

  QJsonArray rows;
  const int rowCount = table->rowCount();
  const int colCount = table->columnCount();

  QStringList headers;
  if (options_.includeHeaders) {
    for (int c = 0; c < colCount; ++c) {
      auto *item = table->horizontalHeaderItem(c);
      headers << (item ? item->text() : QString());
    }
  }

  for (int r = 0; r < rowCount; ++r) {
    QJsonObject row;
    for (int c = 0; c < colCount; ++c) {
      auto *item = table->item(r, c);
      QString value = item ? item->text() : QString();
      QString key = (c < headers.size()) ? headers[c]
                                         : QString::number(c);
      row[key] = value;
    }
    rows.append(row);
  }

  QJsonDocument doc(rows);
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly)) {
    emit exportFailed(QStringLiteral("Cannot open file: %1").arg(path));
    return false;
  }
  file.write(doc.toJson(QJsonDocument::Indented));
  file.close();
  emit exportCompleted(path);
  return true;
}

bool ExportService::exportToText(QPlainTextEdit *editor, const QString &path) {
  if (!editor) {
    emit exportFailed(QStringLiteral("Null editor widget"));
    return false;
  }

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    emit exportFailed(QStringLiteral("Cannot open file: %1").arg(path));
    return false;
  }
  file.write(editor->toPlainText().toUtf8());
  file.close();
  emit exportCompleted(path);
  return true;
}

void ExportService::setExportOptions(const ExportOptions &options) {
  options_ = options;
}

ExportOptions ExportService::exportOptions() const { return options_; }
