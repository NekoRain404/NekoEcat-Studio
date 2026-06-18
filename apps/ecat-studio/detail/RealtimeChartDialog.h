#pragma once

// RealtimeChartDialog — modal dialog for live value plotting.
// Records numeric values from Free Run entries over time and renders
// them as a scrolling line chart using QPainter (no QtCharts dependency).

#include <QDialog>
#include <QTimer>
#include <QVector>

class QLabel;
class QPushButton;
class QComboBox;

// A single data point in the time series.
struct ChartPoint {
    double value = 0.0;
    qint64 timestampMs = 0;  // milliseconds since dialog open
};

// Custom widget that renders the line chart.
class ChartWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = nullptr);

    void addPoint(double value);
    void clear();
    void setLabel(const QString &name, const QString &unit);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<ChartPoint> points_;
    QString seriesName_;
    QString unit_;
    double yMin_ = 0.0;
    double yMax_ = 1.0;
    double yRange_ = 1.0;
    qint64 startTimeMs_ = 0;

    void recalcRange();
};

// Modal dialog that combines a chart widget with controls.
class RealtimeChartDialog : public QDialog {
    Q_OBJECT

public:
    // entryName: display label, entryKey: for lookup, initialValue: first sample.
    explicit RealtimeChartDialog(const QString &entryName,
                                 const QString &entryKey,
                                 double initialValue,
                                 QWidget *parent = nullptr);

    // Call this from the Free Run poll cycle to feed new values.
    void feedValue(double value);
    void setFreeRunRow(int row) { freeRunRow_ = row; }
    int freeRunRow() const { return freeRunRow_; }

signals:
    // Emitted when the user starts or stops recording.
    void recordingChanged(bool active);

private slots:
    void toggleRecording();
    void onPollTick();

private:
    ChartWidget *chart_ = nullptr;
    QLabel *valueLabel_ = nullptr;
    QLabel *statsLabel_ = nullptr;
    QPushButton *recordBtn_ = nullptr;
    QComboBox *intervalCombo_ = nullptr;
    QTimer *pollTimer_ = nullptr;

    QString entryKey_;
    int freeRunRow_ = -1;
    bool recording_ = false;
    int maxPoints_ = 300;
    double lastValue_ = 0.0;
    double sum_ = 0.0;
    int count_ = 0;
    double peak_ = 0.0;
    double trough_ = 0.0;
    qint64 startTimeMs_ = 0;

    void updateStats(double value);
};
