#include "AlEventHandler.h"

#include <QProcess>
#include <QRegularExpression>

void AlEventHandler::poll()
{
    auto newEvents = pollSlaveAlStatus();
    for (const auto &evt : newEvents) {
        events_.append(evt);
        if (events_.size() > kMaxEvents)
            events_.removeFirst();
    }
}

QVector<AlEventEntry> AlEventHandler::pollSlaveAlStatus() const
{
    QVector<AlEventEntry> result;

    // Use `ethercat slaves` to get current slave states.
    // If the CLI is unavailable, runCommand returns empty and we silently produce no events.
    const QString output = runCommand("ethercat slaves");
    if (output.isEmpty())
        return result;

    const QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    // Expected format: "0  0:0   OP  +  EL1008"
    // Fields: position, address, state, flag, name
    static const QRegularExpression re(QStringLiteral("\\s+"));

    int pos = 0;
    for (const QString &line : lines) {
        const QStringList parts = line.split(re, Qt::SkipEmptyParts);
        if (parts.size() < 4) {
            ++pos;
            continue;
        }

        const QString state = parts[2];
        // Only record an event when the slave is NOT in a healthy running state.
        if (state != "OP" && state != "SAFEOP" && state != "PREOP") {
            AlEventEntry entry;
            entry.timestampMs = QDateTime::currentMSecsSinceEpoch();
            entry.slave = pos;
            entry.slaveName = parts.size() > 4 ? parts[4] : QString();
            entry.alStatusCode = state;
            entry.severity = (state == "INIT") ? QStringLiteral("Error") : QStringLiteral("Warning");
            entry.description = QStringLiteral("Slave %1 in %2 state").arg(pos).arg(state);
            result.append(entry);
        }
        ++pos;
    }

    return result;
}

QJsonObject AlEventHandler::handle(const QString &id, const QJsonObject &params)
{
    Q_UNUSED(id);
    QJsonArray arr;
    const int limit = params.value("limit").toInt(100);
    const int start = qMax(0, events_.size() - limit);

    for (int i = start; i < events_.size(); ++i) {
        const auto &e = events_[i];
        QJsonObject obj;
        obj["timestamp"] = e.timestampMs;
        obj["slave"] = e.slave;
        obj["slaveName"] = e.slaveName;
        obj["code"] = e.alStatusCode;
        obj["description"] = e.description;
        obj["severity"] = e.severity;
        arr.append(obj);
    }

    QJsonObject result;
    result["events"] = arr;
    result["total"] = events_.size();
    return result;
}

void AlEventHandler::clear()
{
    events_.clear();
    previousAlStatus_.clear();
}

void AlEventHandler::addEvent(const AlEventEntry &entry)
{
    events_.append(entry);
    if (events_.size() > kMaxEvents)
        events_.removeFirst();
}

QString AlEventHandler::runCommand(const QString &cmd) const
{
    QProcess proc;
    proc.start("sh", {"-c", cmd});
    proc.waitForFinished(5000);
    return QString::fromUtf8(proc.readAllStandardOutput());
}
