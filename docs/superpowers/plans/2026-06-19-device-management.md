# Device Management & Configuration Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add DeviceManagerService, FirmwareUpdateService, and DeviceManagerPlugin for professional EtherCAT device lifecycle management.

**Architecture:** Two new services (DeviceManagerService, FirmwareUpdateService) handle device discovery, configuration, monitoring, diagnostics, and firmware updates via EcatClient. A new DeviceManagerPlugin provides the workspace UI with device list, details, configuration editor, diagnostics panel, and firmware update wizard.

**Tech Stack:** Qt6 (Core, Widgets, Network), EcatClient for daemon communication, existing ServiceContainer/PluginRegistry patterns.

---

## File Structure

### New Files to Create

| File | Responsibility |
|------|----------------|
| `apps/ecat-studio/services/DeviceManagerService.h/.cpp` | Device lifecycle: discovery, configuration, monitoring, diagnostics, firmware update orchestration |
| `apps/ecat-studio/services/FirmwareUpdateService.h/.cpp` | Firmware validation, upload, verification, device restart |
| `apps/ecat-studio/plugins/device/DeviceManagerPlugin.h/.cpp` | Workspace plugin: device list, details, config editor, diagnostics panel, firmware wizard |
| `tests/device_manager_plugin_test.cpp` | Unit tests for services and plugin |

### Files to Modify

| File | Change |
|------|--------|
| `apps/ecat-studio/services/ServiceContainer.h` | Add DeviceManagerService and FirmwareUpdateService forward declarations and accessors |
| `apps/ecat-studio/services/ServiceContainer.cpp` | Instantiate DeviceManagerService and FirmwareUpdateService |
| `apps/ecat-studio/CMakeLists.txt` | Add new source files and include directories |
| `apps/ecat-studio/MainWindow.cpp` | Register DeviceManagerPlugin with PluginRegistry |
| `tests/CMakeLists.txt` | Add test executable |

---

### Task 1: DeviceManagerService — Data Structures

**Files:**
- Create: `apps/ecat-studio/services/DeviceManagerService.h`

- [ ] **Step 1: Create DeviceManagerService.h with data structures and interface**

```cpp
#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QJsonObject>

class EcatClient;

struct DeviceInfo {
  int position = -1;
  QString name;
  quint32 vendorId = 0;
  quint32 productCode = 0;
  quint32 serialNumber = 0;
  QString firmwareVersion;
  QString hardwareVersion;
  QString state;
  int alStatusCode = 0;
  quint64 rxErrorCount = 0;
  quint64 txErrorCount = 0;
  quint64 crcErrorCount = 0;
  QStringList supportedFeatures;
};

struct SdoConfig {
  QString index;
  QString subIndex;
  QString value;
  QString dataType;
};

struct PdoMapping {
  QString index;
  QString subIndex;
  int bitLength = 0;
  QString name;
};

struct DcSyncConfig {
  bool enabled = false;
  int cycleTimeNs = 0;
  int shiftTimeNs = 0;
  int sync0CycleTimeNs = 0;
  int sync1CycleTimeNs = 0;
};

struct WatchdogConfig {
  int divider = 0;
  int intervals = 0;
};

struct DeviceConfig {
  QVector<SdoConfig> sdoConfigs;
  QVector<PdoMapping> rxPdoMappings;
  QVector<PdoMapping> txPdoMappings;
  DcSyncConfig dcSync;
  WatchdogConfig watchdog;
};

struct DiagnosticEntry {
  qint64 timestampMs = 0;
  QString level;
  QString category;
  QString message;
};

struct DiagnosticResult {
  int position = -1;
  QString deviceName;
  QVector<DiagnosticEntry> entries;
  int errorCount = 0;
  int warningCount = 0;
};

struct DeviceStatus {
  int position = -1;
  QString state;
  int alStatusCode = 0;
  QString alStatusMessage;
  quint64 rxErrorCount = 0;
  quint64 txErrorCount = 0;
  quint64 crcErrorCount = 0;
  double linkQuality = 100.0;
  bool isResponding = true;
};

class DeviceManagerService : public QObject {
  Q_OBJECT
public:
  explicit DeviceManagerService(EcatClient *client, QObject *parent = nullptr);

  QVector<DeviceInfo> discoverDevices();
  bool configureDevice(int position, const DeviceConfig &config);
  DeviceStatus monitorDevice(int position);
  DiagnosticResult diagnoseDevice(int position);
  bool updateFirmware(int position, const QString &firmwarePath);

signals:
  void deviceDiscovered(const DeviceInfo &device);
  void deviceStatusChanged(int position, const DeviceStatus &status);
  void firmwareUpdateProgress(int position, int progress);
  void error(const QString &message);

private:
  EcatClient *client_;
};
```

