#include "ChartService.h"

#include <QFile>
#include <QImage>
#include <QPainter>

// ChartService.cpp — In-memory chart management and image export
//
// Implementation notes:
//   - Stores charts as ChartRecord objects keyed by auto-incrementing integer ID
//   - Export renders a placeholder image (title + dataset summary) via QPainter
//   - All mutations emit signals: chartCreated, chartUpdated, chartRemoved

ChartService::ChartService(QObject *parent) : QObject(parent) {}

int ChartService::createChart(const QString &type, const QString &title,
                              const ChartData &data) {
  const int id = nextId_++;
  ChartRecord rec{type, title, data};
  charts_.insert(id, rec);
  emit chartCreated(id);
  return id;
}

void ChartService::updateChart(int chartId, const ChartData &data) {
  if (!charts_.contains(chartId)) {
    emit error(QStringLiteral("Chart %1 not found").arg(chartId));
    return;
  }
  charts_[chartId].data = data;
  emit chartUpdated(chartId);
}

void ChartService::removeChart(int chartId) {
  if (!charts_.contains(chartId)) return;
  charts_.remove(chartId);
  emit chartRemoved(chartId);
}

bool ChartService::exportChart(int chartId, const QString &filePath) {
  if (!charts_.contains(chartId)) {
    emit error(QStringLiteral("Chart %1 not found").arg(chartId));
    return false;
  }
  const auto &rec = charts_[chartId];
  QImage image(800, 600, QImage::Format_ARGB32);
  image.fill(Qt::white);
  QPainter p(&image);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setPen(Qt::black);
  p.setFont(QFont("Sans", 16));
  p.drawText(image.rect(), Qt::AlignTop | Qt::AlignHCenter, rec.title);
  p.setFont(QFont("Sans", 10));
  p.drawText(image.rect(), Qt::AlignCenter,
             QStringLiteral("%1 chart with %2 datasets")
                 .arg(rec.type)
                 .arg(rec.data.datasets.size()));
  p.end();
  return image.save(filePath);
}

ChartData ChartService::chartData(int chartId) const {
  if (charts_.contains(chartId)) return charts_[chartId].data;
  return {};
}

QString ChartService::chartTitle(int chartId) const {
  if (charts_.contains(chartId)) return charts_[chartId].title;
  return {};
}

QString ChartService::chartType(int chartId) const {
  if (charts_.contains(chartId)) return charts_[chartId].type;
  return {};
}

QVector<int> ChartService::chartIds() const {
  return QVector<int>(charts_.keyBegin(), charts_.keyEnd());
}
