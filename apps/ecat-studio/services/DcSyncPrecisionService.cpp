#include "DcSyncPrecisionService.h"
#include "EventBus.h"
#include "infra/EcatClient.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QtMath>

DcSyncPrecisionService::DcSyncPrecisionService(EcatClient *client,
                                               EventBus *bus,
                                               QObject *parent)
    : QObject(parent), client_(client), bus_(bus) {
  pollTimer_ = new QTimer(this);
  pollTimer_->setInterval(500);
  connect(pollTimer_, &QTimer::timeout, this,
          &DcSyncPrecisionService::pollSyncData);

  if (bus_) {
    connect(bus_, &EventBus::dcSyncUpdate, this,
            &DcSyncPrecisionService::handleDcSyncUpdate);
  }
}

void DcSyncPrecisionService::startMonitoring() {
  if (monitoring_) return;
  if (!client_ || !client_->isConnected()) return;
  monitoring_ = true;
  pollTimer_->start();
  emit monitoringStateChanged(true);
}

void DcSyncPrecisionService::stopMonitoring() {
  if (!monitoring_) return;
  monitoring_ = false;
  pollTimer_->stop();
  emit monitoringStateChanged(false);
}

void DcSyncPrecisionService::setDriftThreshold(double ns) {
  driftThreshold_ = ns;
}

void DcSyncPrecisionService::setHistoryWindow(int samples) {
  historyWindow_ = samples;
}

void DcSyncPrecisionService::pollSyncData() {
  if (!client_ || !client_->isConnected()) return;
  client_->dcSyncStatus();
}

void DcSyncPrecisionService::handleDcSyncUpdate(const QJsonObject &data) {
  processData(data);
}

void DcSyncPrecisionService::processData(const QJsonObject &data) {
  const int newRef = data.value("referenceClockPosition").toInt(-1);
  if (newRef != refClock_) {
    refClock_ = newRef;
    emit referenceClockChanged(refClock_);
  }

  const QJsonArray slaves = data.value("slaves").toArray();
  slaveDrifts_.clear();
  jitterSamples_.clear();

  for (const auto &entry : slaves) {
    const QJsonObject s = entry.toObject();
    DriftStatusEx ds;
    ds.slave = s.value("position").toInt();
    ds.drift = s.value("driftNs").toDouble();
    ds.threshold = driftThreshold_;
    ds.timestamp = QDateTime::currentMSecsSinceEpoch();

    if (qAbs(ds.drift) > driftThreshold_)
      ds.status = QStringLiteral("Error");
    else if (qAbs(ds.drift) > driftThreshold_ * 0.7)
      ds.status = QStringLiteral("Warning");
    else
      ds.status = QStringLiteral("OK");

    auto it = std::find_if(slaveDrifts_.begin(), slaveDrifts_.end(),
                           [&](const DriftStatusEx &ex) {
                             return ex.slave == ds.slave;
                           });
    Q_UNUSED(it);

    slaveDrifts_.append(ds);

    if (s.contains("jitterAvg")) {
      double jmin = s.value("jitterMin").toDouble();
      double jmax = s.value("jitterMax").toDouble();
      jitterSamples_.append((jmin + jmax) / 2.0);
    }
  }

  if (!slaveDrifts_.isEmpty()) {
    driftStatus_ = slaveDrifts_.first();
    for (const auto &ds : slaveDrifts_) {
      if (qAbs(ds.drift) > qAbs(driftStatus_.drift))
        driftStatus_ = ds;
    }
    emit driftUpdated(driftStatus_);
  }

  computeJitterStats();
  computeSyncQuality();
}

void DcSyncPrecisionService::computeJitterStats() {
  if (jitterSamples_.isEmpty()) return;

  JitterStatsEx stats;
  stats.sampleCount = jitterSamples_.size();
  stats.windowSize = historyWindow_;
  stats.min = jitterSamples_.first();
  stats.max = jitterSamples_.first();
  double sum = 0.0;

  for (double v : jitterSamples_) {
    if (v < stats.min) stats.min = v;
    if (v > stats.max) stats.max = v;
    sum += v;
  }
  stats.avg = sum / stats.sampleCount;

  double varSum = 0.0;
  for (double v : jitterSamples_) {
    double diff = v - stats.avg;
    varSum += diff * diff;
  }
  stats.stddev = qSqrt(varSum / stats.sampleCount);

  const int binCount = 20;
  stats.histogramBinWidth = (stats.max - stats.min) / binCount;
  if (stats.histogramBinWidth < 1.0) stats.histogramBinWidth = 1.0;
  stats.histogram.resize(binCount);
  stats.histogram.fill(0);
  for (double v : jitterSamples_) {
    int bin = static_cast<int>((v - stats.min) / stats.histogramBinWidth);
    if (bin < 0) bin = 0;
    if (bin >= binCount) bin = binCount - 1;
    stats.histogram[bin]++;
  }

  jitterStats_ = stats;
  emit jitterUpdated(stats);
}

void DcSyncPrecisionService::computeSyncQuality() {
  SyncQuality q;
  q.totalSlaves = slaveDrifts_.size();

  for (const auto &ds : slaveDrifts_) {
    if (ds.status == "Error")
      q.errorSlaves++;
    else if (ds.status == "Warning")
      q.warningSlaves++;
    else
      q.syncedSlaves++;

    double absDrift = qAbs(ds.drift);
    if (absDrift > q.maxDrift) q.maxDrift = absDrift;
  }

  q.avgJitter = jitterStats_.avg;

  if (q.totalSlaves == 0) {
    q.score = 0.0;
    q.grade = QStringLiteral("N/A");
  } else {
    double syncRatio =
        static_cast<double>(q.syncedSlaves) / q.totalSlaves;
    double driftPenalty = qMin(q.maxDrift / driftThreshold_, 1.0) * 30.0;
    double jitterPenalty =
        qMin(q.avgJitter / (driftThreshold_ * 0.5), 1.0) * 20.0;
    q.score = syncRatio * 100.0 - driftPenalty - jitterPenalty;
    if (q.score < 0) q.score = 0;

    if (q.score >= 90.0)
      q.grade = QStringLiteral("Excellent");
    else if (q.score >= 75.0)
      q.grade = QStringLiteral("Good");
    else if (q.score >= 50.0)
      q.grade = QStringLiteral("Fair");
    else
      q.grade = QStringLiteral("Poor");
  }

  syncQuality_ = q;
  emit syncQualityChanged(q);
}
