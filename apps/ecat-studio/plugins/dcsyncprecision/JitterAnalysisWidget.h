#pragma once

#include <QVector>
#include <QWidget>

class JitterAnalysisWidget : public QWidget {
    Q_OBJECT
public:
    explicit JitterAnalysisWidget(QWidget* parent = nullptr);

    void setJitterData(double min, double max, double avg, double stddev, int sampleCount);
    void setHistogram(const QVector<int>& bins, double binWidth, double minVal);
    void clear();

    double jitterMin() const { return min_; }
    double jitterMax() const { return max_; }
    double jitterAvg() const { return avg_; }
    double jitterStddev() const { return stddev_; }
    int sampleCount() const { return sampleCount_; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawStatsPanel(QPainter& p, const QRect& area);
    void drawHistogram(QPainter& p, const QRect& area);
    void drawTrendLine(QPainter& p, const QRect& area);

    double min_ = 0.0;
    double max_ = 0.0;
    double avg_ = 0.0;
    double stddev_ = 0.0;
    int sampleCount_ = 0;

    QVector<int> histogram_;
    double histBinWidth_ = 10.0;
    double histMin_ = 0.0;
    QVector<double> trendHistory_;
    int maxTrendSamples_ = 100;
};
