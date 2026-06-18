#include "TopologyService.h"
#include "EcatClient.h"

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