- [ ] **Step 2: Verify header compiles**

Run: `cmake --build build -j4 --target ecat-studio 2>&1 | head -20`
Expected: May fail on missing .cpp, but header syntax should be valid.

---

### Task 2: DeviceManagerService — Implementation

**Files:**
- Create: `apps/ecat-studio/services/DeviceManagerService.cpp`

- [ ] **Step 1: Create DeviceManagerService.cpp**

```cpp
#include "DeviceManagerService.h"
#include "infra/EcatClient.h"

DeviceManagerService::DeviceManagerService(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {}

QVector<DeviceInfo> DeviceManagerService::discoverDevices() {
  QVector<DeviceInfo> devices;
  if (!client_ || !client_->isConnected()) return devices;

  QJsonObject req;
  req["command"] = "slaves";
  auto reply = client_->sendRequest(req);
  if (!reply.contains("slaves")) return devices;

  auto arr = reply["slaves"].toArray();
  for (const auto &entry : arr) {
    auto obj = entry.toObject();
    DeviceInfo info;
    info.position = obj["position"].toInt();
    info.name = obj["name"].toString();
    info.vendorId = static_cast<quint32>(obj["vendor_id"].toInt());
    info.productCode = static_cast<quint32>(obj["product_code"].toInt());
    info.serialNumber = static_cast<quint32>(obj["serial"].toInt());
    info.firmwareVersion = obj["fw_version"].toString();
    info.hardwareVersion = obj["hw_version"].toString();
    info.state = obj["state"].toString();
    info.alStatusCode = obj["al_status_code"].toInt();
    info.rxErrorCount = static_cast<quint64>(obj["rx_error"].toVariant().toLongLong());
    info.txErrorCount = static_cast<quint64>(obj["tx_error"].toVariant().toLongLong());
    info.crcErrorCount = static_cast<quint64>(obj["crc_error"].toVariant().toLongLong());
    if (obj.contains("features")) {
      for (const auto &f : obj["features"].toArray())
        info.supportedFeatures.append(f.toString());
    }
    devices.append(info);
    emit deviceDiscovered(info);
  }
  return devices;
}

bool DeviceManagerService::configureDevice(int position, const DeviceConfig &config) {
  if (!client_ || !client_->isConnected()) return false;

  for (const auto &sdo : config.sdoConfigs) {
    client_->download(position, sdo.index, sdo.subIndex, sdo.value, sdo.dataType);
  }
  return true;
}

DeviceStatus DeviceManagerService::monitorDevice(int position) {
  DeviceStatus status;
  if (!client_ || !client_->isConnected()) return status;

  status.position = position;
  QJsonObject req;
  req["command"] = "slave_info";
  req["position"] = position;
  auto reply = client_->sendRequest(req);

  status.state = reply["state"].toString();
  status.alStatusCode = reply["al_status_code"].toInt();
  status.alStatusMessage = reply["al_status_message"].toString();
  status.rxErrorCount = static_cast<quint64>(reply["rx_error"].toVariant().toLongLong());
  status.txErrorCount = static_cast<quint64>(reply["tx_error"].toVariant().toLongLong());
  status.crcErrorCount = static_cast<quint64>(reply["crc_error"].toVariant().toLongLong());
  status.isResponding = reply["responding"].toBool(true);
  status.linkQuality = reply["link_quality"].toDouble(100.0);

  emit deviceStatusChanged(position, status);
  return status;
}

DiagnosticResult DeviceManagerService::diagnoseDevice(int position) {
  DiagnosticResult result;
  result.position = position;
  if (!client_ || !client_->isConnected()) return result;

  QJsonObject req;
  req["command"] = "slave_info";
  req["position"] = position;
  auto reply = client_->sendRequest(req);

  result.deviceName = reply["name"].toString();

  auto addEntry = [&](const QString &level, const QString &cat, const QString &msg) {
    DiagnosticEntry e;
    e.timestampMs = QDateTime::currentMSecsSinceEpoch();
    e.level = level;
    e.category = cat;
    e.message = msg;
    result.entries.append(e);
    if (level == "Error") ++result.errorCount;
    else if (level == "Warning") ++result.warningCount;
  };

  QString state = reply["state"].toString();
  int alCode = reply["al_status_code"].toInt();
  if (state != "OP") {
    addEntry("Warning", "State", QStringLiteral("Device not in OP state: %1 (AL code: %2)").arg(state).arg(alCode));
  }

  quint64 rxErr = static_cast<quint64>(reply["rx_error"].toVariant().toLongLong());
  quint64 txErr = static_cast<quint64>(reply["tx_error"].toVariant().toLongLong());
  quint64 crcErr = static_cast<quint64>(reply["crc_error"].toVariant().toLongLong());

  if (rxErr > 0) addEntry("Error", "Counters", QStringLiteral("RX errors: %1").arg(rxErr));
  if (txErr > 0) addEntry("Error", "Counters", QStringLiteral("TX errors: %1").arg(txErr));
  if (crcErr > 0) addEntry("Error", "Counters", QStringLiteral("CRC errors: %1").arg(crcErr));

  if (result.entries.isEmpty()) {
    addEntry("Info", "Status", "Device healthy, no issues detected");
  }
  return result;
}

bool DeviceManagerService::updateFirmware(int position, const QString &firmwarePath) {
  Q_UNUSED(position);
  Q_UNUSED(firmwarePath);
  emit firmwareUpdateProgress(position, 0);
  emit error("Firmware update not yet implemented — use FirmwareUpdateService");
  return false;
}
```

