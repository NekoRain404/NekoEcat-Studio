#pragma once

// RealtimeChartDialog — independent window for live value plotting.
// Scrolls up to 10,000 data points with switchable X-axis units
// (Seconds / Samples). Uses QPainter — no QtCharts dependency.

#include <QDialog>
#include <QTimer>
#include <QVector>

class QLabel;
class QPushButton;
class QComboBox;
class QSpinBox;

// A single data point in the time series.
struct ChartPoint {
    double value = 0.0;
    qint64 timestampMs = 0;  // milliseconds since dialog open
};

// X-axis display modes.
enum class ChartXAxisMode {
    Seconds,   // time in seconds since start
    Samples    // sample index (1, 2, 3, ...)
};

// Custom widget that renders the scrolling line chart.
class ChartWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = nullptr);

    void addPoint(double value);
    void clear();
    void setLabel(const QString &name);
    void setXAxisMode(ChartXAxisMode mode);
    void setVisibleWindow(int points);  // how many points to show

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<ChartPoint> points_;
    QString seriesName_;
    double yMin_ = 0.0;
    double yMax_ = 1.0;
    double yRange_ = 1.0;
    qint64 startTimeMs_ = 0;
    ChartXAxisMode xAxisMode_ = ChartXAxisMode::Seconds;
    int visibleWindow_ = 200;  // points visible at once

    void recalcVisibleRange(double &visYMin, double &visYMax);
};

// Non-modal dialog that combines chart + controls + stats.
class RealtimeChartDialog : public QDialog {
    Q_OBJECT

public:
    explicit RealtimeChartDialog(const QString &entryName,
                                 const QString &entryKey,
                                 double initialValue,
                                 QWidget *parent = nullptr);

    void feedValue(double value);
    void setFreeRunRow(int row) { freeRunRow_ = row; }
    int freeRunRow() const { return freeRunRow_; }
    int totalPoints() const { return chart_ ? totalPoints_ : 0; }

signals:
    void pollingIntervalChanged(int intervalMs);

private slots:
    void toggleRecording();
    void onXAxisModeChanged(int idx);
    void updateAxisUi();
    void updatePollingInterval(int idx);

private:
    ChartWidget *chart_ = nullptr;
    QLabel *valueLabel_ = nullptr;
    QLabel *statsLabel_ = nullptr;
    QPushButton *recordBtn_ = nullptr;
    QComboBox *intervalCombo_ = nullptr;
    QComboBox *xAxisCombo_ = nullptr;
    QSpinBox *windowSpin_ = nullptr;

    QString entryKey_;
    int freeRunRow_ = -1;
    bool recording_ = false;
    int totalPoints_ = 0;
    double lastValue_ = 0.0;
    double sum_ = 0.0;
    int count_ = 0;
    double peak_ = 0.0;
    double trough_ = 0.0;

    int pollingIntervalMs_ = 100;   // estimated polling period (ms)
    int equivalentPoints_ = 200;    // window in points (Samples mode)
    int equivalentSeconds_ = 2;     // window in seconds  (Seconds mode)

    void updateStats(double value);
};
