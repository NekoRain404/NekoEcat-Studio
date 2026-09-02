#pragma once

#include "services/SdoOptimizationService.h"
#include <QWidget>

class QLabel;
class QPushButton;

class BatchOptimizerWidget : public QWidget {
    Q_OBJECT
public:
    explicit BatchOptimizerWidget(QWidget* parent = nullptr);

    void updateCurrentBatch(int batchSize, double transferTime, double overhead);
    void showOptimizationResult(const SdoOptimizationResult& result);
    void setOptimized();

    QLabel* batchSizeLabel() const { return batchSizeValue_; }
    QLabel* transferTimeLabel() const { return transferTimeValue_; }
    QLabel* overheadLabel() const { return overheadValue_; }
    QPushButton* optimizeButton() const { return optimizeBtn_; }

signals:
    void optimizeRequested();

private:
    void buildUi();

    QLabel* batchSizeValue_ = nullptr;
    QLabel* transferTimeValue_ = nullptr;
    QLabel* overheadValue_ = nullptr;
    QLabel* beforeLabel_ = nullptr;
    QLabel* afterLabel_ = nullptr;
    QLabel* improvementLabel_ = nullptr;
    QPushButton* optimizeBtn_ = nullptr;
};