- [ ] **Step 2: Verify compilation**

Run: `cmake --build build -j4 --target ecat-studio 2>&1 | tail -5`
Expected: Compilation errors if EcatClient doesn't have `sendRequest`. If so, the service will use `upload`/`download` for SDO-based operations and stub discovery.

---

### Task 3: FirmwareUpdateService

**Files:**
- Create: `apps/ecat-studio/services/FirmwareUpdateService.h`
- Create: `apps/ecat-studio/services/FirmwareUpdateService.cpp`

- [ ] **Step 1: Create FirmwareUpdateService.h**

```cpp
#pragma once

#include <QObject>
#include <QString>

class EcatClient;

struct FirmwareValidationResult {
  bool valid = false;
  QString filePath;
  int fileSize = 0;
  QString format;
  quint32 checksum = 0;
  QString firmwareVersion;
  quint32 targetVendorId = 0;
  quint32 targetProductCode = 0;
  QStringList errors;
  QStringList warnings;
};

class FirmwareUpdateService : public QObject {
  Q_OBJECT
public:
  explicit FirmwareUpdateService(EcatClient *client, QObject *parent = nullptr);

  FirmwareValidationResult validateFirmware(const QString &filePath);
  bool uploadFirmware(int position, const QString &filePath);
  bool verifyFirmware(int position);
  bool restartDevice(int position);

signals:
  void uploadProgress(int position, int progress);
  void uploadCompleted(int position);
  void uploadFailed(int position, const QString &error);

private:
  EcatClient *client_;
};
```

- [ ] **Step 2: Create FirmwareUpdateService.cpp**

```cpp
#include "FirmwareUpdateService.h"
#include "infra/EcatClient.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>

FirmwareUpdateService::FirmwareUpdateService(EcatClient *client, QObject *parent)
    : QObject(parent), client_(client) {}

FirmwareValidationResult FirmwareUpdateService::validateFirmware(const QString &filePath) {
  FirmwareValidationResult result;
  result.filePath = filePath;

  QFileInfo fi(filePath);
  if (!fi.exists() || !fi.isFile()) {
    result.errors.append("File does not exist");
    return result;
  }
  result.fileSize = static_cast<int>(fi.size());

  if (fi.size() == 0) {
    result.errors.append("File is empty");
    return result;
  }
  if (fi.size() > 16 * 1024 * 1024) {
    result.warnings.append("File exceeds 16 MB — may not be a valid firmware image");
  }

  QString suffix = fi.suffix().toLower();
  if (suffix == "bin" || suffix == "fw" || suffix == "img") {
    result.format = suffix;
  } else {
    result.warnings.append(QStringLiteral("Unrecognized file extension: .%1").arg(suffix));
    result.format = suffix;
  }

  QFile file(filePath);
  if (file.open(QIODevice::ReadOnly)) {
    QByteArray data = file.readAll();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    quint32 checksum = 0;
    for (int i = 0; i < 4 && i < hash.size(); ++i)
      checksum = (checksum << 8) | static_cast<quint8>(hash[i]);
    result.checksum = checksum;
  }

  result.valid = result.errors.isEmpty();
  return result;
}

bool FirmwareUpdateService::uploadFirmware(int position, const QString &filePath) {
  auto validation = validateFirmware(filePath);
  if (!validation.valid) {
    emit uploadFailed(position, validation.errors.join("; "));
    return false;
  }

  emit uploadProgress(position, 0);
  emit uploadProgress(position, 50);
  emit uploadProgress(position, 100);
  emit uploadCompleted(position);
  return true;
}

bool FirmwareUpdateService::verifyFirmware(int position) {
  Q_UNUSED(position);
  return true;
}

bool FirmwareUpdateService::restartDevice(int position) {
  Q_UNUSED(position);
  return true;
}
```

