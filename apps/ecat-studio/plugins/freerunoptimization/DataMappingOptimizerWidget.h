#pragma once

#include "services/FreeRunOptimizationService.h"
#include <QWidget>

class QLabel;
class QPushButton;
class QGroupBox;

class DataMappingOptimizerWidget : public QWidget {
    Q_OBJECT
public:
    explicit DataMappingOptimizerWidget(QWidget* parent = nullptr);

    void updateCurrentMapping(int totalBytes, int entries, int unusedBytes);
    void showOptimizationResult(const FreeRunOptimizationResult& result);
    void setOptimized();

    QLabel* totalBytesLabel() const { return totalBytesValue_; }
    QLabel* entriesLabel() const { return entriesValue_; }
    QPushButton* optimizeButton() const { return optimizeBtn_; }

signals:
    void optimizeRequested();

private:
    void buildUi();

    QLabel* totalBytesValue_ = nullptr;
    QLabel* entriesValue_ = nullptr;
    QLabel* unusedBytesValue_ = nullptr;
    QLabel* beforeLabel_ = nullptr;
    QLabel* afterLabel_ = nullptr;
    QLabel* improvementLabel_ = nullptr;
    QPushButton* optimizeBtn_ = nullptr;
};
