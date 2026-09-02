#pragma once

#include "services/PdoMappingOptimizationService.h"
#include <QWidget>

class QLabel;
class QPushButton;
class QGroupBox;

class MappingOptimizerWidget : public QWidget {
    Q_OBJECT
public:
    explicit MappingOptimizerWidget(QWidget* parent = nullptr);

    void updateCurrentMapping(int totalPdos, int totalEntries, int duplicateEntries, int unusedEntries);
    void showOptimizationResult(const PdoMappingOptimizationResult& result);
    void setOptimized();

    QLabel* totalPdosLabel() const { return totalPdosValue_; }
    QLabel* totalEntriesLabel() const { return totalEntriesValue_; }
    QPushButton* optimizeButton() const { return optimizeBtn_; }

signals:
    void optimizeRequested();

private:
    void buildUi();

    QLabel* totalPdosValue_ = nullptr;
    QLabel* totalEntriesValue_ = nullptr;
    QLabel* duplicateEntriesValue_ = nullptr;
    QLabel* unusedEntriesValue_ = nullptr;
    QLabel* beforeLabel_ = nullptr;
    QLabel* afterLabel_ = nullptr;
    QLabel* improvementLabel_ = nullptr;
    QPushButton* optimizeBtn_ = nullptr;
};