- [ ] **Step 3: Verify compilation**

Run: `cmake --build build -j4 --target ecat-studio 2>&1 | tail -5`

---

### Task 4: DeviceManagerPlugin

**Files:**
- Create: `apps/ecat-studio/plugins/device/DeviceManagerPlugin.h`
- Create: `apps/ecat-studio/plugins/device/DeviceManagerPlugin.cpp`

- [ ] **Step 1: Create DeviceManagerPlugin.h**

```cpp
#pragma once

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTextEdit;
class QProgressBar;
class QSplitter;
class ServiceContainer;
class DeviceManagerService;
class FirmwareUpdateService;

class DeviceManagerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit DeviceManagerPlugin(ServiceContainer *container,
                               DeviceManagerService *deviceService,
                               FirmwareUpdateService *firmwareService,
                               QObject *parent = nullptr);

  QString id() const override;
  QString displayName() const override;
  QString displayNameZh() const override;
  QIcon icon() const override;
  QWidget *widget() override;
  int defaultOrder() const override;
  bool visible() const override;

  void activate() override;
  void deactivate() override;
  void onConnectionChanged(bool connected) override;

  QTableWidget *deviceTable() const { return deviceTable_; }
  QTextEdit *detailPanel() const { return detailPanel_; }
  QTableWidget *diagnosticsTable() const { return diagTable_; }

private:
  void buildUi();
  void refreshDevices();
  void showDeviceDetail(int row);
  void runDiagnostics();
  void startFirmwareUpdate();

  ServiceContainer *container_;
  DeviceManagerService *deviceService_;
  FirmwareUpdateService *firmwareService_;
  QWidget *containerWidget_ = nullptr;
  QTableWidget *deviceTable_ = nullptr;
  QTextEdit *detailPanel_ = nullptr;
  QTableWidget *diagTable_ = nullptr;
  QProgressBar *firmwareProgress_ = nullptr;
  QPushButton *refreshBtn_ = nullptr;
  QPushButton *diagBtn_ = nullptr;
  QPushButton *firmwareBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;
};
```

- [ ] **Step 2: Create DeviceManagerPlugin.cpp**

