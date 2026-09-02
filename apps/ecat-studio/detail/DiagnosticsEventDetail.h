#pragma once

// Diagnostics event row model with level, source, message, and timestamp.


#include <QList>
#include <QString>
#include <QStringList>

// Localized strings for the diagnostics event table headers and summary.
struct DiagnosticsEventTexts {
    QString timeHeader;
    QString levelHeader;
    QString sourceHeader;
    QString messageHeader;
    QString noDiagnostics;
    QString shown;
    QString errorLabel;
    QString warningLabel;
    QString infoLabel;
};

// Visibility and level for a single diagnostics row.
struct DiagnosticsEventRowState {
    QString level;
    bool visible = true;
};

// Aggregated counts and formatted summary text for diagnostics.
struct DiagnosticsEventSummary {
    int visible = 0;
    int total = 0;
    int errors = 0;
    int warnings = 0;
    int infos = 0;
    QString text;
};

QString diagnosticsEventColorKey(const QString& level);
QStringList diagnosticsEventHeaders(const DiagnosticsEventTexts& texts);
DiagnosticsEventSummary diagnosticsEventCounts(const QStringList& levels);
DiagnosticsEventSummary diagnosticsEventSummary(const QList<DiagnosticsEventRowState>& rows,
                                                const DiagnosticsEventTexts& texts);
