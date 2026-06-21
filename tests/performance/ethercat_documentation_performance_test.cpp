#include <QTest>
#include <QElapsedTimer>
#include "services/EtherCATDocumentationService.h"
#include "services/EventBus.h"
#include "infra/EcatClient.h"

class EtherCATDocumentationPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testGenerateApiDocumentationThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATDocumentationService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      Documentation d = svc.generateApiDocumentation();
      Q_UNUSED(d);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "API doc throughput:" << count << "docs in" << elapsed << "ms";
  }

  void testGenerateUserDocumentationThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATDocumentationService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      Documentation d = svc.generateUserDocumentation();
      Q_UNUSED(d);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "User doc throughput:" << count << "docs in" << elapsed << "ms";
  }

  void testGenerateDeveloperDocumentationThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATDocumentationService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      Documentation d = svc.generateDeveloperDocumentation();
      Q_UNUSED(d);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "Developer doc throughput:" << count << "docs in" << elapsed << "ms";
  }

  void testGenerateSystemDocumentationThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATDocumentationService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      Documentation d = svc.generateSystemDocumentation();
      Q_UNUSED(d);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "System doc throughput:" << count << "docs in" << elapsed << "ms";
  }

  void testSearchDocumentationThroughput() {
    EventBus bus;
    EcatClient client;
    EtherCATDocumentationService svc(&bus, &client);

    QElapsedTimer timer;
    timer.start();

    const int count = 1000;
    for (int i = 0; i < count; i++) {
      QVector<SearchResult> results = svc.searchDocumentation("ethercat master");
      Q_UNUSED(results);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 2000);
    qDebug() << "Search throughput:" << count << "searches in" << elapsed << "ms";
  }
};

QTEST_MAIN(EtherCATDocumentationPerformanceTest)
#include "ethercat_documentation_performance_test.moc"