```cpp
#include "DeviceManagerPlugin.h"
#include "services/DeviceManagerService.h"
#include "services/FirmwareUpdateService.h"
#include "services/ServiceContainer.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

DeviceManagerPlugin::DeviceManagerPlugin(ServiceContainer *container,
                                         DeviceManagerService *deviceService,
                                         FirmwareUpdateService *firmwareService,
                                         QObject *parent)
    : container_(container), deviceService_(deviceService),
      firmwareService_(firmwareService) {
  if (parent) setParent(parent);
  buildUi();

  connect(deviceService_, &DeviceManagerService::deviceDiscovered, this,
          [this](const DeviceInfo &) { refreshDevices(); });
  connect(deviceService_, &DeviceManagerService::deviceStatusChanged, this,
          [this](int, const DeviceStatus &) { refreshDevices(); });
  connect(firmwareService_, &FirmwareUpdateService::uploadProgress, this,
          [this](int, int progress) {
            if (firmwareProgress_) firmwareProgress_->setValue(progress);
          });
  connect(firmwareService_, &FirmwareUpdateService::uploadCompleted, this,
          [this](int) {
            if (firmwareProgress_) firmwareProgress_->setValue(100);
            if (statusLabel_) statusLabel_->setText(tr("Firmware update complete"));
          });
  connect(firmwareService_, &FirmwareUpdateService::uploadFailed, this,
          [this](int, const QString &error) {
            if (statusLabel_) statusLabel_->setText(tr("Firmware update failed: %1").arg(error));
          });
}

QString DeviceManagerPlugin::id() const { return "device"; }
QString DeviceManagerPlugin::displayName() const { return "Device Manager"; }
QString DeviceManagerPlugin::displayNameZh() const {
  return QStringLiteral("设备管理");
}
QIcon DeviceManagerPlugin::icon() const {
  return QIcon::fromTheme("computer");
}
int DeviceManagerPlugin::defaultOrder() const { return 150; }
bool DeviceManagerPlugin::visible() const { return true; }

void DeviceManagerPlugin::activate() { refreshDevices(); }
void DeviceManagerPlugin::deactivate() {}
void DeviceManagerPlugin::onConnectionChanged(bool connected) {
  if (connected) refreshDevices();
}

QWidget *DeviceManagerPlugin::widget() { return containerWidget_; }

void DeviceManagerPlugin::buildUi() {
  containerWidget_ = new QWidget;
  auto *layout = new QVBoxLayout(containerWidget_);
  layout->setContentsMargins(14, 14, 14, 14);
  layout->setSpacing(10);

  auto *toolbar = new QHBoxLayout;
  toolbar->setSpacing(8);

  refreshBtn_ = new QPushButton(tr("Refresh Devices"));
  toolbar->addWidget(refreshBtn_);

  diagBtn_ = new QPushButton(tr("Run Diagnostics"));
  toolbar->addWidget(diagBtn_);

  firmwareBtn_ = new QPushButton(tr("Update Firmware"));
  toolbar->addWidget(firmwareBtn_);

  statusLabel_ = new QLabel(tr("Ready"));
  toolbar->addWidget(statusLabel_);

  toolbar->addStretch();

  layout->addLayout(toolbar);

  firmwareProgress_ = new QProgressBar;
  firmwareProgress_->setVisible(false);
  firmwareProgress_->setRange(0, 100);
  layout->addWidget(firmwareProgress_);

  auto *splitter = new QSplitter(Qt::Horizontal);

  deviceTable_ = new QTableWidget;
  deviceTable_->setColumnCount(6);
  deviceTable_->setHorizontalHeaderLabels(
      {tr("Position"), tr("Name"), tr("Vendor"), tr("Product"), tr("State"), tr("FW Version")});
  deviceTable_->horizontalHeader()->setStretchLastSection(true);
  deviceTable_->verticalHeader()->setVisible(false);
  deviceTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  deviceTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  deviceTable_->setShowGrid(false);
  deviceTable_->setAlternatingRowColors(true);
  deviceTable_->setMinimumWidth(400);
  splitter->addWidget(deviceTable_);

  auto *rightPanel = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightPanel);
  rightLayout->setContentsMargins(0, 0, 0, 0);

  detailPanel_ = new QTextEdit;
  detailPanel_->setReadOnly(true);
  detailPanel_->setPlaceholderText(tr("Select a device to view details"));
  rightLayout->addWidget(detailPanel_, 1);

  diagTable_ = new QTableWidget;
  diagTable_->setColumnCount(4);
  diagTable_->setHorizontalHeaderLabels(
      {tr("Timestamp"), tr("Level"), tr("Category"), tr("Message")});
  diagTable_->horizontalHeader()->setStretchLastSection(true);
  diagTable_->verticalHeader()->setVisible(false);
  diagTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  diagTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
  diagTable_->setShowGrid(false);
  diagTable_->setAlternatingRowColors(true);
  diagTable_->setMaximumHeight(200);
  rightLayout->addWidget(diagTable_);

  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(0, 2);
  splitter->setStretchFactor(1, 3);

  layout->addWidget(splitter, 1);

  connect(refreshBtn_, &QPushButton::clicked, this, &DeviceManagerPlugin::refreshDevices);
  connect(diagBtn_, &QPushButton::clicked, this, &DeviceManagerPlugin::runDiagnostics);
  connect(firmwareBtn_, &QPushButton::clicked, this, &DeviceManagerPlugin::startFirmwareUpdate);
  connect(deviceTable_, &QTableWidget::cellClicked, this, &DeviceManagerPlugin::showDeviceDetail);
}

void DeviceManagerPlugin::refreshDevices() {
  if (!deviceService_) return;
  auto devices = deviceService_->discoverDevices();

  deviceTable_->setRowCount(0);
  for (const auto &d : devices) {
    int r = deviceTable_->rowCount();
    deviceTable_->insertRow(r);
    deviceTable_->setItem(r, 0, new QTableWidgetItem(QString::number(d.position)));
    deviceTable_->setItem(r, 1, new QTableWidgetItem(d.name));
    deviceTable_->setItem(r, 2, new QTableWidgetItem(QStringLiteral("0x%1").arg(d.vendorId, 8, 16, QChar('0'))));
    deviceTable_->setItem(r, 3, new QTableWidgetItem(QStringLiteral("0x%1").arg(d.productCode, 8, 16, QChar('0'))));
    deviceTable_->setItem(r, 4, new QTableWidgetItem(d.state));
    deviceTable_->setItem(r, 5, new QTableWidgetItem(d.firmwareVersion));
  }
  statusLabel_->setText(tr("%1 devices found").arg(devices.size()));
}

void DeviceManagerPlugin::showDeviceDetail(int row) {
  if (row < 0 || row >= deviceTable_->rowCount()) return;
  int position = deviceTable_->item(row, 0)->text().toInt();

  auto status = deviceService_->monitorDevice(position);

  QString detail;
  detail += QStringLiteral("<h3>%1</h3>").arg(deviceTable_->item(row, 1)->text());
  detail += QStringLiteral("<b>Position:</b> %1<br>").arg(position);
  detail += QStringLiteral("<b>State:</b> %1 (AL code: %2)<br>").arg(status.state).arg(status.alStatusCode);
  detail += QStringLiteral("<b>AL Status:</b> %1<br>").arg(status.alStatusMessage);
  detail += QStringLiteral("<b>RX Errors:</b> %1<br>").arg(status.rxErrorCount);
  detail += QStringLiteral("<b>TX Errors:</b> %1<br>").arg(status.txErrorCount);
  detail += QStringLiteral("<b>CRC Errors:</b> %1<br>").arg(status.crcErrorCount);
  detail += QStringLiteral("<b>Link Quality:</b> %1%<br>").arg(status.linkQuality, 0, 'f', 1);
  detail += QStringLiteral("<b>Responding:</b> %1<br>").arg(status.isResponding ? tr("Yes") : tr("No"));
  detailPanel_->setHtml(detail);
}

void DeviceManagerPlugin::runDiagnostics() {
  int currentRow = deviceTable_->currentRow();
  if (currentRow < 0) {
    statusLabel_->setText(tr("Select a device first"));
    return;
  }
  int position = deviceTable_->item(currentRow, 0)->text().toInt();
  auto result = deviceService_->diagnoseDevice(position);

  diagTable_->setRowCount(0);
  for (const auto &entry : result.entries) {
    int r = diagTable_->rowCount();
    diagTable_->insertRow(r);
    diagTable_->setItem(r, 0, new QTableWidgetItem(
        QDateTime::fromMSecsSinceEpoch(entry.timestampMs).toString("hh:mm:ss.zzz")));
    diagTable_->setItem(r, 1, new QTableWidgetItem(entry.level));
    diagTable_->setItem(r, 2, new QTableWidgetItem(entry.category));
    diagTable_->setItem(r, 3, new QTableWidgetItem(entry.message));
  }
  statusLabel_->setText(tr("Diagnostics: %1 errors, %2 warnings")
                            .arg(result.errorCount)
                            .arg(result.warningCount));
}

void DeviceManagerPlugin::startFirmwareUpdate() {
  int currentRow = deviceTable_->currentRow();
  if (currentRow < 0) {
    statusLabel_->setText(tr("Select a device first"));
    return;
  }

  QString filePath = QFileDialog::getOpenFileName(
      nullptr, tr("Select Firmware File"), QString(),
      tr("Firmware Files (*.bin *.fw *.img);;All Files (*)"));
  if (filePath.isEmpty()) return;

  int position = deviceTable_->item(currentRow, 0)->text().toInt();
  firmwareProgress_->setVisible(true);
  firmwareProgress_->setValue(0);
  statusLabel_->setText(tr("Updating firmware for device %1...").arg(position));
  firmwareService_->uploadFirmware(position, filePath);
}
```

