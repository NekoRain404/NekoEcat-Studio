#include "EtherCATSimulationService.h"

// EtherCATSimulationService.cpp — Virtual slave simulation for testing without hardware
//
// Implementation notes:
//   - Creates/removes virtual slaves keyed by position (rejects duplicates)
//   - Start/stop controls simulation running state
//   - Emits state change signals when slaves are added/removed during active simulation

EtherCATSimulationService::EtherCATSimulationService(QObject *parent)
    : QObject(parent)
{
}

int EtherCATSimulationService::createVirtualSlave(const SimulationSlaveConfig &config)
{
    for (const auto &s : slaves_) {
        if (s.config.position == config.position)
            return -1;
    }

    VirtualSlave slave;
    slave.config = config;
    slave.online = true;
    slave.lastUpdateTimeMs = QDateTime::currentMSecsSinceEpoch();
    slaves_.append(slave);

    emit virtualSlaveCreated(config.position);
    if (running_) {
        SimulationState state = simulationState();
        emit simulationStateChanged(state);
    }
    return config.position;
}

bool EtherCATSimulationService::removeVirtualSlave(int position)
{
    for (int i = 0; i < slaves_.size(); ++i) {
        if (slaves_[i].config.position == position) {
            slaves_.removeAt(i);
            emit virtualSlaveRemoved(position);
            if (running_) {
                SimulationState state = simulationState();
                emit simulationStateChanged(state);
            }
            return true;
        }
    }
    return false;
}

bool EtherCATSimulationService::startSimulation()
{
    if (running_)
        return false;
    if (slaves_.isEmpty())
        return false;

    running_ = true;
    frameCount_ = 0;
    errorCount_ = 0;

    if (!slaves_.isEmpty())
        cycleTimeUs_ = slaves_[0].config.cycleTimeUs;

    emit simulationStarted();
    emit simulationStateChanged(simulationState());
    return true;
}

bool EtherCATSimulationService::stopSimulation()
{
    if (!running_)
        return false;

    running_ = false;
    emit simulationStopped();
    emit simulationStateChanged(simulationState());
    return true;
}

SimulationState EtherCATSimulationService::simulationState() const
{
    SimulationState state;
    state.running = running_;
    state.slaveCount = slaves_.size();
    state.cycleTimeUs = cycleTimeUs_;
    state.frameCount = frameCount_;
    state.errorCount = errorCount_;
    state.timestampMs = QDateTime::currentMSecsSinceEpoch();
    return state;
}

QVector<VirtualSlave> EtherCATSimulationService::virtualSlaves() const
{
    return slaves_;
}

VirtualSlave EtherCATSimulationService::virtualSlaveAt(int position) const
{
    for (const auto &s : slaves_) {
        if (s.config.position == position)
            return s;
    }
    return VirtualSlave{};
}
