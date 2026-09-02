#pragma once

// EoEPlugin — Ethernet over EtherCAT workspace plugin.
//
// Provides a UI for EoE protocol operations:
//   - EoE status query per slave (capability detection)
//   - IP address configuration via standard EoE SDO objects
//   - IP address readback and display
//   - EoE frame statistics monitoring (TX/RX frames, errors)
//
// The plugin connects to EoEService for daemon communication and
// EventBus for slave scan notifications.

#include "EthercatTypes.h"
#include "plugins/WorkspacePlugin.h"

class EoEService;
class EventBus;
class QTableWidget;
class QLineEdit;
class QPushButton;
class QLabel;
class QGroupBox;

class EoEPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit EoEPlugin(EoEService* eoeService, EventBus* eventBus, QObject* parent = nullptr);

    // ── WorkspacePlugin Identity ──────────────────────────────────
    QString id() const override;            // "eoe"
    QString displayName() const override;   // "EoE"
    QString displayNameZh() const override; // "以太网透传"
    QWidget* widget() override;
    int defaultOrder() const override; // 155
    bool visible() const override;

private slots:
    void onSlaveScanComplete(const QVector<SlaveInfo>& slaves);
    void onStatusReceived(int position, const QJsonObject& data);
    void onIpConfigured(int position, const QString& ip);
    void onIpReadback(int position, const QJsonObject& data);
    void onStatsReceived(int position, const QJsonObject& data);
    void onError(const QString& msg);

private:
    void buildUi();
    void refreshSlaveList();
    void querySelectedSlaveStatus();
    void configureSelectedSlaveIp();
    void querySelectedSlaveIp();
    void querySelectedSlaveStats();

    EoEService* eoeService_;
    EventBus* eventBus_;
    QWidget* container_ = nullptr;

    // Slave selection.
    QTableWidget* slaveTable_ = nullptr;

    // Status display.
    QGroupBox* statusGroup_ = nullptr;
    QLabel* eoeSupportLabel_ = nullptr;
    QLabel* ipConfigSupportLabel_ = nullptr;
    QLabel* currentIpLabel_ = nullptr;

    // IP configuration.
    QGroupBox* ipGroup_ = nullptr;
    QLineEdit* ipEdit_ = nullptr;
    QLineEdit* subnetEdit_ = nullptr;
    QLineEdit* gatewayEdit_ = nullptr;
    QLineEdit* dnsEdit_ = nullptr;
    QPushButton* configIpBtn_ = nullptr;
    QPushButton* readIpBtn_ = nullptr;

    // Statistics.
    QGroupBox* statsGroup_ = nullptr;
    QLabel* txFramesLabel_ = nullptr;
    QLabel* rxFramesLabel_ = nullptr;
    QLabel* txErrorsLabel_ = nullptr;
    QLabel* rxErrorsLabel_ = nullptr;
    QPushButton* refreshStatsBtn_ = nullptr;

    // Log.
    QLabel* statusLog_ = nullptr;
};
