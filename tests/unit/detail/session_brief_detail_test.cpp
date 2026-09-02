// SessionBriefDetailTest — Tests for SessionBriefDetail
//
// Test coverage:
//   - Status text mapping and color key generation
//   - Table header generation
//   - UI row cell and tooltip generation

#include "detail/SessionBriefDetail.h"

#include <QCoreApplication>

#include <cstdlib>
#include <iostream>

namespace {

void fail(const QString& message) {
    std::cerr << message.toStdString() << '\n';
    std::exit(1);
}

void expectEqual(const QString& actual, const QString& expected, const QString& message) {
    if (actual != expected) {
        fail(QString("%1: expected %2, got %3").arg(message, expected, actual));
    }
}

void expectEqual(int actual, int expected, const QString& message) {
    if (actual != expected) {
        fail(QString("%1: expected %2, got %3").arg(message).arg(expected).arg(actual));
    }
}

SessionBriefUiTexts englishTexts() {
    return {
        .ready = "Ready",
        .action = "Action",
        .warning = "Warning",
        .error = "Error",
        .info = "Info",
        .areaHeader = "Area",
        .statusHeader = "Status",
        .evidenceHeader = "Evidence",
        .nextHeader = "Next",
        .openLocalEvidenceTooltipPattern = "%1\nOpen local evidence.",
    };
}

// Test status text and color key for each SessionBriefStatus value
void testStatusTextAndColorKeys() {
    const SessionBriefUiTexts texts = englishTexts();
    expectEqual(sessionBriefStatusText(SessionBriefStatus::Ready, texts), "Ready", "ready status text");
    expectEqual(sessionBriefStatusText(SessionBriefStatus::Action, texts), "Action", "action status text");
    expectEqual(sessionBriefStatusText(SessionBriefStatus::Warning, texts), "Warning", "warning status text");
    expectEqual(sessionBriefStatusText(SessionBriefStatus::Error, texts), "Error", "error status text");
    expectEqual(sessionBriefStatusText(SessionBriefStatus::Info, texts), "Info", "info status text");
    expectEqual(sessionBriefStatusColorKey(SessionBriefStatus::Ready), "ready", "ready color key");
    expectEqual(sessionBriefStatusColorKey(SessionBriefStatus::Warning), "warning", "warning color key");
    expectEqual(sessionBriefStatusColorKey(SessionBriefStatus::Error), "error", "error color key");
}

// Test table headers and UI row cell/tooltip generation
void testHeadersAndUiRow() {
    const SessionBriefUiTexts texts = englishTexts();
    const QStringList headers = sessionBriefTableHeaders(texts);
    expectEqual(headers.size(), 4, "brief header count");
    expectEqual(headers.first(), "Area", "first brief header");
    expectEqual(headers.last(), "Next", "last brief header");

    SessionBriefRow row;
    row.actionKey = "gate";
    row.status = SessionBriefStatus::Warning;
    const SessionBriefUiRow uiRow = sessionBriefUiRow(row, "Gate", "2 warnings", "Open gate", texts);
    expectEqual(uiRow.cells.size(), 4, "brief cell count");
    expectEqual(uiRow.cells.at(0), "Gate", "area cell");
    expectEqual(uiRow.cells.at(1), "Warning", "status cell");
    expectEqual(uiRow.cells.at(2), "2 warnings", "evidence cell");
    expectEqual(uiRow.cells.at(3), "Open gate", "next cell");
    expectEqual(uiRow.actionKey, "gate", "route action key");
    expectEqual(uiRow.tooltips.size(), 4, "tooltip count");
    expectEqual(uiRow.tooltips.at(2), "2 warnings\nOpen local evidence.", "evidence tooltip uses cell text");
}

} // namespace

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    testStatusTextAndColorKeys();
    testHeadersAndUiRow();
    return 0;
}
