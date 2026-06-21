#pragma once

// ChartService — manages chart data and provides chart creation/update/export
// for the ChartPlugin and DashboardPlugin.
//
// This service provides chart management capabilities for data visualization.
// It handles:
//   - Chart creation with various types (line, bar, pie, scatter, gauge)
//   - Chart data updates and management
//   - Chart export to various formats
//   - Chart metadata management (title, type, datasets)
//   - Multiple chart instances with unique IDs
//
// Usage:
//   ServiceContainer *container = ...;
//   ChartService *chart = container->chart();
//   ChartData data;
//   data.labels << "Jan" << "Feb" << "Mar";
//   ChartDataset ds;
//   ds.name = "Sales";
//   ds.values << 100.0 << 150.0 << 200.0;
//   ds.color = Qt::blue;
//   data.datasets << ds;
//   int chartId = chart->createChart("line", "Monthly Sales", data);
//   chart->exportChart(chartId, "/path/to/chart.png");
//
// Thread safety:
//   All methods must be called from the main (GUI) thread. Chart data
//   is stored in memory and accessed synchronously.
//
// Performance:
//   - Chart creation is O(1)
//   - Chart updates are O(n) where n is number of data points
//   - Chart export is O(n) for rendering
//   - Memory usage is bounded by number of charts

#include <QObject>
#include <QColor>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVector>

// Represents a single dataset in a chart.
struct ChartDataset {
  QString name;              // Dataset name (for legend)
  QVector<double> values;    // Data values
  QColor color;              // Dataset color
};

// Represents chart data with labels and datasets.
struct ChartData {
  QStringList labels;            // X-axis labels
  QVector<ChartDataset> datasets; // Datasets to plot
};

class ChartService : public QObject {
  Q_OBJECT
public:
  explicit ChartService(QObject *parent = nullptr);

  // Create a new chart with specified type, title, and data.
  // @param type   Chart type ("line", "bar", "pie", "scatter", "gauge")
  // @param title  Chart title
  // @param data   Chart data with labels and datasets
  // @return Chart ID for future reference
  int createChart(const QString &type, const QString &title, const ChartData &data);

  // Update an existing chart with new data.
  // @param chartId  Chart ID to update
  // @param data     New chart data
  void updateChart(int chartId, const ChartData &data);

  // Remove a chart by ID.
  // @param chartId  Chart ID to remove
  void removeChart(int chartId);

  // Export a chart to a file.
  // @param chartId   Chart ID to export
  // @param filePath  Path to export the chart to
  // @return true if export was successful
  bool exportChart(int chartId, const QString &filePath);

  // Get the data for a specific chart.
  // @param chartId  Chart ID to get data for
  // @return ChartData structure
  ChartData chartData(int chartId) const;

  // Get the title of a specific chart.
  // @param chartId  Chart ID to get title for
  // @return Chart title
  QString chartTitle(int chartId) const;

  // Get the type of a specific chart.
  // @param chartId  Chart ID to get type for
  // @return Chart type
  QString chartType(int chartId) const;

  // Get all chart IDs.
  // @return Vector of chart IDs
  QVector<int> chartIds() const;

signals:
  // Emitted when a new chart is created.
  // @param chartId  Chart ID of the new chart
  void chartCreated(int chartId);

  // Emitted when a chart is updated.
  // @param chartId  Chart ID that was updated
  void chartUpdated(int chartId);

  // Emitted when a chart is removed.
  // @param chartId  Chart ID that was removed
  void chartRemoved(int chartId);

  // Emitted when an error occurs.
  // @param message  Human-readable error message
  void error(const QString &message);

private:
  // Internal chart record structure.
  struct ChartRecord {
    QString type;      // Chart type
    QString title;     // Chart title
    ChartData data;    // Chart data
  };

  QMap<int, ChartRecord> charts_;  // All charts by ID
  int nextId_ = 1;                 // Next chart ID to assign
};
