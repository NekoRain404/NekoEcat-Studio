#pragma once

#include "services/FreeRunOptimizationService.h"
#include <QWidget>

class QLabel;
class QPushButton;
class QGroupBox;

class CycleTimeOptimizerWidget : public QWidget {
    Q_OBJECT
public:
    explicit CycleTimeOptimizerWidget(QWidget* parent = nullptr);

    void updateCurrentCycleTime(double cycleTimeUs, double jitterUs);
    void showOptimizationResult(const FreeRunOptimizationResult& result);
    void setOptimized();

    QLabel* cycleTimeLabel() const { return cycleTimeValue_; }
    QLabel* jitterLabel() const { return jitterValue_; }
    QPushButton* optimizeButton() const { return optimizeBtn_; }

signals:
    void optimizeRequested();

private:
    void buildUi();

    QLabel* cycleTimeValue_ = nullptr;
    QLabel* jitterValue_ = nullptr;
    QLabel* beforeLabel_ = nullptr;
    QLabel* afterLabel_ = nullptr;
    QLabel* improvementLabel_ = nullptr;
    QPushButton* optimizeBtn_ = nullptr;
};
