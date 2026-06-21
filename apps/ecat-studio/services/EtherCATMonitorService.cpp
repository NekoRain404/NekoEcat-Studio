#include "EtherCATMonitorService.h"
#include "infra/EcatClient.h"
#include "EventBus.h"

// EtherCATMonitorService.cpp — Real-time bus monitoring with timer-driven polling
//
// Implementation notes:
//   - Tracks four metric domains: traffic, error rate, performance, health
//   - Uses QTimer at configurable interval to emit metric update signals
//   - External callers push updates; poll() re-emits current state when connected

EtherCATMonitorService::EtherCATMonitorService(EventBus *bus, EcatClient *client,
                                               QObject *parent)
    : QObject(parent), bus_(bus), client_(client)
{
}

void EtherCATMonitorService::startMonitoring(int intervalMs)
{
    if (running_)
        return;
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &EtherCATMonitorService::poll);
    timer_->start(intervalMs);
    running_ = true;
}

void EtherCATMonitorService::stopMonitoring()
{
    if (!running_)
        return;
    timer_->stop();
    timer_->deleteLater();
    timer_ = nullptr;
    running_ = false;
}

void EtherCATMonitorService::updateTraffic(const BusTraffic &t)
{
    traffic_ = t;
    if (running_)
        emit trafficUpdated(traffic_);
}

void EtherCATMonitorService::updateErrorRate(const ErrorRate &r)
{
    errors_ = r;
    if (running_)
        emit errorRateUpdated(errors_);
}

void EtherCATMonitorService::updatePerformance(const PerformanceMetrics &m)
{
    perf_ = m;
    if (running_)
        emit performanceUpdated(perf_);
}

void EtherCATMonitorService::updateHealth(const HealthStatus &h)
{
    health_ = h;
    if (running_)
        emit healthUpdated(health_);
}

void EtherCATMonitorService::poll()
{
    if (!client_ || !client_->isConnected())
        return;

    emit trafficUpdated(traffic_);
    emit errorRateUpdated(errors_);
    emit performanceUpdated(perf_);
    emit healthUpdated(health_);
}
