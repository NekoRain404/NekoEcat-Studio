// TestDocumentationBrowserPlugin — Tests for DocumentationBrowserPlugin
//
// Test coverage:
//   - Plugin identity and visibility
//   - Doc tree, content viewer, search box, bookmarks widgets
//   - Add/remove/clear documents
//   - Add/remove/clear bookmarks
//   - Content text and search query get/set
//   - Export documentation
//   - Signal emissions (document, bookmark, search)

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTemporaryDir>
#include <QRegularExpression>
#include <QtTest/QtTest>

#include "plugins/documentationbrowser/DocumentationBrowserPlugin.h"

class TestDocumentationBrowserPlugin : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();
  void cleanupTestCase();
  // Verify plugin id, display names, order, and visibility
  void identity();
  // Verify main widget is created
  void widgetNotNull();
  // Verify document tree widget exists
  void docTree();
  // Verify content viewer widget exists
  void contentViewer();
  // Verify search box widget exists
  void searchBox();
  // Verify bookmarks widget exists
  void bookmarks();
  // Verify adding and removing documents updates count correctly
  void addAndRemoveDocuments();
  // Verify clearDocuments resets count to zero
  void clearDocuments();
  // Verify adding and removing bookmarks updates count correctly
  void addAndRemoveBookmarks();
  // Verify clearBookmarks resets count to zero
  void clearBookmarks();
  // Verify content text get/set
  void contentText();
  // Verify search query get/set
  void searchQuery();
  // Verify export creates documentation file
  void exportDocumentation();
  // Verify all expected signals are emitted on operations
  void signalEmissions();

private:
  DocumentationBrowserPlugin *plugin_ = nullptr;
};

void TestDocumentationBrowserPlugin::initTestCase() {
  plugin_ = new DocumentationBrowserPlugin(this);
}

void TestDocumentationBrowserPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

// Verify plugin id, display names, order, and visibility
void TestDocumentationBrowserPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("documentationbrowser"));
  QCOMPARE(plugin_->displayName(), QString("Documentation Browser"));
  QCOMPARE(plugin_->displayNameZh(), QString("文档浏览器"));
  QCOMPARE(plugin_->defaultOrder(), 350);
  QVERIFY(plugin_->visible());
}

// Verify main widget is created
void TestDocumentationBrowserPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

// Verify document tree widget is created
void TestDocumentationBrowserPlugin::docTree() {
  QVERIFY(plugin_->docTree() != nullptr);
}

// Verify content viewer widget is created
void TestDocumentationBrowserPlugin::contentViewer() {
  QVERIFY(plugin_->contentViewer() != nullptr);
}

// Verify search box widget is created
void TestDocumentationBrowserPlugin::searchBox() {
  QVERIFY(plugin_->searchBox() != nullptr);
}

// Verify bookmarks widget is created
void TestDocumentationBrowserPlugin::bookmarks() {
  QVERIFY(plugin_->bookmarks() != nullptr);
}

// Verify add, remove, and clear document operations
void TestDocumentationBrowserPlugin::addAndRemoveDocuments() {
  plugin_->clearDocuments();
  QCOMPARE(plugin_->documentCount(), 0);

  plugin_->addDocument("API", "EtherCAT Types Reference");
  QCOMPARE(plugin_->documentCount(), 1);

  plugin_->addDocument("User Guide", "Getting Started");
  QCOMPARE(plugin_->documentCount(), 2);

  plugin_->removeDocument("EtherCAT Types Reference");
  QCOMPARE(plugin_->documentCount(), 1);

  plugin_->removeDocument("NonExistent");
  QCOMPARE(plugin_->documentCount(), 1);

  plugin_->clearDocuments();
}

// Verify clearDocuments resets count to zero
void TestDocumentationBrowserPlugin::clearDocuments() {
  plugin_->addDocument("A", "Doc1");
  plugin_->addDocument("B", "Doc2");
  QCOMPARE(plugin_->documentCount(), 2);

  plugin_->clearDocuments();
  QCOMPARE(plugin_->documentCount(), 0);
}

