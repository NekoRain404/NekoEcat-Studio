#pragma once

#include "services/SdoOptimizationService.h"
#include <QWidget>

class QLabel;
class QPushButton;

class CacheOptimizerWidget : public QWidget {
    Q_OBJECT
public:
    explicit CacheOptimizerWidget(QWidget* parent = nullptr);

    void updateCurrentCache(int size, double hitRate, double missLatency);
    void showOptimizationResult(const SdoOptimizationResult& result);
    void setOptimized();

    QLabel* cacheSizeLabel() const { return cacheSizeValue_; }
    QLabel* hitRateLabel() const { return hitRateValue_; }
    QLabel* missLatencyLabel() const { return missLatencyValue_; }
    QPushButton* optimizeButton() const { return optimizeBtn_; }

signals:
    void optimizeRequested();

private:
    void buildUi();

    QLabel* cacheSizeValue_ = nullptr;
    QLabel* hitRateValue_ = nullptr;
    QLabel* missLatencyValue_ = nullptr;
    QLabel* beforeLabel_ = nullptr;
    QLabel* afterLabel_ = nullptr;
    QLabel* improvementLabel_ = nullptr;
    QPushButton* optimizeBtn_ = nullptr;
};
