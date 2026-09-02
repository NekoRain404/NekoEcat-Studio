#pragma once

// EtherCATSimulationService — virtual EtherCAT network simulation for
// development and testing without physical hardware.
//
// Provides virtual slave/master/network/timing management. Emits signals
// when simulation state changes.
//
// Thread safety: main (GUI) thread only.

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QVector>

struct SimulationSlaveConfig {
    int position = 0;
    QString name;
    QString vendorId;
    QString productCode;
    int inputSize = 0;
    int outputSize = 0;
    double cycleTimeUs = 1000.0;
};

struct VirtualSlave {
    SimulationSlaveConfig config;
    bool online = true;
    int errorCount = 0;
    double lastUpdateTimeMs = 0.0;
};

struct SimulationState {
    bool running = false;
    int slaveCount = 0;
    double cycleTimeUs = 1000.0;
    qint64 frameCount = 0;
    int errorCount = 0;
    qint64 timestampMs = 0;
};

class EtherCATSimulationService : public QObject {
    Q_OBJECT
public:
    explicit EtherCATSimulationService(QObject* parent = nullptr);

    int createVirtualSlave(const SimulationSlaveConfig& config);
    bool removeVirtualSlave(int position);
    bool startSimulation();
    bool stopSimulation();
    SimulationState simulationState() const;

    QVector<VirtualSlave> virtualSlaves() const;
    VirtualSlave virtualSlaveAt(int position) const;

signals:
    void simulationStarted();
    void simulationStopped();
    void virtualSlaveCreated(int position);
    void virtualSlaveRemoved(int position);
    void simulationStateChanged(const SimulationState& state);

private:
    QVector<VirtualSlave> slaves_;
    bool running_ = false;
    qint64 frameCount_ = 0;
    int errorCount_ = 0;
    double cycleTimeUs_ = 1000.0;
};
