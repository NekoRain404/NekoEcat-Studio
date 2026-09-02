#pragma once

// BusMonitorWidget — Real-time bus traffic visualization widget.
//
// Renders bus traffic data using QPainter with frame counters,
// error rate display, bandwidth utilization, and traffic history graph.

#include "services/EtherCATMonitorService.h"

#include <QVector>
#include <QWidget>

class BusMonitorWidget : public QWidget {
    Q_OBJECT
public:
    explicit BusMonitorWidget(QWidget* parent = nullptr);

    void updateTraffic(const BusTraffic& traffic);
    void updateErrorRate(const ErrorRate& rate);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawBackground(QPainter& p, const QRect& rect);
    void drawTrafficGraph(QPainter& p, const QRect& rect);
    void drawMetrics(QPainter& p, const QRect& rect);

    BusTraffic traffic_;
    ErrorRate errorRate_;
    QVector<double> trafficHistory_;
    QVector<double> errorHistory_;
    static constexpr int kMaxHistory = 120;
};
