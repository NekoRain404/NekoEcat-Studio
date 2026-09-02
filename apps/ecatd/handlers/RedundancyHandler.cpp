// RedundancyHandler — EtherCAT cable redundancy operations.

#include "RedundancyHandler.h"

#include "CommandDispatcher.h"
#include "EcatService.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>

// ─── Construction ──────────────────────────────────────────────────────────

RedundancyHandler::RedundancyHandler(EcatService* backend) : backend_(backend) {}

// ─── Status Query ──────────────────────────────────────────────────────────

QJsonObject RedundancyHandler::handleStatus(const QString& id, const QJsonObject& params) {
    const QString master = params.value("master").toString("0").trimmed();

    // Query master info for link status.
    QString error;
    const QString masterOutput = runCliCommand(master, {"master"}, &error);
    if (!error.isEmpty()) {
        return CommandDispatcher::failure(id, error);
    }

    bool primaryUp = false, secondaryUp = false;
    const bool hasRedundancy = parseLinkStatus(masterOutput, &primaryUp, &secondaryUp);

    // Determine state.
    QString state;
    if (!hasRedundancy) {
        state = "SinglePath";
    } else if (primaryUp && secondaryUp) {
        state = "DualPath";
    } else if (primaryUp) {
        state = "PrimaryOnly";
    } else if (secondaryUp) {
        state = "SecondaryOnly";
    } else {
        state = "BothDown";
    }

    QJsonObject primaryPath;
    primaryPath["pathId"] = 0;
    primaryPath["state"] = primaryUp ? "Active" : "Down";
    primaryPath["isHealthy"] = primaryUp;

    QJsonObject secondaryPath;
    secondaryPath["pathId"] = 1;
    secondaryPath["state"] = secondaryUp ? "Active" : "Down";
    secondaryPath["isHealthy"] = secondaryUp;

    QJsonObject result;
    result["enabled"] = hasRedundancy;
    result["state"] = state;
    result["primaryPath"] = primaryPath;
    result["secondaryPath"] = secondaryPath;
    result["master"] = master;
    return CommandDispatcher::success(id, result);
}

// ─── Enable Redundancy ─────────────────────────────────────────────────────

QJsonObject RedundancyHandler::handleEnable(const QString& id, const QJsonObject& params) {
    const QString master = params.value("master").toString("0").trimmed();

    // IgH master redundancy is configured at module load time via ec_master redundancy.
    // Runtime enable is not typically supported; report the current state.
    QString error;
    const QString masterOutput = runCliCommand(master, {"master"}, &error);
    if (!error.isEmpty()) {
        addEvent("enable", -1, error, false);
        return CommandDispatcher::failure(id, error);
    }

    bool primaryUp = false, secondaryUp = false;
    const bool hasRedundancy = parseLinkStatus(masterOutput, &primaryUp, &secondaryUp);

    if (hasRedundancy) {
        addEvent("enable", 0, "Redundancy already active", true);
        return CommandDispatcher::success(id, {{"success", true}, {"message", "Redundancy is already active"}});
    }

    // Redundancy requires IgH master configuration (ec_master redundancy parameter).
    // This cannot be enabled at runtime.
    const QString msg = "Redundancy must be configured at IgH master load time "
                        "(ec_master redundancy=1). Restart the master with redundancy enabled.";
    addEvent("enable", -1, msg, false);
    return CommandDispatcher::failure(id, msg);
}

// ─── Disable Redundancy ────────────────────────────────────────────────────

QJsonObject RedundancyHandler::handleDisable(const QString& id, const QJsonObject& params) {
    Q_UNUSED(params);
    const QString msg = "Redundancy cannot be disabled at runtime. "
                        "Restart the master without the redundancy parameter.";
    addEvent("disable", -1, msg, false);
    return CommandDispatcher::failure(id, msg);
}

// ─── Failover ──────────────────────────────────────────────────────────────

QJsonObject RedundancyHandler::handleFailover(const QString& id, const QJsonObject& params) {
    const QString master = params.value("master").toString("0").trimmed();

    // Check current state.
    QString error;
    const QString masterOutput = runCliCommand(master, {"master"}, &error);
    if (!error.isEmpty()) {
        addEvent("failover", -1, error, false);
        return CommandDispatcher::failure(id, error);
    }

    bool primaryUp = false, secondaryUp = false;
    const bool hasRedundancy = parseLinkStatus(masterOutput, &primaryUp, &secondaryUp);

    if (!hasRedundancy) {
        const QString msg = "Redundancy is not active. Cannot perform failover.";
        addEvent("failover", -1, msg, false);
        return CommandDispatcher::failure(id, msg);
    }

    if (!secondaryUp) {
        const QString msg = "Secondary path is down. Cannot failover.";
        addEvent("failover", 1, msg, false);
        return CommandDispatcher::failure(id, msg);
    }

    // IgH handles failover automatically when the primary link goes down.
    // Manual failover is not a standard IgH operation.
    // We can only report the current state.
    if (primaryUp && secondaryUp) {
        const QString msg = "Both paths are healthy. IgH handles failover automatically "
                            "when the primary link goes down.";
        addEvent("failover", 0, msg, true);
        return CommandDispatcher::success(id, {{"success", true}, {"fromPath", 0}, {"toPath", 1}, {"message", msg}});
    }

    // Primary is down, already on secondary.
    addEvent("failover", 1, "Already failed over to secondary", true);
    return CommandDispatcher::success(
        id, {{"success", true}, {"fromPath", 0}, {"toPath", 1}, {"message", "Already operating on secondary path"}});
}

