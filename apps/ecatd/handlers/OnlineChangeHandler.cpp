// OnlineChangeHandler — runtime configuration changes without full bus restart.

#include "OnlineChangeHandler.h"

#include "CommandDispatcher.h"
#include "EcatService.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QSet>

OnlineChangeHandler::OnlineChangeHandler(EcatService *backend)
    : backend_(backend)
{
}

// ─── Affected Slaves ───────────────────────────────────────────────────────

QJsonArray OnlineChangeHandler::affectedSlaves(const QJsonArray &changes) const
{
    QSet<int> positions;
    for (const auto &c : changes) {
        const auto obj = c.toObject();
        if (obj.contains("position")) {
            positions.insert(obj.value("position").toInt());
        }
    }
    QJsonArray result;
    auto sorted = positions.values();
    std::sort(sorted.begin(), sorted.end());
    for (int p : sorted) result.append(p);
    return result;
}

// ─── Preview ───────────────────────────────────────────────────────────────

QJsonObject OnlineChangeHandler::handlePreview(const QString &id, const QJsonObject &params)
{
    const QJsonArray changes = params.value("changes").toArray();
    if (changes.isEmpty()) {
        return CommandDispatcher::failure(id, "No changes specified.");
    }

    const QJsonArray affected = affectedSlaves(changes);

    QJsonArray operations;
    for (const auto &c : changes) {
        const auto obj = c.toObject();
        QJsonObject op;
        op["position"] = obj.value("position").toInt();
        op["index"] = obj.value("index").toString();
        op["subIndex"] = obj.value("subIndex").toString();
        op["value"] = obj.value("value").toString();
        op["type"] = obj.value("type").toString();
        op["action"] = "sdo_write";
        operations.append(op);
    }

    // Estimate downtime: each affected slave needs OP→PREOP→OP transition.
    // Empirically ~50-200ms per state transition on typical hardware.
    const int estimatedDowntimeMs = affected.size() * 300;

    QJsonObject result;
    result["affectedSlaves"] = affected;
    result["operations"] = operations;
    result["estimatedDowntimeMs"] = estimatedDowntimeMs;
    result["operationCount"] = operations.size();
    return CommandDispatcher::success(id, result);
}

// ─── Apply ─────────────────────────────────────────────────────────────────

QJsonObject OnlineChangeHandler::handleApply(const QString &id, const QJsonObject &params)
{
    if (inProgress_) {
        return CommandDispatcher::failure(id, "An online change is already in progress.");
    }

    const QString master = params.value("master").toString("0").trimmed();
    const QJsonArray changes = params.value("changes").toArray();
    const QString targetState = params.value("targetState").toString("OP");

    if (changes.isEmpty()) {
        return CommandDispatcher::failure(id, "No changes specified.");
    }
    if (!backend_) {
        return CommandDispatcher::failure(id, "No backend available.");
    }

    inProgress_ = true;
    const QJsonArray affected = affectedSlaves(changes);

    int applied = 0, failed = 0;
    QJsonArray results;

    // Phase 1: Transition affected slaves to PREOP (SDO access requires PREOP+).
    currentPhase_ = "transition_preop";
    for (const auto &posVal : affected) {
        const int pos = posVal.toInt();
        QString error;
        if (!backend_->setState(master, pos, "PREOP", &error)) {
            results.append(QJsonObject{
                {"position", pos}, {"phase", "preop"}, {"ok", false}, {"error", error}});
            // Continue with others; this slave's writes will likely fail.
        }
    }

    // Phase 2: Apply SDO writes.
    currentPhase_ = "apply_sdo";
    for (const auto &c : changes) {
        const auto obj = c.toObject();
        const int pos = obj.value("position").toInt();
        QString error;
        const bool ok = backend_->download(
            master, pos,
            obj.value("index").toString(),
            obj.value("subIndex").toString(),
            obj.value("value").toString(),
            obj.value("type").toString(),
            &error);
        if (ok) {
            ++applied;
            results.append(QJsonObject{
                {"position", pos}, {"phase", "sdo"}, {"ok", true},
                {"index", obj.value("index").toString()},
                {"subIndex", obj.value("subIndex").toString()}});
        } else {
            ++failed;
            results.append(QJsonObject{
                {"position", pos}, {"phase", "sdo"}, {"ok", false},
                {"index", obj.value("index").toString()},
                {"subIndex", obj.value("subIndex").toString()},
                {"error", error}});
        }
    }

    // Phase 3: Return affected slaves to target state (default OP).
    currentPhase_ = "transition_op";
    QJsonArray restoreResults;
    for (const auto &posVal : affected) {
        const int pos = posVal.toInt();
        QString error;
        const bool ok = backend_->setState(master, pos, targetState, &error);
        restoreResults.append(QJsonObject{
            {"position", pos}, {"targetState", targetState}, {"ok", ok},
            {"error", ok ? QString() : error}});
    }

    currentPhase_ = "idle";
    inProgress_ = false;

    QJsonObject result;
    result["success"] = (failed == 0);
    result["applied"] = applied;
    result["failed"] = failed;
    result["results"] = results;
    result["restoreResults"] = restoreResults;
    result["affectedSlaves"] = affected;
    return CommandDispatcher::success(id, result);
}

// ─── Status ────────────────────────────────────────────────────────────────

QJsonObject OnlineChangeHandler::handleStatus(const QString &id, const QJsonObject &params)
{
    Q_UNUSED(params);
    QJsonObject result;
    result["inProgress"] = inProgress_;
    result["phase"] = currentPhase_;
    return CommandDispatcher::success(id, result);
}
