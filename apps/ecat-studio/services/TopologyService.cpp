#include "TopologyService.h"
#include "EcatClient.h"

// TopologyService.cpp — Manages EtherCAT slave topology scanning and baseline comparison
//
// Implementation notes:
//   - Delegates scan/rescan to EcatClient, receives results via slavesChanged signal
//   - Baseline capture enables drift detection against a known-good topology
//   - Current and baseline slave lists stored as QVector<SlaveInfo>

TopologyService::TopologyService(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {
  connect(client_, &EcatClient::slavesChanged, this,
          [this](const QVector<SlaveInfo> &slaves) {
            slaves_ = slaves;
            emit scanComplete(slaves);
          });
}

void TopologyService::scan() { client_->scan(); }

void TopologyService::rescan() { client_->rescan(); }

QVector<SlaveInfo> TopologyService::currentSlaves() const { return slaves_; }

QVector<SlaveInfo> TopologyService::baselineSlaves() const { return baseline_; }

void TopologyService::captureBaseline() {
  baseline_ = slaves_;
  baselineCaptured_ = true;
  emit baselineChanged();
}

void TopologyService::clearBaseline() {
  baseline_.clear();
  baselineCaptured_ = false;
  emit baselineChanged();
}

bool TopologyService::baselineCaptured() const { return baselineCaptured_; }
