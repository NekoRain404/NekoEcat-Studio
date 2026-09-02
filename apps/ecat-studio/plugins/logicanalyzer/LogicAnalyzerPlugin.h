#pragma once

// LogicAnalyzerPlugin — workspace plugin for logic analysis.
// Displays channel list, trigger settings, waveform display, and measurement tools.

#include "plugins/WorkspacePlugin.h"

class QTableWidget;
class QComboBox;
class QSpinBox;
class QLabel;
class QPushButton;
class QCheckBox;
class TraceService;

struct LogicChannel {
    int id = -1;
    QString name;
    QString protocol;
    QVector<bool> samples;
};

class LogicAnalyzerPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit LogicAnalyzerPlugin(TraceService* service, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

private slots:
    void showAddChannelDialog();
    void removeSelectedChannel();
    void startCapture();
    void stopCapture();
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void decodeProtocol();

private:
    void buildUi();
    void updateChannelTable();
    void refreshWaveforms();

    TraceService* service_;
    QWidget* container_ = nullptr;
    QTableWidget* channelTable_ = nullptr;
    QComboBox* triggerModeCombo_ = nullptr;
    QSpinBox* triggerChannelSpin_ = nullptr;
    QPushButton* startBtn_ = nullptr;
    QPushButton* stopBtn_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QLabel* cursorLabel_ = nullptr;
    QWidget* waveformDisplay_ = nullptr;
    QVector<LogicChannel> logicChannels_;
    double zoomLevel_ = 1.0;
};
