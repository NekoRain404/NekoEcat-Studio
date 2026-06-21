#pragma once
#include <QTest>
#include <QSignalSpy>
#include <QTimer>
#include <QEventLoop>
#include "EthercatTypes.h"
#include "services/EventBus.h"

class TestUtilities {
public:
    static SlaveInfo createTestSlave(int position, const QString &name = "TestSlave") {
        SlaveInfo info;
        info.position = position;
        info.name = name;
        info.state = "OP";
        return info;
    }

    static QVector<SlaveInfo> createTestTopology(int count) {
        QVector<SlaveInfo> slaves;
        for (int i = 0; i < count; i++) {
            slaves.append(createTestSlave(i, QString("Slave_%1").arg(i)));
        }
        return slaves;
    }

    static bool waitForSignal(QObject *sender, const char *signal, int timeout = 1000) {
        QSignalSpy spy(sender, signal);
        return spy.wait(timeout);
    }
};
