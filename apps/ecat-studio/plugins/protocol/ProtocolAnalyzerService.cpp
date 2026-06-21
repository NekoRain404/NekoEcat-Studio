// ProtocolAnalyzerService — implementation.  See header for interface documentation.
#include "ProtocolAnalyzerService.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>

ProtocolAnalyzerService::ProtocolAnalyzerService(QObject *parent)
    : QObject(parent), timer_(new QTimer(this)) {
  connect(timer_, &QTimer::timeout, this, &ProtocolAnalyzerService::generateFrame);
}

void ProtocolAnalyzerService::startCapture() {
  if (capturing_) return;
  capturing_ = true;
  tickCount_ = 0;
  timer_->start(10); // 100 Hz capture rate
}

void ProtocolAnalyzerService::stopCapture() {
  capturing_ = false;
  timer_->stop();
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

void ProtocolAnalyzerService::generateFrame() {
  ++tickCount_;

  ProtocolFrame frame;
  frame.timestamp = QDateTime::currentMSecsSinceEpoch();
  frame.direction = (tickCount_ % 2 == 0) ? ProtocolDirection::TX
                                           : ProtocolDirection::RX;

  // Cycle through frame types.
  const int typeIdx = tickCount_ % 5;
  switch (typeIdx) {
  case 0: frame.frameType = ProtocolFrameType::EtherCAT; break;
  case 1: frame.frameType = ProtocolFrameType::CoE; break;
  case 2: frame.frameType = ProtocolFrameType::EoE; break;
  case 3: frame.frameType = ProtocolFrameType::FoE; break;
  case 4: frame.frameType = ProtocolFrameType::SoE; break;
  }

  frame.wkc = static_cast<quint16>(tickCount_ % 256);
  frame.data = QByteArray(64, static_cast<char>(tickCount_ & 0xFF));

  // Decode summary.
  static const char *kTypeNames[] = {"EtherCAT", "CoE", "EoE", "FoE", "SoE"};
  frame.decodedSummary = QStringLiteral("%1 %2 WKC=%3 Len=%4")
                             .arg(frame.direction == ProtocolDirection::TX ? "TX" : "RX")
                             .arg(kTypeNames[typeIdx])
                             .arg(frame.wkc)
                             .arg(frame.data.size());

  // Apply filter.
  if (filter_.enabled) {
    if (filter_.frameType != frame.frameType) return;
    if (filter_.direction != frame.direction) return;
  }

  // Update stats.
  ++stats_.totalFrames;
  if (frame.direction == ProtocolDirection::TX) ++stats_.txFrames;
  else ++stats_.rxFrames;

  switch (frame.frameType) {
  case ProtocolFrameType::CoE: ++stats_.coeFrames; break;
  case ProtocolFrameType::EoE: ++stats_.eoeFrames; break;
  case ProtocolFrameType::FoE: ++stats_.foeFrames; break;
  case ProtocolFrameType::SoE: ++stats_.soeFrames; break;
  default: break;
  }

  stats_.bandwidthBps = frames_.isEmpty() ? 0.0
      : static_cast<double>(stats_.totalFrames * 64) / (tickCount_ * 0.01);

  // Store frame (rolling window).
  frames_.append(frame);
  if (frames_.size() > kMaxFrames) {
    frames_.remove(0, frames_.size() - kMaxFrames);
  }

  emit frameCaptured(frame);
  if (tickCount_ % 10 == 0) {
    emit statisticsUpdated(stats_);
  }
}
