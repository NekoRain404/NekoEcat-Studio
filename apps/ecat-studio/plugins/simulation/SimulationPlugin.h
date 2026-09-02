#pragma once

// SimulationPlugin — workspace plugin for EtherCAT bus simulation with
// real-time visualization, statistics, and result export.

#include "plugins/WorkspacePlugin.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSpinBox;
class QTableWidget;
class QTextEdit;
class QTimer;

class SimulationPlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit SimulationPlugin(QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    enum class SimState { Idle, Running, Paused };

    SimState simulationState() const;
    bool isRunning() const;
    bool isPaused() const;

    void setCycleTimeUs(int us);
    int cycleTimeUs() const;

    void setSlaveCount(int count);
    int slaveCount() const;

    void setSimulationDuration(int seconds);
    int simulationDuration() const;

    int frameCount() const;
    double averageLatencyUs() const;
    double maxLatencyUs() const;
    double minLatencyUs() const;
    int errorCount() const;

    void addLogEntry(const QString& entry);
    void clearLog();
    int logCount() const;

    QTableWidget* statisticsTable() const;
    QTableWidget* dataViewTable() const;
    QTextEdit* logView() const;

    bool exportResults(const QString& path);

signals:
    void simulationStateChanged(SimulationPlugin::SimState state);
    void statisticsUpdated();
    void frameProcessed(int frameNumber);

public slots:
    void startSimulation();
    void stopSimulation();
    void pauseSimulation();
    void stepSimulation();
    void resetStatistics();

private:
    void buildUi();
    void updateStatisticsDisplay();
    void onSimulationTimer();

    QWidget* containerWidget_ = nullptr;
    SimState state_ = SimState::Idle;

    QPushButton* startBtn_ = nullptr;
    QPushButton* stopBtn_ = nullptr;
    QPushButton* pauseBtn_ = nullptr;
    QPushButton* stepBtn_ = nullptr;
    QPushButton* resetBtn_ = nullptr;
    QPushButton* exportBtn_ = nullptr;

    QSpinBox* cycleTimeSpin_ = nullptr;
    QSpinBox* slaveCountSpin_ = nullptr;
    QSpinBox* durationSpin_ = nullptr;
    QComboBox* modeCombo_ = nullptr;
    QLabel* stateLabel_ = nullptr;

    QTableWidget* statisticsTable_ = nullptr;
    QTableWidget* dataViewTable_ = nullptr;
    QTextEdit* logView_ = nullptr;
    QTableWidget* settingsTable_ = nullptr;

    QTimer* simTimer_ = nullptr;

    int frameCount_ = 0;
    double totalLatencyUs_ = 0.0;
    double maxLatencyUs_ = 0.0;
    double minLatencyUs_ = 1e9;
    int errorCount_ = 0;
    int elapsedMs_ = 0;
};
