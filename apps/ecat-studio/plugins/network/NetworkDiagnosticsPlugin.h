#pragma once

// NetworkDiagnosticsPlugin — workspace plugin for network diagnostics.
// Displays port status table, error counters, bandwidth utilization,
// and health indicators.  Polls NetworkDiagnosticsService periodically.

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QTimer;
class NetworkDiagnosticsService;

class NetworkDiagnosticsPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit NetworkDiagnosticsPlugin(NetworkDiagnosticsService* service, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    NetworkDiagnosticsService* service() const { return service_; }
    QTableWidget* portTable() const { return portTable_; }
    QTableWidget* errorTable() const { return errorTable_; }
    bool exportReportToFile(const QString& path);

private:
    void buildUi();
    void updateDisplay();
    void exportReport();

    NetworkDiagnosticsService* service_;
    QWidget* containerWidget_ = nullptr;
    QTableWidget* portTable_ = nullptr;
    QTableWidget* errorTable_ = nullptr;
    QPushButton* startStopBtn_ = nullptr;
    QPushButton* resetBtn_ = nullptr;
    QPushButton* exportBtn_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QLabel* healthLabel_ = nullptr;
    QLabel* bandwidthLabel_ = nullptr;
    QLabel* latencyLabel_ = nullptr;
};