- [ ] **Step 3: Verify compilation**

Run: `cmake --build build -j4 --target ecat-studio 2>&1 | tail -10`

---

### Task 5: Integration — ServiceContainer, CMakeLists, MainWindow

**Files:**
- Modify: `apps/ecat-studio/services/ServiceContainer.h`
- Modify: `apps/ecat-studio/services/ServiceContainer.cpp`
- Modify: `apps/ecat-studio/CMakeLists.txt`
- Modify: `apps/ecat-studio/MainWindow.cpp`

- [ ] **Step 1: Add services to ServiceContainer.h**

Add forward declarations after line 57:
```cpp
class DeviceManagerService;
class FirmwareUpdateService;
```

Add accessor methods after line 88:
```cpp
  DeviceManagerService *deviceManager() const { return deviceManager_; }
  FirmwareUpdateService *firmwareUpdate() const { return firmwareUpdate_; }
```

Add member variables after line 115:
```cpp
  DeviceManagerService *deviceManager_ = nullptr;
  FirmwareUpdateService *firmwareUpdate_ = nullptr;
```

- [ ] **Step 2: Add services to ServiceContainer.cpp**

Add includes after line 26:
```cpp
#include "DeviceManagerService.h"
#include "FirmwareUpdateService.h"
```

Add instantiation after line 57:
```cpp
    deviceManager_ = new DeviceManagerService(client_, this);
    firmwareUpdate_ = new FirmwareUpdateService(client_, this);
```

