#pragma once

// DataPipelinePlugin — workspace plugin for data pipeline management.
// Provides pipeline configuration, stage management, and monitoring UI.
// Polls DataPipelineService for pipeline state and metrics.

#include "plugins/WorkspacePlugin.h"

class QLabel;
class QPushButton;
class QTableWidget;
class QComboBox;
class DataPipelineService;

class DataPipelinePlugin : public WorkspacePlugin {
    Q_OBJECT
public:
    explicit DataPipelinePlugin(DataPipelineService* service, QObject* parent = nullptr);

    QString id() const override;
    QString displayName() const override;
    QString displayNameZh() const override;
    QIcon icon() const override;
    QWidget* widget() override;
    int defaultOrder() const override;
    bool visible() const override;

    void activate() override;
    void deactivate() override;

    DataPipelineService* service() const { return service_; }
    QTableWidget* stageTable() const { return stageTable_; }

private:
    void buildUi();
    void updateDisplay();

    DataPipelineService* service_;
    QWidget* containerWidget_ = nullptr;
    QTableWidget* stageTable_ = nullptr;
    QPushButton* startStopBtn_ = nullptr;
    QPushButton* addStageBtn_ = nullptr;
    QPushButton* removeStageBtn_ = nullptr;
    QPushButton* resetBtn_ = nullptr;
    QComboBox* pipelineCombo_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QLabel* throughputLabel_ = nullptr;
    QLabel* latencyLabel_ = nullptr;
};
