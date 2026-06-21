#include <QTest>
#include <QElapsedTimer>
#include "services/WorkflowDocumentationBrowserService.h"

class WorkflowDocumentationBrowserPerformanceTest : public QObject {
  Q_OBJECT
private slots:
  void testAddNodeThroughput() {
    WorkflowDocumentationBrowserService svc;
    QElapsedTimer timer;
    timer.start();
    const int count = 10000;
    for (int i = 0; i < count; i++) {
      svc.addNode({}, QString("Node_%1").arg(i), "Content");
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "AddNode throughput:" << count << "nodes in" << elapsed << "ms";
  }

  void testSearchThroughput() {
    WorkflowDocumentationBrowserService svc;
    for (int i = 0; i < 500; i++) {
      svc.addNode({}, QString("Node_%1").arg(i), QString("Content about topic_%1").arg(i));
    }
    QElapsedTimer timer;
    timer.start();
    const int count = 1000;
    for (int i = 0; i < count; i++) {
      svc.search("topic");
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Search throughput:" << count << "searches in" << elapsed << "ms";
  }

  void testBookmarkThroughput() {
    WorkflowDocumentationBrowserService svc;
    QVector<QString> nodeIds;
    for (int i = 0; i < 1000; i++) {
      nodeIds << svc.addNode({}, QString("Node_%1").arg(i), "Content");
    }
    QElapsedTimer timer;
    timer.start();
    for (const auto &id : nodeIds) {
      svc.addBookmark(id, "Bookmark");
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "Bookmark throughput:" << nodeIds.size() << "bookmarks in" << elapsed << "ms";
  }

  void testChildNodesThroughput() {
    WorkflowDocumentationBrowserService svc;
    QString parentId = svc.addNode({}, "Parent", "Content");
    for (int i = 0; i < 1000; i++) {
      svc.addNode(parentId, QString("Child_%1").arg(i), "Content");
    }
    QElapsedTimer timer;
    timer.start();
    const int count = 10000;
    for (int i = 0; i < count; i++) {
      svc.childNodes(parentId);
    }
    qint64 elapsed = timer.elapsed();
    QVERIFY(elapsed < 5000);
    qDebug() << "ChildNodes throughput:" << count << "queries in" << elapsed << "ms";
  }
};

QTEST_MAIN(WorkflowDocumentationBrowserPerformanceTest)
#include "workflow_documentation_browser_performance_test.moc"
