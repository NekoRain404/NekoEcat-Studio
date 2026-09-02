#include "freerun_rpc_handlers.h"

#include "CommandDispatcher.h"
#include "FreeRunController.h"

#include <QJsonObject>

// Minimal duplicate of requestedMaster / requestedMasterIndex from EcatDaemon
// to avoid cross include; production will call through same logic.
static QString requestedMaster(const QJsonObject& params) {
    return params.value("master").toString("0").trimmed();
}

static bool requestedMasterIndex(const QJsonObject& params, uint32_t* index, QString* error) {
    bool ok = false;
    const uint value = requestedMaster(params).toUInt(&ok);
    if (!ok) {
        if (error) {
            *error = "Free Run requires a numeric IgH master index.";
        }
        return false;
    }
    *index = value;
    return true;
}

void registerFreeRunHandlers(CommandDispatcher& dispatcher, FreeRunController& freeRun) {
    dispatcher.registerHandler("freeRunStart", [&freeRun](const QString& id, const QJsonObject& params) {
        uint32_t masterIndex = 0;
        QString error;
        if (!requestedMasterIndex(params, &masterIndex, &error))
            return CommandDispatcher::failure(id, error);
        return freeRun.start(masterIndex, &error) ? CommandDispatcher::success(id, freeRun.telemetry())
                                                  : CommandDispatcher::failure(id, error);
    });

    dispatcher.registerHandler("freeRunStop", [&freeRun](const QString& id, const QJsonObject&) {
        freeRun.stop();
        return CommandDispatcher::success(id, {{"status", freeRun.status()}});
    });

    dispatcher.registerHandler("freeRunStatus", [&freeRun](const QString& id, const QJsonObject&) {
        return CommandDispatcher::success(id, freeRun.telemetry());
    });

    dispatcher.registerHandler("freeRunShmInfo", [&freeRun](const QString& id, const QJsonObject&) {
        return CommandDispatcher::success(id, freeRun.shmInfo());
    });
}
