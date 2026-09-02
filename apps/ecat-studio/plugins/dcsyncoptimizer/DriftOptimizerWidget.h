#pragma once

#include <QWidget>

class QTableWidget;
class QLabel;
class QPushButton;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
struct DcSyncOptimizationResult;

class DriftOptimizerWidget : public QWidget {
    Q_OBJECT
public:
    explicit DriftOptimizerWidget(QWidget* parent = nullptr);

    void displayResult(const DcSyncOptimizationResult& result);
    void clear();

signals:
    void applyRequested();

private:
    void buildUi();

    QTableWidget* paramsTable_ = nullptr;
    QLabel* improvementLabel_ = nullptr;
    QLabel* recommendationsLabel_ = nullptr;
    QComboBox* algorithmCombo_ = nullptr;
    QDoubleSpinBox* kpSpin_ = nullptr;
    QDoubleSpinBox* kiSpin_ = nullptr;
    QDoubleSpinBox* kdSpin_ = nullptr;
    QSpinBox* historyWindowSpin_ = nullptr;
    QDoubleSpinBox* thresholdSpin_ = nullptr;
    QPushButton* applyBtn_ = nullptr;
};
