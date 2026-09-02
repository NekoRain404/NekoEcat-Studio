#pragma once

#include <QWidget>

class QTableWidget;
class QLabel;
class QPushButton;
struct DcSyncOptimizationResult;

class SyncOptimizerWidget : public QWidget {
    Q_OBJECT
public:
    explicit SyncOptimizerWidget(QWidget* parent = nullptr);

    void displayResult(const DcSyncOptimizationResult& result);
    void clear();

signals:
    void applyRequested();

private:
    void buildUi();

    QTableWidget* beforeTable_ = nullptr;
    QTableWidget* afterTable_ = nullptr;
    QLabel* improvementLabel_ = nullptr;
    QLabel* recommendationsLabel_ = nullptr;
    QPushButton* applyBtn_ = nullptr;
};
