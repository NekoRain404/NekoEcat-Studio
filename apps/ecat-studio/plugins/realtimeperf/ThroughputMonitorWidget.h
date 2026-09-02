#pragma once

/// @brief Widget for monitoring EtherCAT bus throughput.
///
/// @details Displays frame rate, byte rate, error rate, and utilization
/// with color-coded gauges and numeric readouts.

#include <QWidget>

struct ThroughputMetrics;

class QLabel;
class QProgressBar;

class ThroughputMonitorWidget : public QWidget {
    Q_OBJECT
public:
    explicit ThroughputMonitorWidget(QWidget* parent = nullptr);

    void updateMetrics(const ThroughputMetrics& m);

private:
    void buildUi();

    QLabel* frameRateLabel_ = nullptr;
    QLabel* byteRateLabel_ = nullptr;
    QLabel* errorRateLabel_ = nullptr;
    QLabel* utilizationLabel_ = nullptr;
    QLabel* totalFramesLabel_ = nullptr;
    QLabel* totalBytesLabel_ = nullptr;
    QLabel* totalErrorsLabel_ = nullptr;
    QProgressBar* utilizationBar_ = nullptr;
};
