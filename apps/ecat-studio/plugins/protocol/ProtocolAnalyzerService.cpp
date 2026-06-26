// ProtocolAnalyzerService — implementation.  See header for interface documentation.
#include "ProtocolAnalyzerService.h"

#include <QJsonArray>
#include <QJsonObject>

ProtocolAnalyzerService::ProtocolAnalyzerService(QObject *parent)
    : QObject(parent) {}

void ProtocolAnalyzerService::startCapture() {
  if (capturing_) return;
  capturing_ = true;
}

void ProtocolAnalyzerService::stopCapture() {
  capturing_ = false;
}

bool ProtocolAnalyzerService::isCapturing() const { return capturing_; }

void ProtocolAnalyzerService::setFilter(const ProtocolFilter &filter) {
  filter_ = filter;
}

ProtocolFilter ProtocolAnalyzerService::filter() const { return filter_; }

QVector<ProtocolFrame> ProtocolAnalyzerService::getFrames(int count) const {
  const int start = qMax(0, frames_.size() - count);
  return frames_.mid(start);
}

int ProtocolAnalyzerService::frameCount() const { return frames_.size(); }

ProtocolStatistics ProtocolAnalyzerService::statistics() const { return stats_; }

QJsonObject ProtocolAnalyzerService::statisticsJson() const {
  QJsonObject obj;
  obj["totalFrames"] = stats_.totalFrames;
  obj["errorFrames"] = stats_.errorFrames;
  obj["txFrames"] = stats_.txFrames;
  obj["rxFrames"] = stats_.rxFrames;
  obj["coeFrames"] = stats_.coeFrames;
  obj["eoeFrames"] = stats_.eoeFrames;
  obj["foeFrames"] = stats_.foeFrames;
  obj["soeFrames"] = stats_.soeFrames;
  obj["bandwidthBps"] = stats_.bandwidthBps;
  return obj;
}

void ProtocolAnalyzerService::clearFrames() {
  frames_.clear();
  stats_ = ProtocolStatistics{};
  emit statisticsUpdated(stats_);
}
