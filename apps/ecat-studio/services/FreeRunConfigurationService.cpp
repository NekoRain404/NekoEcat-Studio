#include "FreeRunConfigurationService.h"
#include "infra/EcatClient.h"

FreeRunConfigurationService::FreeRunConfigurationService(EcatClient *client,
                                                         EventBus *bus,
                                                         QObject *parent)
    : QObject(parent), client_(client), bus_(bus) {}

bool FreeRunConfigurationService::configureProcessData(
    const ProcessDataConfig &config) {
  if (config.inputs.isEmpty() && config.outputs.isEmpty())
    return false;
  if (config.cycleTimeUs <= 0)
    return false;

  pdConfig_ = config;
  applied_ = false;
  emit configurationChanged();
  return true;
}

bool FreeRunConfigurationService::configureCycleTime(int cycleTimeUs) {
  if (cycleTimeUs <= 0)
    return false;

  pdConfig_.cycleTimeUs = cycleTimeUs;
  applied_ = false;
  emit configurationChanged();
  return true;
}

bool FreeRunConfigurationService::configureDataMapping(
    const DataMappingConfig &config) {
  if (!config.autoMap && config.inputOffsets.isEmpty() &&
      config.outputOffsets.isEmpty())
    return false;

  mappingConfig_ = config;
  applied_ = false;
  emit configurationChanged();
  return true;
}

bool FreeRunConfigurationService::configureErrorHandling(
    const ErrorHandlingConfig &config) {
  if (config.maxRetries < 0)
    return false;
  if (config.retryDelayMs < 0)
    return false;

  errorConfig_ = config;
  applied_ = false;
  emit configurationChanged();
  return true;
}

bool FreeRunConfigurationService::applyConfiguration() {
  if (pdConfig_.inputs.isEmpty() && pdConfig_.outputs.isEmpty())
    return false;
  if (!client_ || !client_->isConnected())
    return false;
  // Staging validation and daemon connectivity are not enough to prove the
  // process-data layout was applied to the EtherCAT master. Keep failing closed
  // until EcatClient exposes a request/acknowledgement path for this operation.
  return false;
}
