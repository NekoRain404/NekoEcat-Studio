#pragma once

// EcatChartWidget — custom QPainter widget for rendering multiple chart types.
// Supports line, bar, pie, scatter, and gauge charts with anti-aliased
// rendering, legends, and tooltips.

#include <QColor>
#include <QPointF>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QWidget>

struct ChartDataset;

class EcatChartWidget : public QWidget {
    Q_OBJECT
public:
    enum ChartType { Line, Bar, Pie, Scatter, Gauge };

    explicit EcatChartWidget(QWidget* parent = nullptr);

    void setChartType(ChartType type);
    ChartType chartType() const { return type_; }

    void setTitle(const QString& title);
    void setLabels(const QStringList& labels);
    void setDatasets(const QVector<ChartDataset>& datasets);
    void setGaugeValue(double value, double min, double max);

    static constexpr int kColorCount = 10;
    static const QColor kColors[kColorCount];

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    void drawLineChart(QPainter& p, const QRect& area);
    void drawBarChart(QPainter& p, const QRect& area);
    void drawPieChart(QPainter& p, const QRect& area);
    void drawScatterChart(QPainter& p, const QRect& area);
    void drawGaugeChart(QPainter& p, const QRect& area);
    void drawLegend(QPainter& p, const QRect& area);
    void drawTitle(QPainter& p, const QRect& area);
    void drawTooltip(QPainter& p);

    ChartType type_ = Line;
    QString title_;
    QStringList labels_;
    QVector<ChartDataset> datasets_;
    double gaugeValue_ = 0.0;
    double gaugeMin_ = 0.0;
    double gaugeMax_ = 100.0;
    QPoint mousePos_;

    static constexpr int kMarginLeft = 60;
    static constexpr int kMarginRight = 20;
    static constexpr int kMarginTop = 40;
    static constexpr int kMarginBottom = 32;
};
