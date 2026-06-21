#include "DistributedClockService.h"
#include "infra/EcatClient.h"

// DistributedClockService.cpp — DC drift/jitter monitoring and sync configuration
//
// Implementation notes:
//   - Processes raw DC sync JSON to extract per-slave drift and jitter stats
//   - Tracks reference clock position and worst-case drift slave
//   - Emits driftDetected/jitterUpdated for dashboard consumption

DistributedClockService::DistributedClockService(EcatClient *client,
                                                 QObject *parent)
    : QObject(parent), client_(client) {
  connect(client_, &EcatClient::dcSyncStatusResult, this,
          [this](const QJsonObject &data) { processDcSyncData(data); });
}

bool DistributedClockService::configureSync(int slave, int sync0, int sync1) {
  if (!client_ || !client_->isConnected()) return false;
  emit syncChanged(slave, sync0, sync1);
  return true;
}

void DistributedClockService::requestUpdate() {
  if (!client_ || !client_->isConnected()) return;
  client_->dcSyncStatus();
}

// Parses DC sync JSON: computes max drift, jitter min/max/avg across all slaves
void DistributedClockService::processDcSyncData(const QJsonObject &data) {
  refClock_ = data.value("referenceClockPosition").toInt(-1);

  const auto slaves = data.value("slaves").toArray();
  double maxDrift = 0.0;
  int maxDriftSlave = -1;

  double sumJitter = 0.0;
  double minJitter = 1e9;
  double maxJitter = -1e9;
  int count = 0;

  for (const auto &entry : slaves) {
    const QJsonObject s = entry.toObject();
    const double drift = s.value("driftNs").toDouble();
    const int pos = s.value("position").toInt();

    if (qAbs(drift) > qAbs(maxDrift)) {
      maxDrift = drift;
      maxDriftSlave = pos;
    }

    const double jMin = s.value("jitterMin").toDouble();
    const double jMax = s.value("jitterMax").toDouble();
    const double jAvg = s.value("jitterAvg").toDouble();

    if (jMin < minJitter) minJitter = jMin;
    if (jMax > maxJitter) maxJitter = jMax;
    sumJitter += jAvg;
    ++count;
  }

  if (maxDriftSlave >= 0) {
    driftStatus_.slave = maxDriftSlave;
    driftStatus_.drift = maxDrift;
    driftStatus_.status = qAbs(maxDrift) > driftStatus_.threshold
                              ? QStringLiteral("excessive")
                              : QStringLiteral("ok");
    emit driftDetected(maxDriftSlave, maxDrift);
  }

  if (count > 0) {
    jitterStats_.min = minJitter;
    jitterStats_.max = maxJitter;
    jitterStats_.avg = sumJitter / count;
    jitterStats_.sampleCount = count;
    emit jitterUpdated(jitterStats_);
  }

  emit dcSyncUpdate(data);
}
