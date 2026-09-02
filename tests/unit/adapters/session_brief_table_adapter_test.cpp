// SessionBriefTableAdapterTest — Tests for SessionBriefTableAdapter
//
// Test coverage:
//   - Structured row extraction from QTableWidget
//   - Invalid row handling and legacy action key fallback

#include "adapters/SessionBriefTableAdapter.h"

#include <QApplication>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString& message) {
    std::cerr << message.toStdString() << '\n';
    std::exit(1);
}

void expectEqual(int actual, int expected, const QString& message) {
    if (actual != expected) {
        fail(QString("%1: expected %2, got %3").arg(message).arg(expected).arg(actual));
    }
}

void expectEqual(const QString& actual, const QString& expected, const QString& message) {
    if (actual != expected) {
        fail(QString("%1: expected %2, got %3").arg(message, expected, actual));
    }
}

void setCell(QTableWidget* table, int row, int column, const QString& text, const QString& tooltip = QString()) {
    auto* item = new QTableWidgetItem(text);
    item->setToolTip(tooltip);
    table->setItem(row, column, item);
}

void initSessionBriefTable(QTableWidget* table) {
    table->setColumnCount(4);
    table->setRowCount(2);

    setCell(table, 0, kSessionBriefAreaColumn, "Target", "Target detail");
    setCell(table, 0, kSessionBriefStatusColumn, "Ready");
    setCell(table, 0, kSessionBriefEvidenceColumn, "runtime connected");
    setCell(table, 0, kSessionBriefNextColumn, "Continue");
    setSessionBriefActionKey(table, 0, "target");

    setCell(table, 1, kSessionBriefAreaColumn, "Gate");
    setCell(table, 1, kSessionBriefStatusColumn, "Warning", "Gate detail");
    setCell(table, 1, kSessionBriefEvidenceColumn, "1 warning");
    setCell(table, 1, kSessionBriefNextColumn, "Open gate");
    setSessionBriefActionKey(table, 1, "gate");
}

// Test extracting structured row data from populated table
void testStructuredRowExtraction() {
    QTableWidget table;
    initSessionBriefTable(&table);

    const SessionBriefTableRow row = sessionBriefTableRowFromTable(&table, 1);
    expectEqual(row.row, 1, "brief row index");
    expectEqual(row.area, "Gate", "brief area");
    expectEqual(row.status, "Warning", "brief status");
    expectEqual(row.evidence, "1 warning", "brief evidence");
    expectEqual(row.next, "Open gate", "brief next");
    expectEqual(row.actionKey, "gate", "brief action key");
    expectEqual(row.firstTooltip, "Gate detail", "brief tooltip");
}

// Test invalid row returns empty data and legacy UserRole action key fallback
void testInvalidRowsAndLegacyActionKeyFallback() {
    QTableWidget table;
    initSessionBriefTable(&table);

    SessionBriefTableRow row = sessionBriefTableRowFromTable(&table, -1);
    expectEqual(row.row, -1, "invalid row index");
    expectEqual(row.area, QString(), "invalid row area");

    table.item(1, kSessionBriefAreaColumn)->setData(Qt::UserRole, "legacyGate");
    table.item(1, kSessionBriefAreaColumn)->setData(Qt::UserRole + 33, QString());
    expectEqual(sessionBriefActionKeyForRow(&table, 1), "legacyGate", "legacy action key fallback");
}

} // namespace

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    testStructuredRowExtraction();
    testInvalidRowsAndLegacyActionKeyFallback();
    return 0;
}
