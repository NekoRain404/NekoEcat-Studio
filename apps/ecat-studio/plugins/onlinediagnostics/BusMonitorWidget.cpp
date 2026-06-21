#include "BusMonitorWidget.h"

#include <QPainter>
#include <QPaintEvent>

BusMonitorWidget::BusMonitorWidget(QWidget *parent) : QWidget(parent) {
  setMinimumHeight(200);
  setMinimumWidth(400);
}

void BusMonitorWidget::updateTraffic(const BusTraffic &traffic) {
  traffic_ = traffic;
  trafficHistory_.append(traffic.bandwidth);
  if (trafficHistory_.size() > kMaxHistory)
    trafficHistory_.removeFirst();
  update();
}

void BusMonitorWidget::updateErrorRate(const ErrorRate &rate) {
  errorRate_ = rate;
  errorHistory_.append(rate.rate);
  if (errorHistory_.size() > kMaxHistory)
    errorHistory_.removeFirst();
  update();
}

void BusMonitorWidget::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);

  QRect r = rect().adjusted(8, 8, -8, -8);
  drawBackground(p, r);

  int metricsH = 80;
  QRect graphRect(r.left(), r.top(), r.width(), r.height() - metricsH - 10);
  QRect metricsRect(r.left(), graphRect.bottom() + 10, r.width(), metricsH);

  drawTrafficGraph(p, graphRect);
  drawMetrics(p, metricsRect);
}

void BusMonitorWidget::drawBackground(QPainter &p, const QRect &rect) {
  p.fillRect(rect, QColor(30, 30, 40));
  p.setPen(QPen(QColor(60, 60, 80), 1));
  p.drawRect(rect);
}

void BusMonitorWidget::drawTrafficGraph(QPainter &p, const QRect &rect) {
  if (trafficHistory_.isEmpty()) return;

  p.setPen(QPen(QColor(80, 80, 100), 1));
  for (int i = 0; i < 5; ++i) {
    int y = rect.top() + (rect.height() * i) / 4;
    p.drawLine(rect.left(), y, rect.right(), y);
  }

  if (trafficHistory_.size() < 2) return;

  double maxVal = 1.0;
  for (double v : trafficHistory_)
    maxVal = qMax(maxVal, v);

  QPolygonF poly;
  for (int i = 0; i < trafficHistory_.size(); ++i) {
    double x = rect.left() + (static_cast<double>(i) / (kMaxHistory - 1)) * rect.width();
    double y = rect.bottom() - (trafficHistory_[i] / maxVal) * rect.height();
    poly << QPointF(x, y);
  }

  p.setPen(QPen(QColor(0, 180, 255), 2));
  p.drawPolyline(poly);

  if (!errorHistory_.isEmpty()) {
    QPolygonF errPoly;
    for (int i = 0; i < errorHistory_.size(); ++i) {
      double x = rect.left() + (static_cast<double>(i) / (kMaxHistory - 1)) * rect.width();
      double y = rect.bottom() - (errorHistory_[i] / qMax(maxVal, 1.0)) * rect.height();
      errPoly << QPointF(x, y);
    }
    p.setPen(QPen(QColor(255, 80, 80), 2));
    p.drawPolyline(errPoly);
  }

  p.setPen(QColor(180, 180, 200));
  p.drawText(rect.adjusted(4, 4, 0, 0), tr("Bandwidth (blue) / Errors (red)"));
}

void BusMonitorWidget::drawMetrics(QPainter &p, const QRect &rect) {
  p.fillRect(rect, QColor(40, 40, 55));
  p.setPen(QColor(200, 200, 220));
  QFont font = p.font();
  font.setPointSize(10);
  p.setFont(font);

  int colW = rect.width() / 4;
  auto drawMetric = [&](int col, const QString &label, const QString &value) {
    int x = rect.left() + col * colW + 10;
    p.setPen(QColor(140, 140, 160));
    p.drawText(x, rect.top() + 20, label);
    p.setPen(QColor(220, 220, 240));
    QFont vf = p.font();
    vf.setPointSize(12);
    vf.setBold(true);
    p.setFont(vf);
    p.drawText(x, rect.top() + 45, value);
    vf.setBold(false);
    vf.setPointSize(10);
    p.setFont(vf);
  };

  drawMetric(0, tr("TX Frames"), QString::number(traffic_.txFrames));
  drawMetric(1, tr("RX Frames"), QString::number(traffic_.rxFrames));
  drawMetric(2, tr("Bandwidth"), QString::number(traffic_.bandwidth, 'f', 2) + " Mbps");
  drawMetric(3, tr("Errors"), QString::number(errorRate_.totalErrors));
}
