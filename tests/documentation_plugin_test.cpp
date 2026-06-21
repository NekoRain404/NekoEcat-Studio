// DocumentationPluginTest — Tests for DocumentationPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Widget creation and initial state
//   - Document tree and content view
//   - Add, select, and search documents
//   - Bookmarks add/remove
//   - Annotations add/remove
//   - Case-insensitive search

// DocumentationPluginTest — Tests for DocumentationPlugin
//
// Test coverage:
//   - Plugin identity and ordering
//   - Document tree and content view
//   - Add/select/search documents
//   - Bookmark add/remove and table
//   - Annotation add/remove and table
//   - Search case insensitivity and results table

#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include "plugins/documentation/DocumentationPlugin.h"

class DocumentationPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display name, order, and visibility
  void testPluginIdentity() {
    DocumentationPlugin plugin;

    QCOMPARE(plugin.id(), QString("documentation"));
    QCOMPARE(plugin.displayName(), QString("Documentation"));
    QCOMPARE(plugin.defaultOrder(), 215);
    QCOMPARE(plugin.visible(), true);
  }

  // Verify main widget is created
  // Verify main widget is created
  void testWidgetCreation() {
    DocumentationPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify initial document, bookmark, annotation, and search counts
  // Verify initial document, bookmark, annotation, and search counts
  void testInitialState() {
    DocumentationPlugin plugin;

    QCOMPARE(plugin.documentCount(), 7); // built-in docs
    QCOMPARE(plugin.bookmarkCount(), 0);
    QCOMPARE(plugin.annotationCount(), 0);
    QCOMPARE(plugin.searchResultCount(), 0);
  }

  // Verify document tree widget has top-level items
  // Verify document tree widget exists with items
  void testDocTree() {
    DocumentationPlugin plugin;

    QTreeWidget *tree = plugin.docTree();
    QVERIFY(tree != nullptr);
    QVERIFY(tree->topLevelItemCount() > 0);
  }

  // Verify content view is read-only
  // Verify content view is read-only
  void testContentView() {
    DocumentationPlugin plugin;

    QTextEdit *view = plugin.contentView();
    QVERIFY(view != nullptr);
    QVERIFY(view->isReadOnly());
  }

  // Verify adding a document increments count
  // Verify adding a document increments count
  void testAddDocument() {
    DocumentationPlugin plugin;
    int initialCount = plugin.documentCount();

    DocumentationPlugin::DocEntry entry;
    entry.id = "test_doc";
    entry.title = "Test Document";
    entry.category = "Test";
    entry.content = "Test content";
    entry.tags = "test";

    plugin.addDocument(entry);
    QCOMPARE(plugin.documentCount(), initialCount + 1);
  }

  // Verify selecting a document emits signal
  // Verify selecting a document emits signal
  void testSelectDocument() {
    DocumentationPlugin plugin;
    QSignalSpy docSpy(&plugin, &DocumentationPlugin::documentSelected);

    plugin.selectDocument("quickstart");
    QCOMPARE(docSpy.count(), 1);
    QCOMPARE(docSpy.at(0).at(0).toString(), QString("quickstart"));
  }

  // Verify search finds matching documents
  // Verify search returns results and emits signal
  void testSearch() {
    DocumentationPlugin plugin;
    QSignalSpy searchSpy(&plugin, &DocumentationPlugin::searchCompleted);

    plugin.search("Protocol");
    QCOMPARE(plugin.searchResultCount(), 1);
    QCOMPARE(searchSpy.count(), 1);
  }

  // Verify search returns zero for non-existent term
  // Verify search with no matches returns zero results
  void testSearchNoResults() {
    DocumentationPlugin plugin;

    plugin.search("nonexistent_term_xyz");
    QCOMPARE(plugin.searchResultCount(), 0);
  }

  // Verify search returns multiple results
  // Verify search returns multiple results
  void testSearchMultipleResults() {
    DocumentationPlugin plugin;

    plugin.search("API");
    QVERIFY(plugin.searchResultCount() >= 1);
  }

  // Verify search is case-insensitive
  // Verify search is case insensitive
  void testSearchCaseInsensitive() {
    DocumentationPlugin plugin;

    plugin.search("protocol");
    QCOMPARE(plugin.searchResultCount(), 1);
  }

  // Verify search results table row count matches
  // Verify search results table row count matches result count
  void testSearchResultsTable() {
    DocumentationPlugin plugin;

    plugin.search("API");
    QTableWidget *table = plugin.searchResultsTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), plugin.searchResultCount());
  }

  // Verify adding a bookmark increments count and emits signal
  // Verify adding a bookmark increments count and emits signal
  void testAddBookmark() {
    DocumentationPlugin plugin;
    QSignalSpy bmSpy(&plugin, &DocumentationPlugin::bookmarkAdded);

    plugin.addBookmark("quickstart", "Quick Start Guide", "Getting Started");
    QCOMPARE(plugin.bookmarkCount(), 1);
    QCOMPARE(bmSpy.count(), 1);
  }

  // Verify bookmark table row count matches
  // Verify bookmark table row count matches bookmark count
  void testBookmarkTable() {
    DocumentationPlugin plugin;

    plugin.addBookmark("test", "Test", "Category");

    QTableWidget *table = plugin.bookmarkTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 1);
  }

  // Verify removing a bookmark decrements count
  // Verify removing a bookmark decrements count
  void testRemoveBookmark() {
    DocumentationPlugin plugin;

    plugin.addBookmark("test", "Test", "Cat");
    QCOMPARE(plugin.bookmarkCount(), 1);

    plugin.removeBookmark(0);
    QCOMPARE(plugin.bookmarkCount(), 0);
  }

  // Verify adding an annotation increments count
  // Verify adding an annotation increments count
  void testAddAnnotation() {
    DocumentationPlugin plugin;

    plugin.addAnnotation("quickstart", "This is a note");
    QCOMPARE(plugin.annotationCount(), 1);
  }

  // Verify annotation table row count matches
  // Verify annotation table row count matches annotation count
  void testAnnotationTable() {
    DocumentationPlugin plugin;

    plugin.addAnnotation("test", "Note");

    QTableWidget *table = plugin.annotationTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->rowCount(), 1);
  }

  // Verify removing an annotation decrements count
  // Verify removing an annotation decrements count
  void testRemoveAnnotation() {
    DocumentationPlugin plugin;

    plugin.addAnnotation("test", "Note");
    QCOMPARE(plugin.annotationCount(), 1);

    plugin.removeAnnotation(0);
    QCOMPARE(plugin.annotationCount(), 0);
  }

  // Verify multiple bookmarks can be added
  // Verify multiple bookmarks can be added
  void testMultipleBookmarks() {
    DocumentationPlugin plugin;

    plugin.addBookmark("doc1", "Doc 1", "Cat A");
    plugin.addBookmark("doc2", "Doc 2", "Cat B");
    plugin.addBookmark("doc3", "Doc 3", "Cat A");

    QCOMPARE(plugin.bookmarkCount(), 3);
  }

  // Verify multiple annotations can be added to same document
  // Verify multiple annotations can be added to same document
  void testMultipleAnnotations() {
    DocumentationPlugin plugin;

    plugin.addAnnotation("doc1", "Note 1");
    plugin.addAnnotation("doc1", "Note 2");
    plugin.addAnnotation("doc2", "Note 3");

    QCOMPARE(plugin.annotationCount(), 3);
  }
};

QTEST_MAIN(DocumentationPluginTest)
#include "documentation_plugin_test.moc"