// Verify add, remove, and clear bookmark operations
void TestDocumentationBrowserPlugin::addAndRemoveBookmarks() {
  plugin_->clearBookmarks();
  QCOMPARE(plugin_->bookmarkCount(), 0);

  plugin_->addBookmark("Architecture Overview");
  QCOMPARE(plugin_->bookmarkCount(), 1);

  plugin_->addBookmark("Plugin Guide");
  QCOMPARE(plugin_->bookmarkCount(), 2);

  plugin_->removeBookmark("Architecture Overview");
  QCOMPARE(plugin_->bookmarkCount(), 1);

  plugin_->removeBookmark("NonExistent");
  QCOMPARE(plugin_->bookmarkCount(), 1);

  plugin_->clearBookmarks();
}

// Verify clearBookmarks resets count to zero
void TestDocumentationBrowserPlugin::clearBookmarks() {
  plugin_->addBookmark("A");
  plugin_->addBookmark("B");
  QCOMPARE(plugin_->bookmarkCount(), 2);

  plugin_->clearBookmarks();
  QCOMPARE(plugin_->bookmarkCount(), 0);
}

// Verify content text get/set
void TestDocumentationBrowserPlugin::contentText() {
  plugin_->setContentText("documentation content");
  QCOMPARE(plugin_->contentText(), QString("documentation content"));

  plugin_->setContentText("");
  QCOMPARE(plugin_->contentText(), QString(""));
}

// Verify search query get/set
void TestDocumentationBrowserPlugin::searchQuery() {
  plugin_->setSearchQuery("ethercat master");
  QCOMPARE(plugin_->searchQuery(), QString("ethercat master"));

  plugin_->setSearchQuery("");
  QCOMPARE(plugin_->searchQuery(), QString(""));
}

// Verify export documentation to HTML
void TestDocumentationBrowserPlugin::exportDocumentation() {
  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  const QString tmpPath = dir.filePath("doc_browser_test_export.html");
  QVERIFY(plugin_->exportDocumentation(tmpPath, "HTML"));

  QTest::failOnWarning(QRegularExpression(
      QStringLiteral("QFSFileEngine::open: No file name specified")));
  QVERIFY(!plugin_->exportDocumentation(QString(), "HTML"));
  QVERIFY(!plugin_->exportDocumentation(dir.path(), "HTML"));
}

// Verify document, bookmark, and search signals are emitted
void TestDocumentationBrowserPlugin::signalEmissions() {
  QSignalSpy docAddSpy(plugin_, &DocumentationBrowserPlugin::documentAdded);
  QSignalSpy docRemoveSpy(plugin_, &DocumentationBrowserPlugin::documentRemoved);
  QSignalSpy bmAddSpy(plugin_, &DocumentationBrowserPlugin::bookmarkAdded);
  QSignalSpy bmRemoveSpy(plugin_, &DocumentationBrowserPlugin::bookmarkRemoved);
  QSignalSpy searchSpy(plugin_, &DocumentationBrowserPlugin::searchTriggered);

  plugin_->addDocument("Test", "SignalDoc");
  QCOMPARE(docAddSpy.count(), 1);
  QCOMPARE(docAddSpy.at(0).at(0).toString(), QString("SignalDoc"));

  plugin_->removeDocument("SignalDoc");
  QCOMPARE(docRemoveSpy.count(), 1);

  plugin_->addBookmark("SignalBookmark");
  QCOMPARE(bmAddSpy.count(), 1);
  QCOMPARE(bmAddSpy.at(0).at(0).toString(), QString("SignalBookmark"));

  plugin_->removeBookmark("SignalBookmark");
  QCOMPARE(bmRemoveSpy.count(), 1);

  plugin_->setSearchQuery("test query");
  QTest::keyClick(plugin_->searchBox(), Qt::Key_Return);
  QCOMPARE(searchSpy.count(), 1);
  QCOMPARE(searchSpy.at(0).at(0).toString(), QString("test query"));

  plugin_->setSearchQuery("");
}

QTEST_MAIN(TestDocumentationBrowserPlugin)
#include "documentationbrowser_plugin_test.moc"