// ─── Failback ──────────────────────────────────────────────────────────────

QJsonObject RedundancyHandler::handleFailback(const QString& id, const QJsonObject& params) {
    const QString master = params.value("master").toString("0").trimmed();

    QString error;
    const QString masterOutput = runCliCommand(master, {"master"}, &error);
    if (!error.isEmpty()) {
        addEvent("failback", -1, error, false);
        return CommandDispatcher::failure(id, error);
    }

    bool primaryUp = false, secondaryUp = false;
    const bool hasRedundancy = parseLinkStatus(masterOutput, &primaryUp, &secondaryUp);

    if (!hasRedundancy) {
        const QString msg = "Redundancy is not active. Cannot perform failback.";
        addEvent("failback", -1, msg, false);
        return CommandDispatcher::failure(id, msg);
    }

    if (primaryUp) {
        // IgH automatically uses the primary path when it's available.
        addEvent("failback", 0, "Primary path is up, IgH uses it automatically", true);
        return CommandDispatcher::success(id, {{"success", true},
                                               {"fromPath", 1},
                                               {"toPath", 0},
                                               {"message", "Primary path is active. IgH uses it automatically."}});
    }

    const QString msg = "Primary path is still down. Cannot failback.";
    addEvent("failback", 0, msg, false);
    return CommandDispatcher::failure(id, msg);
}

// ─── History ───────────────────────────────────────────────────────────────

QJsonObject RedundancyHandler::handleHistory(const QString& id, const QJsonObject& params) {
    const int limit = params.value("limit").toInt(100);
    const int start = qMax(0, history_.size() - limit);

    QJsonArray events;
    for (int i = start; i < history_.size(); ++i) {
        const auto& evt = history_[i];
        QJsonObject obj;
        obj["type"] = evt.type;
        obj["pathId"] = evt.pathId;
        obj["reason"] = evt.reason;
        obj["timestamp"] = evt.timestamp.toString(Qt::ISODate);
        obj["success"] = evt.success;
        events.append(obj);
    }

    return CommandDispatcher::success(id, {{"events", events}});
}

// ─── Helpers ───────────────────────────────────────────────────────────────

void RedundancyHandler::addEvent(const QString& type, int pathId, const QString& reason, bool success) {
    RedundancyEventRecord evt;
    evt.type = type;
    evt.pathId = pathId;
    evt.reason = reason;
    evt.timestamp = QDateTime::currentDateTime();
    evt.success = success;
    history_.append(evt);
    if (history_.size() > kMaxHistory) {
        history_.removeFirst();
    }
}

QString RedundancyHandler::runCliCommand(const QString& master, const QStringList& args, QString* error) const {
    if (backend_ && args.size() == 1 && args.first() == "master") {
        return backend_->masterText(master, error);
    }

    QProcess proc;
    QStringList fullArgs;
    const QString trimmed = master.trimmed();
    if (!trimmed.isEmpty()) {
        fullArgs << "-m" << trimmed;
    }
    fullArgs << args;
    proc.setProgram("ethercat");
    proc.setArguments(fullArgs);
    proc.start();
    if (!proc.waitForFinished(5000)) {
        proc.kill();
        proc.waitForFinished(1000);
        if (error)
            *error = "ethercat command timed out";
        return {};
    }
    if (error && proc.exitCode() != 0) {
        *error = QString::fromUtf8(proc.readAllStandardError()).trimmed();
    }
    return QString::fromUtf8(proc.readAllStandardOutput());
}

bool RedundancyHandler::parseLinkStatus(const QString& masterOutput, bool* primaryUp, bool* secondaryUp) const {
    // Look for "Link" info in master output.
    // IgH reports: "Link: 0: up  1: up" or similar patterns.
    static QRegularExpression linkRe(R"(Link:\s*(\d+):\s*(up|down)\s+(\d+):\s*(up|down))",
                                     QRegularExpression::CaseInsensitiveOption);
    const auto match = linkRe.match(masterOutput);
    if (match.hasMatch()) {
        if (primaryUp)
            *primaryUp = (match.captured(2).toLower() == "up");
        if (secondaryUp)
            *secondaryUp = (match.captured(4).toLower() == "up");
        return true;
    }

    // Fallback: check for "Main" and "Redundant" link indicators.
    static QRegularExpression mainRe(R"(Main:\s*(\w+))", QRegularExpression::CaseInsensitiveOption);
    static QRegularExpression redRe(R"(Redundant:\s*(\w+))", QRegularExpression::CaseInsensitiveOption);
    const auto mainMatch = mainRe.match(masterOutput);
    const auto redMatch = redRe.match(masterOutput);

    if (mainMatch.hasMatch()) {
        if (primaryUp)
            *primaryUp = (mainMatch.captured(1).toLower() == "up");
    }
    if (redMatch.hasMatch()) {
        if (secondaryUp)
            *secondaryUp = (redMatch.captured(1).toLower() == "up");
        return true; // Has redundant link info.
    }

    // No redundancy info found.
    if (primaryUp)
        *primaryUp = true; // Assume single path.
    if (secondaryUp)
        *secondaryUp = false;
    return false;
}
