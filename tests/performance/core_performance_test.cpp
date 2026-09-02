#include <QElapsedTimer>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QSignalSpy>
#include <QString>
#include <QTest>
#include <QVector>

#include "EthercatTypes.h"
#include "JsonProtocol.h"
#include "services/EventBus.h"

class CorePerformanceTest : public QObject {
    Q_OBJECT

private:
    static SlaveInfo makeSlave(int pos) {
        SlaveInfo s;
        s.position = pos;
        s.state = QStringLiteral("OP");
        s.flags = QStringLiteral("--");
        s.name = QStringLiteral("EK1100-%1").arg(pos);
        s.rawLine = QStringLiteral("%1  EK1100  OP  --").arg(pos);
        return s;
    }

    static QVector<SlaveInfo> makeSlaves(int count) {
        QVector<SlaveInfo> v;
        v.reserve(count);
        for (int i = 0; i < count; ++i)
            v.append(makeSlave(i));
        return v;
    }

private slots:
    void benchmark_jsonProtocol_encode() {
        QJsonObject obj = JsonProtocol::request(QStringLiteral("req-1"), QStringLiteral("slave.scan"),
                                                QJsonObject{{"timeout", 5000}});

        QBENCHMARK {
            for (int i = 0; i < 10000; ++i)
                JsonProtocol::encode(obj);
        }
    }

    void benchmark_jsonProtocol_decode() {
        QJsonObject obj = JsonProtocol::success(QStringLiteral("req-1"), QJsonObject{{"status", "ok"}});
        QByteArray data = JsonProtocol::encode(obj);

        QBENCHMARK {
            for (int i = 0; i < 10000; ++i) {
                QJsonDocument doc = QJsonDocument::fromJson(data);
                Q_UNUSED(doc.object());
            }
        }
    }

    void benchmark_jsonProtocol_request_create() {
        QJsonObject params{{"timeout", 5000}, {"retries", 3}};

        QBENCHMARK {
            for (int i = 0; i < 10000; ++i)
                JsonProtocol::request(QStringLiteral("id"), QStringLiteral("method"), params);
        }
    }

    void benchmark_slaveInfo_toJson_single() {
        SlaveInfo slave = makeSlave(0);

        QBENCHMARK {
            for (int i = 0; i < 10000; ++i)
                toJson(slave);
        }
    }

    void benchmark_slaveInfo_fromJson_single() {
        QJsonObject obj = toJson(makeSlave(0));

        QBENCHMARK {
            for (int i = 0; i < 10000; ++i)
                slaveFromJson(obj);
        }
    }

    void benchmark_slaveInfo_toJson_vector_1000() {
        QVector<SlaveInfo> slaves = makeSlaves(1000);

        QBENCHMARK {
            toJson(slaves);
        }
    }

    void benchmark_slaveInfo_fromJson_vector_1000() {
        QJsonArray arr = toJson(makeSlaves(1000));

        QBENCHMARK {
            slavesFromJson(arr);
        }
    }

    void benchmark_slaveInfo_vector_reserve_1000() {
        QBENCHMARK {
            QVector<SlaveInfo> v;
            v.reserve(1000);
            for (int i = 0; i < 1000; ++i)
                v.append(makeSlave(i));
        }
    }

    void benchmark_slaveInfo_vector_copy_1000() {
        QVector<SlaveInfo> src = makeSlaves(1000);

        QBENCHMARK {
            QVector<SlaveInfo> copy = src;
            Q_UNUSED(copy);
        }
    }

    void benchmark_eventBus_sdoValue_10000() {
        EventBus bus;
        int count = 0;
        connect(&bus, &EventBus::sdoValueReceived, this, [&count]() { ++count; });

        QBENCHMARK {
            count = 0;
            for (int i = 0; i < 10000; ++i)
                bus.emitSdoValue(i % 256, QStringLiteral("0x1000"), QStringLiteral("0x00"), QStringLiteral("42"));
        }
    }

    void benchmark_eventBus_slaveChanged_1000() {
        EventBus bus;
        QVector<SlaveInfo> slaves = makeSlaves(100);
        int count = 0;
        connect(&bus, &EventBus::slaveChanged, this, [&count]() { ++count; });

        QBENCHMARK {
            count = 0;
            for (int i = 0; i < 1000; ++i)
                bus.emitSlaveChanged(slaves);
        }
    }

    void benchmark_eventBus_connectionState_10000() {
        EventBus bus;
        int count = 0;
        connect(&bus, &EventBus::connectionStateChanged, this, [&count]() { ++count; });

        QBENCHMARK {
            count = 0;
            for (int i = 0; i < 10000; ++i)
                bus.emitConnectionStateChanged(i % 2 == 0);
        }
    }

    void benchmark_eventBus_freeRunTelemetry_10000() {
        EventBus bus;
        QJsonObject tel{{"in0", 100}, {"in1", 200}, {"out0", 50}};
        int count = 0;
        connect(&bus, &EventBus::freeRunTelemetry, this, [&count]() { ++count; });

        QBENCHMARK {
            count = 0;
            for (int i = 0; i < 10000; ++i)
                bus.emitFreeRunTelemetry(tel);
        }
    }

    void benchmark_eventBus_signalData_10000() {
        EventBus bus;
        QVector<double> values(64, 3.14);
        QVector<qint64> timestamps(64, 1234567890LL);
        int count = 0;
        connect(&bus, &EventBus::signalData, this, [&count]() { ++count; });

        QBENCHMARK {
            count = 0;
            for (int i = 0; i < 10000; ++i)
                bus.emitSignalData(0, values, timestamps);
        }
    }
};

QTEST_MAIN(CorePerformanceTest)
#include "core_performance_test.moc"
