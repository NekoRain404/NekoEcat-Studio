#pragma once

#include <QColor>
#include <QVector>
#include <QWidget>

struct DriftStatusEx;

class DriftMonitorWidget : public QWidget {
    Q_OBJECT
public:
    explicit DriftMonitorWidget(QWidget* parent = nullptr);

    void addSample(double drift);
    void setThreshold(double ns);
    void setHistorySize(int count);
    void clear();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawGrid(QPainter& p, const QRect& area);
    void drawThresholdLines(QPainter& p, const QRect& area);
    void drawDriftCurve(QPainter& p, const QRect& area);
    void drawLegend(QPainter& p, const QRect& area);

    QColor colorForDrift(double drift) const;

    QVector<double> samples_;
    double threshold_ = 1000.0;
    int maxSamples_ = 200;
};
