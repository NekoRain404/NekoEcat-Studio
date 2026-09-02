#pragma once

// ProtocolAnalyzerPlugin — workspace plugin for EtherCAT protocol frame analysis.
//
// Features:
//   - Real-time frame capture from EtherCAT bus
//   - Protocol decode for EtherCAT, CoE, EoE, FoE, SoE subprotocols
//   - Multi-dimensional filtering: by type, direction, and slave position
//   - Live statistics (frame counts, error rates, bandwidth)
//   - PCAP export for external analysis (Wireshark compatible)
//   - Frame table with timestamp, type, direction, slave, length, summary
//
// UI Description:
//   The protocol analyzer displays a frame capture table with filter controls.
//   Capture can be started/stopped. Filters narrow displayed frames by type
//   (Ethernet, CoE, EoE, FoE, SoE), direction (TX/RX), and slave position.
//   Statistics are shown in a summary label. Export saves captured frames as PCAP.
//
// Constructor Pattern: Fine-grained injection (ProtocolAnalyzerService)
// Default Order: 105

#include "plugins/WorkspacePlugin.h"
#include "ProtocolAnalyzerService.h"

class QTableWidget;
class QComboBox;
class QLabel;
class QPushButton;
class QSpinBox;

class ProtocolAnalyzerPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit ProtocolAnalyzerPlugin(ProtocolAnalyzerService* service, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    ProtocolAnalyzerService* service() const { return service_; }

private slots:
    void toggleCapture();
    void clearFrames();
    void applyFilter();
    void onFrameCaptured(const ProtocolFrame& frame);
    void updateStatistics(const ProtocolStatistics& stats);
    void exportPcap();

private:
    void buildUi();

    ProtocolAnalyzerService* service_;
    QWidget* container_ = nullptr;
    QTableWidget* frameTable_ = nullptr;
    QLabel* statsLabel_ = nullptr;
    QPushButton* captureBtn_ = nullptr;
    QComboBox* filterTypeCombo_ = nullptr;
    QComboBox* filterDirCombo_ = nullptr;
    QSpinBox* filterSlaveSpin_ = nullptr;
    QPushButton* filterBtn_ = nullptr;
    QPushButton* clearBtn_ = nullptr;
    QPushButton* exportBtn_ = nullptr;
};
