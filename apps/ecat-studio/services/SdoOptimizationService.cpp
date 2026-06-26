#include "SdoOptimizationService.h"
#include "EventBus.h"
#include "infra/EcatClient.h"

#include <QJsonArray>
#include <QJsonObject>

SdoOptimizationService::SdoOptimizationService(EcatClient *client,
                                               EventBus *bus,
                                               QObject *parent)
    : QObject(parent), client_(client), bus_(bus) {}

SdoOptimizationResult SdoOptimizationService::createRejectedResult(
    const QString &category, const QStringList &recommendations) {
  SdoOptimizationResult r;
  r.category = category;
  r.description = tr("%1 optimization requires a connected SDO optimization backend")
                      .arg(category);
  r.recommendations = recommendations;
  r.applied = false;
  r.timestamp = QDateTime::currentDateTime();
  return r;
}

SdoOptimizationResult SdoOptimizationService::optimizeCache() {
  QStringList recs;
  recs << tr("Measure live SDO cache hit rate before changing cache size")
       << tr("Enable predictive prefetching only after backend latency profiling")
       << tr("Choose eviction policy from real object access frequency")
       << tr("Enable write-through caching for frequently accessed objects");

  return createRejectedResult(tr("Cache"), recs);
}

SdoOptimizationResult SdoOptimizationService::optimizeBatch() {
  QStringList recs;
  recs << tr("Measure live SDO transfer overhead before selecting batch size")
       << tr("Use pipelined request/response only when the backend supports it")
       << tr("Group adjacent SDO addresses after confirming mailbox constraints")
       << tr("Enable concurrent upload/download for independent SDO objects");

  return createRejectedResult(tr("Batch"), recs);
}

SdoOptimizationResult SdoOptimizationService::optimizePerformance() {
  QStringList recs;
  recs << tr("Capture live SDO throughput before recommending framing changes")
       << tr("Measure latency distribution before enabling zero-copy transfers")
       << tr("Use priority queuing only after backend queue support is confirmed")
       << tr("Offload SDO segmentation only when hardware support is verified");

  return createRejectedResult(tr("Performance"), recs);
}

SdoOptimizationResult SdoOptimizationService::optimizeErrorHandling() {
  QStringList recs;
  recs << tr("Measure live SDO recovery time before changing retry behavior")
       << tr("Tune retry count with backend error statistics")
       << tr("Use protocol validation supported by the connected backend")
       << tr("Escalate persistent errors instead of reporting automatic re-routing");

  return createRejectedResult(tr("Error Handling"), recs);
}

bool SdoOptimizationService::applyOptimization(const SdoOptimizationResult &result) {
  if (!client_ || !client_->isConnected())
    return false;

  SdoOptimizationResult applied = result;
  applied.applied = true;
  applied.timestamp = QDateTime::currentDateTime();
  history_.append(applied);
  emit optimizationApplied(applied);
  return true;
}

void SdoOptimizationService::clearHistory() {
  history_.clear();
}
