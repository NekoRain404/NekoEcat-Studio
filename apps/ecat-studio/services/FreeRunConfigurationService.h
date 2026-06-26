#pragma once

// FreeRunConfigurationService — Free Run process data configuration management.
//
// Manages configuration for Free Run mode including process data layout,
// cycle time, data mapping, and error handling policies. Configuration can be
// staged offline; applying it requires a live daemon connection.
//
// Usage:
//   FreeRunConfigurationService *svc = new FreeRunConfigurationService(client, eventBus, this);
//   ProcessDataConfig pdConfig;
//   pdConfig.inputs = {0x6000, 0x6010};
//   pdConfig.outputs = {0x7000, 0x7010};
//   pdConfig.cycleTime = 1000;
//   svc->configureProcessData(pdConfig);
//   svc->applyConfiguration();

#include <QObject>
#include <QVector>
#include <QString>

class EcatClient;
class EventBus;

struct ProcessDataConfig {
  QVector<int> inputs;
  QVector<int> outputs;
  int cycleTimeUs = 1000;
  QString syncMode = QStringLiteral("DC");
  int watchdogTimeoutMs = 5000;
};

struct DataMappingConfig {
  QVector<int> inputOffsets;
  QVector<int> outputOffsets;
  QVector<int> inputSizes;
  QVector<int> outputSizes;
  bool autoMap = true;
};

struct ErrorHandlingConfig {
  bool retryOnFailure = true;
  int maxRetries = 3;
  int retryDelayMs = 100;
  bool haltOnError = false;
  bool logErrors = true;
};

class FreeRunConfigurationService : public QObject {
  Q_OBJECT
public:
  explicit FreeRunConfigurationService(EcatClient *client, EventBus *bus,
                                       QObject *parent = nullptr);

  bool configureProcessData(const ProcessDataConfig &config);
  bool configureCycleTime(int cycleTimeUs);
  bool configureDataMapping(const DataMappingConfig &config);
  bool configureErrorHandling(const ErrorHandlingConfig &config);

  // Apply the staged Free Run configuration to the live daemon.
  // Returns false while offline instead of marking the configuration applied.
  bool applyConfiguration();

  ProcessDataConfig processDataConfig() const { return pdConfig_; }
  DataMappingConfig dataMappingConfig() const { return mappingConfig_; }
  ErrorHandlingConfig errorHandlingConfig() const { return errorConfig_; }
  bool isApplied() const { return applied_; }

signals:
  void configurationApplied();
  void configurationChanged();

private:
  EcatClient *client_;
  EventBus *bus_;
  ProcessDataConfig pdConfig_;
  DataMappingConfig mappingConfig_;
  ErrorHandlingConfig errorConfig_;
  bool applied_ = false;
};
