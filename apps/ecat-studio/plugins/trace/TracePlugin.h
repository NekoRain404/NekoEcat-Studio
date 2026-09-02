#pragma once

// TracePlugin — workspace plugin for multi-channel signal tracing.
// Displays trace channels, controls, waveform display, and trigger settings.

#include "plugins/WorkspacePlugin.h"

class QTableWidget;
class QComboBox;
class QSpinBox;
class QLabel;
class QPushButton;
class QCheckBox;
class TraceService;

class TracePlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit TracePlugin(TraceService* service, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    bool exportTraceDataToFile(const QString& path);

private slots:
    void showAddChannelDialog();
    void removeSelectedChannel();
    void startTrace();
    void stopTrace();
    void singleCapture();
    void exportTraceData();
    void refreshDisplay();
    void onTriggerModeChanged(int index);

private:
    void buildUi();
    void updateChannelTable();

    TraceService* service_;
    QWidget* container_ = nullptr;
    QTableWidget* channelTable_ = nullptr;
    QComboBox* triggerModeCombo_ = nullptr;
    QSpinBox* sampleRateSpin_ = nullptr;
    QSpinBox* bufferSizeSpin_ = nullptr;
    QPushButton* startBtn_ = nullptr;
    QPushButton* stopBtn_ = nullptr;
    QPushButton* singleBtn_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QWidget* traceDisplay_ = nullptr;
};