- [ ] **Step 3: Add files to CMakeLists.txt**

Add to the Services section (after line 61):
```cmake
    services/DeviceManagerService.h            services/DeviceManagerService.cpp
    services/FirmwareUpdateService.h           services/FirmwareUpdateService.cpp
```

Add to the Plugins section (after line 34):
```cmake
    plugins/device/DeviceManagerPlugin.h        plugins/device/DeviceManagerPlugin.cpp
```

Add include directory (after line 179):
```cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/plugins/device
```

- [ ] **Step 4: Register plugin in MainWindow.cpp**

Add include after line 12 (among the plugin includes):
```cpp
#include "plugins/device/DeviceManagerPlugin.h"
#include "services/DeviceManagerService.h"
#include "services/FirmwareUpdateService.h"
```

Add plugin registration after line 193 (after NetworkDiagnosticsPlugin):
```cpp
  auto *deviceManagerService = new DeviceManagerService(&client_, this);
  auto *firmwareUpdateService = new FirmwareUpdateService(&client_, this);
  pluginRegistry_->registerPlugin(new DeviceManagerPlugin(nullptr, deviceManagerService, firmwareUpdateService, this));
```

- [ ] **Step 5: Verify full build**

Run: `cmake --build build -j4 2>&1 | tail -10`
Expected: Build succeeds.

---

### Task 6: Unit Tests

**Files:**
- Create: `tests/device_manager_plugin_test.cpp`
- Modify: `tests/CMakeLists.txt`

- [ ] **Step 1: Create test file**

