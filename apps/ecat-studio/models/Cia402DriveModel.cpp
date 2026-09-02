// CiA 402 controlword/statusword recommendation and mode-interlock logic.
#include "Cia402DriveModel.h"

#include <QHash>
#include <QStringList>

namespace {

// Handles both decimal and 0x-prefixed hex input with optional sign.
QPair<bool, qint64> parseInteger(QString text) {
    text = text.trimmed();
    if (text.isEmpty()) {
        return {false, 0};
    }
    bool negative = false;
    if (text.startsWith('-')) {
        negative = true;
        text = text.mid(1);
    }
    int base = 10;
    if (text.startsWith("0x", Qt::CaseInsensitive)) {
        base = 16;
        text = text.mid(2);
    }
    bool ok = false;
    const quint64 parsed = text.toULongLong(&ok, base);
    if (!ok) {
        return {false, 0};
    }
    return {true, negative ? -static_cast<qint64>(parsed) : static_cast<qint64>(parsed)};
}

// Formats a 16-bit value as lowercase 0x-prefixed hex for display.
QString hex16(qint64 number) {
    return QString("0x%1").arg(static_cast<quint16>(number), 4, 16, QLatin1Char('0')).toLower();
}

} // namespace

// Maps decoded CiA 402 statusword state to the next recommended controlword command.
Cia402ControlwordRecommendation recommendedCia402ControlwordFromStatus(const QString& decodedStatusword) {
    const QString normalized = decodedStatusword.toLower();
    Cia402ControlwordRecommendation recommendation;
    recommendation.reason = decodedStatusword;

    if (normalized.contains("fault")) {
        recommendation.label = "Fault Reset";
        recommendation.value = "0x0080";
    } else if (normalized.contains("switch on disabled")) {
        recommendation.label = "Shutdown";
        recommendation.value = "0x0006";
    } else if (normalized.contains("ready to switch on")) {
        recommendation.label = "Switch On";
        recommendation.value = "0x0007";
    } else if (normalized.contains("switched on")) {
        recommendation.label = "Enable Operation";
        recommendation.value = "0x000f";
    } else if (normalized.contains("quick stop active")) {
        recommendation.label = "Shutdown";
        recommendation.value = "0x0006";
    }

    return recommendation;
}

// Checks whether an OD index belongs to the CiA 402 drive profile range.
bool isCia402Object(const QString& index, const QString& mode) {
    const QString normalizedIndex = index.trimmed().toLower();
    const QString normalizedMode = mode.trimmed().toLower();
    return normalizedMode.contains("cia 402") || normalizedMode.contains("cia402") || normalizedIndex == "0x6040" ||
           normalizedIndex == "0x6041" || normalizedIndex == "0x6060" || normalizedIndex == "0x6061" ||
           normalizedIndex == "0x603f" || normalizedIndex == "0x6064" || normalizedIndex == "0x606c" ||
           normalizedIndex == "0x6077" || normalizedIndex == "0x607a" || normalizedIndex == "0x60ff" ||
           normalizedIndex == "0x6071";
}

// Produces a human-readable interpretation of raw CiA 402 register values.
QString decodeCia402Value(const QString& index, const QString& value) {
    const QString normalizedIndex = index.trimmed().toLower();
    const auto parsed = parseInteger(value);
    if (!parsed.first) {
        return QString();
    }
    const qint64 numeric = parsed.second;

    if (normalizedIndex == "0x6041") {
        const quint16 sw = static_cast<quint16>(numeric);
        QString state;
        if ((sw & 0x004f) == 0x0000) {
            state = "Not ready to switch on";
        } else if ((sw & 0x004f) == 0x0040) {
            state = "Switch on disabled";
        } else if ((sw & 0x006f) == 0x0021) {
            state = "Ready to switch on";
        } else if ((sw & 0x006f) == 0x0023) {
            state = "Switched on";
        } else if ((sw & 0x006f) == 0x0027) {
            state = "Operation enabled";
        } else if ((sw & 0x006f) == 0x0007) {
            state = "Quick stop active";
        } else if ((sw & 0x004f) == 0x000f) {
            state = "Fault reaction active";
        } else if ((sw & 0x004f) == 0x0008) {
            state = "Fault";
        } else {
            state = "Unknown state";
        }
        QStringList flags;
        if (sw & 0x0010) {
            flags << "voltage";
        }
        if (sw & 0x0080) {
            flags << "warning";
        }
        if (sw & 0x0200) {
            flags << "remote";
        }
        if (sw & 0x0400) {
            flags << "target reached";
        }
        if (sw & 0x0800) {
            flags << "internal limit";
        }
        return flags.isEmpty() ? QString("%1 (%2)").arg(state, hex16(sw))
                               : QString("%1 (%2, %3)").arg(state, hex16(sw), flags.join(", "));
    }

    if (normalizedIndex == "0x6040") {
        const quint16 cw = static_cast<quint16>(numeric);
        QStringList commands;
        if ((cw & 0x0080) != 0) {
            commands << "fault reset";
        }
        if ((cw & 0x000f) == 0x0006) {
            commands << "shutdown";
        } else if ((cw & 0x000f) == 0x0007) {
            commands << "switch on";
        } else if ((cw & 0x000f) == 0x000f) {
            commands << "enable operation";
        } else if ((cw & 0x000f) == 0x0002) {
            commands << "quick stop";
        } else if ((cw & 0x000f) == 0x0000) {
            commands << "disable voltage";
        }
        return commands.isEmpty() ? QString("Controlword %1").arg(hex16(cw))
                                  : QString("%1 (%2)").arg(commands.join(", "), hex16(cw));
    }

    if (normalizedIndex == "0x6060" || normalizedIndex == "0x6061") {
        static const QHash<int, QString> modes = {
            {1, "Profile position"},      {3, "Profile velocity"},
            {4, "Profile torque"},        {6, "Homing"},
            {7, "Interpolated position"}, {8, "Cyclic sync position"},
            {9, "Cyclic sync velocity"},  {10, "Cyclic sync torque"},
        };
        return modes.value(static_cast<int>(numeric), QString("Mode %1").arg(numeric));
    }

    if (normalizedIndex == "0x603f") {
        return numeric == 0 ? "No error" : QString("Error code %1").arg(hex16(numeric));
    }
    if (normalizedIndex == "0x6064") {
        return "Actual position";
    }
    if (normalizedIndex == "0x606c") {
        return "Actual velocity";
    }
    if (normalizedIndex == "0x6077") {
        return "Actual torque";
    }
    if (normalizedIndex == "0x607a") {
        return "Target position";
    }
    if (normalizedIndex == "0x60ff") {
        return "Target velocity";
    }
    if (normalizedIndex == "0x6071") {
        return "Target torque";
    }
    return QString();
}
