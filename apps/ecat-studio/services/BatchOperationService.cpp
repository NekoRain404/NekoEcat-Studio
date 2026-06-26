#include "BatchOperationService.h"
#include "infra/EcatClient.h"
#include "SdoService.h"
#include "TopologyService.h"

// BatchOperationService.cpp — Executes batched EtherCAT operations with progress tracking
//
// Implementation notes:
//   - Supports batch SDO read, SDO write, state changes, and topology scans
//   - Cooperative cancellation via a boolean flag checked each iteration
//   - Emits batchProgress/batchCompleted/batchFailed signals for UI feedback

BatchOperationService::BatchOperationService(EcatClient *client,
                                             SdoService *sdo,
                                             TopologyService *topology,
                                             QObject *parent)
    : QObject(parent), client_(client), sdo_(sdo), topology_(topology) {}

BatchResult BatchOperationService::executeBatch(const BatchOperation &op) {
  cancelled_ = false;
  progress_ = 0;
  emit batchStarted(op);

  switch (op.type) {
  case BatchType::ReadSDO:
    return executeReadSDO(op);
  case BatchType::WriteSDO:
    return executeWriteSDO(op);
  case BatchType::SetState:
    return executeSetState(op);
  case BatchType::ScanTopology:
    return executeScanTopology(op);
  }

  BatchResult r;
  r.success = false;
  r.error = QStringLiteral("Unknown batch type");
  emit batchFailed(r.error);
  return r;
}

void BatchOperationService::cancelBatch() { cancelled_ = true; }

int BatchOperationService::progress() const { return progress_; }

BatchResult BatchOperationService::executeReadSDO(const BatchOperation &op) {
  BatchResult r;
  r.totalItems = op.reads.size();
  if (!client_ || !client_->isConnected() || !sdo_) {
    r.error = QStringLiteral("EtherCAT daemon is not connected");
    emit batchFailed(r.error);
    return r;
  }

  for (int i = 0; i < op.reads.size(); ++i) {
    if (cancelled_) {
      r.error = QStringLiteral("Cancelled");
      emit batchFailed(r.error);
      return r;
    }
    const auto &item = op.reads[i];
    sdo_->upload(item.position, item.index, item.subIndex);
    r.completedItems = i + 1;
    progress_ = (i + 1) * 100 / r.totalItems;
    emit batchProgress(progress_);
  }

  return r;
}

BatchResult BatchOperationService::executeWriteSDO(const BatchOperation &op) {
  BatchResult r;
  r.totalItems = op.writes.size();
  if (!client_ || !client_->isConnected() || !sdo_) {
    r.error = QStringLiteral("EtherCAT daemon is not connected");
    emit batchFailed(r.error);
    return r;
  }

  for (int i = 0; i < op.writes.size(); ++i) {
    if (cancelled_) {
      r.error = QStringLiteral("Cancelled");
      emit batchFailed(r.error);
      return r;
    }
    const auto &item = op.writes[i];
    sdo_->download(item.position, item.index, item.subIndex, item.value,
                   item.type);
    r.completedItems = i + 1;
    progress_ = (i + 1) * 100 / r.totalItems;
    emit batchProgress(progress_);
  }

  return r;
}

BatchResult BatchOperationService::executeSetState(const BatchOperation &op) {
  BatchResult r;
  r.totalItems = op.stateChanges.size();
  if (!client_ || !client_->isConnected()) {
    r.error = QStringLiteral("EtherCAT daemon is not connected");
    emit batchFailed(r.error);
    return r;
  }

  for (int i = 0; i < op.stateChanges.size(); ++i) {
    if (cancelled_) {
      r.error = QStringLiteral("Cancelled");
      emit batchFailed(r.error);
      return r;
    }
    const auto &item = op.stateChanges[i];
    client_->setState(item.position, item.state);
    r.completedItems = i + 1;
    progress_ = (i + 1) * 100 / r.totalItems;
    emit batchProgress(progress_);
  }

  return r;
}

BatchResult
BatchOperationService::executeScanTopology(const BatchOperation &op) {
  BatchResult r;
  r.totalItems = op.scanCount;
  if (!client_ || !client_->isConnected() || !topology_) {
    r.error = QStringLiteral("EtherCAT daemon is not connected");
    emit batchFailed(r.error);
    return r;
  }

  for (int i = 0; i < op.scanCount; ++i) {
    if (cancelled_) {
      r.error = QStringLiteral("Cancelled");
      emit batchFailed(r.error);
      return r;
    }
    topology_->scan();
    r.completedItems = i + 1;
    progress_ = (i + 1) * 100 / r.totalItems;
    emit batchProgress(progress_);
  }

  return r;
}
