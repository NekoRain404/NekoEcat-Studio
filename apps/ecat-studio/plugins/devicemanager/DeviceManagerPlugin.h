#pragma once

// DeviceManagerPlugin — workspace plugin for device management.
// Provides device discovery, configuration, and status monitoring UI.
// Polls DeviceManagerService for device state and metrics.

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class DeviceManagerService;

class DeviceManagerPlugin : public WorkspacePlugin {
  Q_OBJECT
public:
  explicit DeviceManagerPlugin(DeviceManagerService *service,
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

  DeviceManagerService *service() const { return service_; }
  QTableWidget *deviceTable() const { return deviceTable_; }

private:
  void buildUi();
  void updateDisplay();

  DeviceManagerService *service_;
  QWidget *containerWidget_ = nullptr;
  QTableWidget *deviceTable_ = nullptr;
  QPushButton *scanBtn_ = nullptr;
  QPushButton *configureBtn_ = nullptr;
  QPushButton *resetBtn_ = nullptr;
  QLabel *statusLabel_ = nullptr;
  QLabel *deviceCountLabel_ = nullptr;
};