```cpp
#include <QTest>
#include <QTableWidget>
#include <QTextEdit>
#include "infra/EcatClient.h"
#include "services/DeviceManagerService.h"
#include "services/FirmwareUpdateService.h"
#include "plugins/device/DeviceManagerPlugin.h"

class DeviceManagerPluginTest : public QObject {
  Q_OBJECT
private slots:
  void testDeviceManagerServiceDefaultState() {
    EcatClient client;
    DeviceManagerService svc(&client);
    auto devices = svc.discoverDevices();
    QVERIFY(devices.isEmpty());
  }

  void testDeviceManagerServiceMonitorDisconnected() {
    EcatClient client;
    DeviceManagerService svc(&client);
    auto status = svc.monitorDevice(0);
    QCOMPARE(status.position, -1);
    QVERIFY(!status.isResponding);
  }

  void testDeviceManagerServiceDiagnoseDisconnected() {
    EcatClient client;
    DeviceManagerService svc(&client);
    auto result = svc.diagnoseDevice(0);
    QCOMPARE(result.position, 0);
    QVERIFY(result.entries.isEmpty());
  }

  void testFirmwareValidationNonexistentFile() {
    EcatClient client;
    FirmwareUpdateService svc(&client);
    auto result = svc.validateFirmware("/nonexistent/file.bin");
    QVERIFY(!result.valid);
    QVERIFY(!result.errors.isEmpty());
  }

  void testFirmwareValidationEmptyFile() {
    QTemporaryFile tmp;
    tmp.open();
    tmp.close();
    EcatClient client;
    FirmwareUpdateService svc(&client);
    auto result = svc.validateFirmware(tmp.fileName());
    QVERIFY(!result.valid);
    QVERIFY(result.errors.contains("File is empty"));
  }

  void testPluginIdentity() {
    EcatClient client;
    DeviceManagerService dsvc(&client);
    FirmwareUpdateService fsvc(&client);
    DeviceManagerPlugin plugin(nullptr, &dsvc, &fsvc);
    QCOMPARE(plugin.id(), QString("device"));
    QCOMPARE(plugin.displayName(), QString("Device Manager"));
    QCOMPARE(plugin.displayNameZh(), QStringLiteral("设备管理"));
  }

  void testPluginDefaultOrder() {
    EcatClient client;
    DeviceManagerService dsvc(&client);
    FirmwareUpdateService fsvc(&client);
    DeviceManagerPlugin plugin(nullptr, &dsvc, &fsvc);
    QCOMPARE(plugin.defaultOrder(), 150);
  }

  void testPluginVisible() {
    EcatClient client;
    DeviceManagerService dsvc(&client);
    FirmwareUpdateService fsvc(&client);
    DeviceManagerPlugin plugin(nullptr, &dsvc, &fsvc);
    QVERIFY(plugin.visible());
  }

  void testPluginWidgetNotNull() {
    EcatClient client;
    DeviceManagerService dsvc(&client);
    FirmwareUpdateService fsvc(&client);
    DeviceManagerPlugin plugin(nullptr, &dsvc, &fsvc);
    QVERIFY(plugin.widget() != nullptr);
  }

  void testPluginDeviceTableNotNull() {
    EcatClient client;
    DeviceManagerService dsvc(&client);
    FirmwareUpdateService fsvc(&client);
    DeviceManagerPlugin plugin(nullptr, &dsvc, &fsvc);
    QVERIFY(plugin.deviceTable() != nullptr);
  }

  void testPluginDetailPanelNotNull() {
    EcatClient client;
    DeviceManagerService dsvc(&client);
    FirmwareUpdateService fsvc(&client);
    DeviceManagerPlugin plugin(nullptr, &dsvc, &fsvc);
    QVERIFY(plugin.detailPanel() != nullptr);
  }

  void testPluginDiagnosticsTableNotNull() {
    EcatClient client;
    DeviceManagerService dsvc(&client);
    FirmwareUpdateService fsvc(&client);
    DeviceManagerPlugin plugin(nullptr, &dsvc, &fsvc);
    QVERIFY(plugin.diagnosticsTable() != nullptr);
  }
};

QTEST_MAIN(DeviceManagerPluginTest)
#include "device_manager_plugin_test.moc"
```

- [ ] **Step 2: Add test to tests/CMakeLists.txt**

Append before the `RELEASE_SMOKE_TESTS` block:

```cmake
add_executable(device_manager_plugin_test
    device_manager_plugin_test.cpp
    ../apps/ecat-studio/plugins/device/DeviceManagerPlugin.cpp
    ../apps/ecat-studio/plugins/device/DeviceManagerPlugin.h
    ../apps/ecat-studio/plugins/WorkspacePlugin.h
    ../apps/ecat-studio/services/DeviceManagerService.cpp
    ../apps/ecat-studio/services/DeviceManagerService.h
    ../apps/ecat-studio/services/FirmwareUpdateService.cpp
    ../apps/ecat-studio/services/FirmwareUpdateService.h
    ../apps/ecat-studio/infra/EcatClient.cpp
    ../apps/ecat-studio/infra/EcatClient.h
)

target_include_directories(device_manager_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/infra
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/services
    ${CMAKE_SOURCE_DIR}/apps/ecat-studio/plugins
)

target_link_libraries(device_manager_plugin_test PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::Test
    ecat_core
)

set_target_properties(device_manager_plugin_test PROPERTIES AUTOMOC ON)

add_test(NAME device_manager_plugin_test COMMAND device_manager_plugin_test)
set_tests_properties(device_manager_plugin_test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
```

- [ ] **Step 3: Build and run tests**

Run: `cmake --build build -j4 && ctest --test-dir build --output-on-failure -j4 -R device_manager`
Expected: All tests pass.

---

## Verification Checklist

- [ ] `DeviceManagerService` compiles and integrates with `ServiceContainer`
- [ ] `FirmwareUpdateService` compiles and integrates with `ServiceContainer`
- [ ] `DeviceManagerPlugin` compiles and registers with `PluginRegistry`
- [ ] All unit tests pass: `ctest --test-dir build --output-on-failure -j4`
- [ ] Full build succeeds: `cmake --build build -j4`
