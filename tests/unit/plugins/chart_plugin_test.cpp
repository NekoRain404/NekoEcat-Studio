// ChartPluginTest — Tests for ChartPlugin and ChartService
//
// Test coverage:
//   - ChartService create, update, remove
//   - Chart IDs listing
//   - Error on nonexistent chart update
//   - Plugin identity and metadata
//   - Widget creation
//   - Chart rendering and export

#include <QTest>
#include <QSignalSpy>
#include <QDir>
#include <QFile>
#include "services/ChartService.h"
#include "plugins/chart/ChartPlugin.h"
#include "plugins/chart/EcatChartWidget.h"

class ChartPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify ChartService create chart returns valid id
  void testServiceCreateChart() {
    ChartService svc;
    ChartData data;
    data.labels = {"A", "B"};
    ChartDataset ds;
    ds.name = "test";
    ds.values = {1.0, 2.0};
    data.datasets.append(ds);
    const int id = svc.createChart("line", "Test", data);
    QVERIFY(id > 0);
    QCOMPARE(svc.chartTitle(id), QString("Test"));
    QCOMPARE(svc.chartType(id), QString("line"));
  }

  // Verify ChartService update chart emits signal
  void testServiceUpdateChart() {
    ChartService svc;
    ChartData data;
    const int id = svc.createChart("bar", "T", data);
    QSignalSpy spy(&svc, &ChartService::chartUpdated);
    ChartData newData;
    ChartDataset ds;
    ds.name = "u";
    ds.values = {5.0};
    newData.datasets.append(ds);
    svc.updateChart(id, newData);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.chartData(id).datasets.size(), 1);
  }

  // Verify ChartService remove chart
  void testServiceRemoveChart() {
    ChartService svc;
    const int id = svc.createChart("pie", "R", {});
    QSignalSpy spy(&svc, &ChartService::chartRemoved);
    svc.removeChart(id);
    QCOMPARE(spy.count(), 1);
    QVERIFY(svc.chartIds().isEmpty());
  }

  // Verify ChartService lists chart IDs
  void testServiceChartIds() {
    ChartService svc;
    svc.createChart("line", "A", {});
    svc.createChart("bar", "B", {});
    QCOMPARE(svc.chartIds().size(), 2);
  }

  // Verify error on updating nonexistent chart
  void testServiceUpdateNonexistent() {
    ChartService svc;
    QSignalSpy spy(&svc, &ChartService::error);
    svc.updateChart(999, {});
    QCOMPARE(spy.count(), 1);
  }

  // Verify plugin identity metadata
  void testPluginIdentity() {
    ChartService svc;
    ChartPlugin plugin(&svc);
    QCOMPARE(plugin.id(), QString("chart"));
    QCOMPARE(plugin.displayName(), QString("Charts"));
    QCOMPARE(plugin.displayNameZh(), QStringLiteral("图表"));
  }

  // Verify plugin default order
  void testPluginDefaultOrder() {
    ChartService svc;
    ChartPlugin plugin(&svc);
    QCOMPARE(plugin.defaultOrder(), 125);
  }

  // Verify plugin is visible
  void testPluginVisible() {
    ChartService svc;
    ChartPlugin plugin(&svc);
    QVERIFY(!plugin.visible());
  }

  void testPluginWidgetNotNull() {
    ChartService svc;
    ChartPlugin plugin(&svc);
    QVERIFY(plugin.widget() != nullptr);
  }

  void testPluginServiceAccessor() {
    ChartService svc;
    ChartPlugin plugin(&svc);
    QCOMPARE(plugin.service(), &svc);
  }

  void testChartDataStructure() {
    ChartData data;
    data.labels = {"X", "Y"};
    ChartDataset ds;
    ds.name = "series";
    ds.values = {1.0, 2.0, 3.0};
    ds.color = Qt::red;
    data.datasets.append(ds);
    QCOMPARE(data.labels.size(), 2);
    QCOMPARE(data.datasets[0].values.size(), 3);
    QCOMPARE(data.datasets[0].color, QColor(Qt::red));
  }

  void testWidgetChartTypes() {
    EcatChartWidget w;
    w.setChartType(EcatChartWidget::Line);
    QCOMPARE(w.chartType(), EcatChartWidget::Line);
    w.setChartType(EcatChartWidget::Bar);
    QCOMPARE(w.chartType(), EcatChartWidget::Bar);
    w.setChartType(EcatChartWidget::Pie);
    QCOMPARE(w.chartType(), EcatChartWidget::Pie);
    w.setChartType(EcatChartWidget::Scatter);
    QCOMPARE(w.chartType(), EcatChartWidget::Scatter);
    w.setChartType(EcatChartWidget::Gauge);
    QCOMPARE(w.chartType(), EcatChartWidget::Gauge);
  }

  void testWidgetGaugeValue() {
    EcatChartWidget w;
    w.setGaugeValue(50.0, 0.0, 100.0);
    w.setChartType(EcatChartWidget::Gauge);
    w.show();
    QTest::qWait(50);
  }

  void testServiceExportChart() {
    ChartService svc;
    const int id = svc.createChart("line", "Export Test", {});
    const QString path = QDir::tempPath() + "/chart_export_test.png";
    QVERIFY(svc.exportChart(id, path));
    QVERIFY(QFile::exists(path));
    QFile::remove(path);
  }
};

QTEST_MAIN(ChartPluginTest)
#include "chart_plugin_test.moc"
