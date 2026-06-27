#include "DataPipelineService.h"

// DataPipelineService.cpp — Configurable multi-stage data processing pipeline
//
// Implementation notes:
//   - Stages are appended dynamically; filter/transform are built-in stage types
//   - Throughput tracked via a 1-second QTimer sampling bytesProcessed_
//   - Pipeline can be started/stopped; stages execute sequentially on process()

DataPipelineService::DataPipelineService(QObject *parent)
    : QObject(parent) {
  throughputTimer_ = new QTimer(this);
  connect(throughputTimer_, &QTimer::timeout,
          this, &DataPipelineService::updateThroughput);
  throughputTimer_->start(1000);
}

int DataPipelineService::addStage(const QString &name,
                                   const QVariantMap &config) {
  PipelineStage stage;
  stage.name = name;
  stage.type = config.value("type", name).toString();
  stage.config = config;
  stages_.append(stage);
  return stages_.size() - 1;
}

void DataPipelineService::removeStage(int index) {
  if (index >= 0 && index < stages_.size())
    stages_.remove(index);
}

int DataPipelineService::stageCount() const { return stages_.size(); }

// Runs input data through all configured stages sequentially, emitting per-stage results
QByteArray DataPipelineService::process(const QByteArray &input) {
  QByteArray data = input;
  if (!running_ || stages_.isEmpty()) return data;

  for (int i = 0; i < stages_.size(); ++i) {
    const auto &stage = stages_[i];
    if (stage.name == "filter") {
      QByteArray filtered;
      char threshold = stage.config.value("threshold", 0).toInt();
      for (char byte : data) {
        if (static_cast<quint8>(byte) >= static_cast<quint8>(threshold))
          filtered.append(byte);
      }
      data = filtered;
    } else if (stage.name == "transform") {
      int offset = stage.config.value("offset", 0).toInt();
      for (int j = 0; j < data.size(); ++j)
        data[j] = static_cast<char>(
            static_cast<quint8>(data[j]) + offset);
    }
    emit stageCompleted(i, data);
  }

  bytesProcessed_ += data.size();
  emit dataProcessed(data.size());
  emit pipelineFinished(data);
  return data;
}

void DataPipelineService::setBufferSize(int bytes) {
  bufferSize_ = qMax(1, bytes);
}

int DataPipelineService::bufferSize() const { return bufferSize_; }

double DataPipelineService::throughput() const { return throughput_; }

// Samples bytesProcessed_ into throughput_ and resets the counter (1s interval)
void DataPipelineService::updateThroughput() {
  throughput_ = static_cast<double>(bytesProcessed_);
  bytesProcessed_ = 0;
}

bool DataPipelineService::isRunning() const { return running_; }

void DataPipelineService::start() {
  running_ = true;
  emit pipelineUpdated();
}

void DataPipelineService::stop() {
  running_ = false;
  emit pipelineUpdated();
}

int DataPipelineService::addDefaultStage() {
  int idx = stages_.size();
  PipelineStage stage;
  stage.name = QString("stage_%1").arg(idx);
  stage.type = "passthrough";
  stage.active = true;
  stages_.append(stage);
  emit pipelineUpdated();
  return idx;
}

void DataPipelineService::resetStatistics() {
  for (auto &s : stages_) {
    s.processed = 0;
    s.errors = 0;
  }
  bytesProcessed_ = 0;
  throughput_ = 0.0;
  emit pipelineUpdated();
}

QVector<PipelineStage> DataPipelineService::allStages() const {
  return stages_;
}

PipelineMetrics DataPipelineService::pipelineMetrics() const {
  PipelineMetrics m;
  m.throughput = throughput_;
  m.latencyMs = stages_.isEmpty() ? 0.0 : 1.0 / qMax(throughput_, 1.0) * 1000.0;
  return m;
}
