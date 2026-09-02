// Host health check result rows for the diagnostics overview.
#include "detail/HostHealthDetail.h"

#include <QJsonObject>

// Maps a health check level to its color key (error, warning, ok).
QString hostHealthColorKey(const QString& level) {
    if (level == QStringLiteral("Error")) {
        return QStringLiteral("error");
    }
    if (level == QStringLiteral("Warning")) {
        return QStringLiteral("warning");
    }
    return QStringLiteral("ok");
}

// Parses host health check JSON into rows, counts, and a localized summary string.
HostHealthDetail buildHostHealthDetail(const QJsonArray& checks, const HostHealthTexts& texts) {
    HostHealthDetail state;
    state.headers = {texts.levelHeader,  texts.checkHeader,   texts.resultHeader,
                     texts.actionHeader, texts.commandHeader, texts.detailHeader};

    // Iterate over collection
    for (const auto& value : checks) {
        const auto check = value.toObject();
        const QString level = check.value("level").toString("Info");
        if (level == QStringLiteral("Error")) {
            ++state.errors;
        } else if (level == QStringLiteral("Warning")) {
            ++state.warnings;
        } else {
            ++state.infos;
        }
        state.rows.append({
            level,
            check.value("source").toString("Host"),
            check.value("message").toString(),
            check.value("hint").toString(),
            check.value("command").toString(),
            check.value("detail").toString(),
        });
        state.colorKeys.append(hostHealthColorKey(level));
    }

    if (checks.isEmpty()) {
        state.summary = texts.unchecked;
    } else if (state.errors > 0) {
        state.summary = QString("%1: %2   %3: %4   %5: %6")
                            .arg(texts.needsAttention)
                            .arg(state.errors)
                            .arg(texts.warningLabel)
                            .arg(state.warnings)
                            .arg(texts.okLabel)
                            .arg(state.infos);
    } else if (state.warnings > 0) {
        state.summary = QString("%1   %2: %3   %4: %5")
                            .arg(texts.usableWithWarnings)
                            .arg(texts.warningLabel)
                            .arg(state.warnings)
                            .arg(texts.okLabel)
                            .arg(state.infos);
    } else {
        state.summary = QString("%1   %2: %3").arg(texts.ready).arg(texts.okLabel).arg(state.infos);
    }

    return state;
}
