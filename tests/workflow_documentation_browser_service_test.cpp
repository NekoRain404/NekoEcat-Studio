#include <QTest>
#include <QSignalSpy>
#include "services/WorkflowDocumentationBrowserService.h"

class WorkflowDocumentationBrowserServiceTest : public QObject {
  Q_OBJECT
private slots:
  void testAddNode() {
    WorkflowDocumentationBrowserService svc;
    QSignalSpy spy(&svc, &WorkflowDocumentationBrowserService::nodeAdded);
    QString id = svc.addNode({}, "Root", "Root content");
    QVERIFY(!id.isEmpty());
    QVERIFY(id.startsWith("node_"));
    QCOMPARE(spy.count(), 1);
    WfDocNode n = svc.node(id);
    QCOMPARE(n.title, QString("Root"));
    QCOMPARE(n.content, QString("Root content"));
    QCOMPARE(n.level, 0);
  }

  void testRemoveNode() {
    WorkflowDocumentationBrowserService svc;
    QString id = svc.addNode({}, "ToRemove", "Content");
    QSignalSpy spy(&svc, &WorkflowDocumentationBrowserService::nodeRemoved);
    QVERIFY(svc.removeNode(id));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.nodeCount(), 0);
  }

  void testNodeLookup() {
    WorkflowDocumentationBrowserService svc;
    QString id = svc.addNode({}, "Lookup", "Content");
    WfDocNode n = svc.node(id);
    QCOMPARE(n.id, id);
    QCOMPARE(n.title, QString("Lookup"));
  }

  void testChildNodes() {
    WorkflowDocumentationBrowserService svc;
    QString parentId = svc.addNode({}, "Parent", "Content");
    svc.addNode(parentId, "Child1", "C1");
    svc.addNode(parentId, "Child2", "C2");
    svc.addNode(parentId, "Child3", "C3");
    QCOMPARE(svc.childNodes(parentId).size(), 3);
  }

  void testSearch() {
    WorkflowDocumentationBrowserService svc;
    svc.addNode({}, "EtherCAT Setup", "How to configure EtherCAT");
    svc.addNode({}, "Network Guide", "Network configuration guide");
    svc.addNode({}, "Troubleshooting", "Common issues and fixes");
    QSignalSpy spy(&svc, &WorkflowDocumentationBrowserService::searchCompleted);
    auto results = svc.search("EtherCAT");
    QCOMPARE(spy.count(), 1);
    QVERIFY(results.size() >= 1);
    bool foundTitle = false;
    for (const auto &r : results) {
      if (r.title.contains("EtherCAT"))
        foundTitle = true;
    }
    QVERIFY(foundTitle);
  }

  void testSearchNotFound() {
    WorkflowDocumentationBrowserService svc;
    svc.addNode({}, "Test", "Content");
    auto results = svc.search("nonexistent_query_xyz");
    QCOMPARE(results.size(), 0);
  }

  void testSearchRelevance() {
    WorkflowDocumentationBrowserService svc;
    svc.addNode({}, "Target", "Some content");
    svc.addNode({}, "Other", "Contains target in body");
    auto results = svc.search("target");
    for (const auto &r : results) {
      QVERIFY(r.relevance > 0.0);
    }
  }

  void testAddBookmark() {
    WorkflowDocumentationBrowserService svc;
    QString nodeId = svc.addNode({}, "Node", "Content");
    QSignalSpy spy(&svc, &WorkflowDocumentationBrowserService::bookmarkAdded);
    QString bmId = svc.addBookmark(nodeId, "My Bookmark", "Important");
    QVERIFY(!bmId.isEmpty());
    QVERIFY(bmId.startsWith("bm_"));
    QCOMPARE(spy.count(), 1);
  }

  void testRemoveBookmark() {
    WorkflowDocumentationBrowserService svc;
    QString nodeId = svc.addNode({}, "Node", "Content");
    QString bmId = svc.addBookmark(nodeId, "Bookmark");
    QSignalSpy spy(&svc, &WorkflowDocumentationBrowserService::bookmarkRemoved);
    QVERIFY(svc.removeBookmark(bmId));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(svc.bookmarkCount(), 0);
  }

  void testAllBookmarks() {
    WorkflowDocumentationBrowserService svc;
    QString n1 = svc.addNode({}, "N1", "C1");
    QString n2 = svc.addNode({}, "N2", "C2");
    svc.addBookmark(n1, "BM1");
    svc.addBookmark(n2, "BM2");
    QCOMPARE(svc.allBookmarks().size(), 2);
  }

  void testBookmarkCount() {
    WorkflowDocumentationBrowserService svc;
    QCOMPARE(svc.bookmarkCount(), 0);
    QString n = svc.addNode({}, "N", "C");
    svc.addBookmark(n, "BM1");
    QCOMPARE(svc.bookmarkCount(), 1);
    svc.addBookmark(n, "BM2");
    QCOMPARE(svc.bookmarkCount(), 2);
  }

  void testNodeCount() {
    WorkflowDocumentationBrowserService svc;
    QCOMPARE(svc.nodeCount(), 0);
    svc.addNode({}, "N1", "C1");
    QCOMPARE(svc.nodeCount(), 1);
    svc.addNode({}, "N2", "C2");
    QCOMPARE(svc.nodeCount(), 2);
  }

  void testParentChildCount() {
    WorkflowDocumentationBrowserService svc;
    QString parentId = svc.addNode({}, "Parent", "Content");
    svc.addNode(parentId, "Child1", "C1");
    svc.addNode(parentId, "Child2", "C2");
    WfDocNode parent = svc.node(parentId);
    QCOMPARE(parent.childCount, 2);
  }

  void testRemoveNodeRemovesBookmarks() {
    WorkflowDocumentationBrowserService svc;
    QString nodeId = svc.addNode({}, "Node", "Content");
    svc.addBookmark(nodeId, "BM1");
    svc.addBookmark(nodeId, "BM2");
    QCOMPARE(svc.bookmarkCount(), 2);
    svc.removeNode(nodeId);
    QCOMPARE(svc.bookmarkCount(), 0);
  }

  void testSignalEmissions() {
    WorkflowDocumentationBrowserService svc;
    QSignalSpy naSpy(&svc, &WorkflowDocumentationBrowserService::nodeAdded);
    QSignalSpy nrSpy(&svc, &WorkflowDocumentationBrowserService::nodeRemoved);
    QSignalSpy baSpy(&svc, &WorkflowDocumentationBrowserService::bookmarkAdded);
    QSignalSpy brSpy(&svc, &WorkflowDocumentationBrowserService::bookmarkRemoved);
    QSignalSpy scSpy(&svc, &WorkflowDocumentationBrowserService::searchCompleted);

    QString nodeId = svc.addNode({}, "Test", "Content");
    svc.search("Test");
    QString bmId = svc.addBookmark(nodeId, "BM");
    svc.removeBookmark(bmId);
    svc.removeNode(nodeId);

    QCOMPARE(naSpy.count(), 1);
    QCOMPARE(nrSpy.count(), 1);
    QCOMPARE(baSpy.count(), 1);
    QCOMPARE(brSpy.count(), 1);
    QCOMPARE(scSpy.count(), 1);
  }
};

QTEST_MAIN(WorkflowDocumentationBrowserServiceTest)
#include "workflow_documentation_browser_service_test.moc"
