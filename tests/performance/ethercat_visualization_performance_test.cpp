#include <QTest>
#include <QElapsedTimer>
#include "EthercatTypes.h"
#include "services/EtherCATVisualizationService.h"
#include "services/EtherCATAnalyticsService.h"
#include "services/EtherCATMonitorService.h"
#include "services/NetworkDiagnosticsService.h"

class EtherCATVisualizationPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testCreateTopologyViewThroughput() {
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      EtherCATVisualizationService svc(nullptr, nullptr);
      QVector<SlaveInfo> slaves;
      for (int j = 0; j < 50; j++) {
        SlaveInfo s;
        s.position = j;
        s.name = QString("Slave_%1").arg(j);
        s.state = "OP";
        slaves.append(s);
      }
      svc.createTopologyView(slaves);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Topology view throughput:" << count << "views in" << elapsed << "ms";
  }

  void testCreateDataViewThroughput() {
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      EtherCATVisualizationService svc(nullptr, nullptr);
      QVector<DataPoint> data;
      for (int j = 0; j < 100; j++) {
        DataPoint dp;
        dp.source = QString("Point_%1").arg(j);
        dp.value = static_cast<double>(j) * 1.5;
        dp.timestamp = j * 1000;
        data.append(dp);
      }
      svc.createDataView(data);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Data view throughput:" << count << "views in" << elapsed << "ms";
  }

  void testCreatePerformanceViewThroughput() {
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      EtherCATVisualizationService svc(nullptr, nullptr);
      PerformanceMetrics metrics;
      metrics.cycleTimeUs = 1000.0;
      metrics.jitterUs = 50.0;
      metrics.frameLossRate = 0.01;
      metrics.sdoResponseMs = 2.0;
      metrics.pdoUpdateRate = 1000.0;
      svc.createPerformanceView(metrics);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Performance view throughput:" << count << "views in" << elapsed << "ms";
  }

  void testCreateErrorViewThroughput() {
    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      EtherCATVisualizationService svc(nullptr, nullptr);
      QVector<ErrorInfo> errors;
      for (int j = 0; j < 20; j++) {
        ErrorInfo e;
        e.timestampMs = 1000 + j;
        e.port = j % 4;
        e.type = "CRC";
        e.description = QString("Error_%1").arg(j);
        errors.append(e);
      }
      svc.createErrorView(errors);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Error view throughput:" << count << "views in" << elapsed << "ms";
  }

  void testSetConfigThroughput() {
    EtherCATVisualizationService svc(nullptr, nullptr);
    VisualizationConfig cfg;
    cfg.viewType = "topology";
    cfg.dataSource = "live";
    cfg.layout = "grid";
    cfg.animations = true;
    cfg.interactions = true;

    QElapsedTimer timer;
    timer.start();

    const int count = 10000;
    for (int i = 0; i < count; i++) {
      svc.setConfig(cfg);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 1000);
    qDebug() << "SetConfig throughput:" << count << "calls in" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATVisualizationPerformanceTest)
#include "ethercat_visualization_performance_test.moc"
