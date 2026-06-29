// TestReportDesignerPlugin — Tests for Report Designer Plugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Layout editor, templates, data bindings, preview pane
//   - Add/remove/clear templates and data bindings
//   - Preview text management
//   - Export/import functionality
//   - Signal emissions for template, binding, and preview events
#include <QApplication>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTemporaryDir>
#include <QRegularExpression>
#include <QtTest/QtTest>

#include "plugins/reportdesigner/ReportDesignerPlugin.h"

class TestReportDesignerPlugin : public QObject {
  Q_OBJECT
private slots:
  // Set up plugin instance
  void initTestCase();
  // Clean up plugin instance
  void cleanupTestCase();
  // Verify plugin id, display names, order, and visibility
  void identity();
  // Widget should be created successfully
  void widgetNotNull();
  // Layout editor widget is valid and meets minimum width
  void layoutEditor();
  // Templates list is populated with minimum count
  void templates();
  // Data bindings list is populated with minimum count
  void dataBindings();
  // Preview pane widget exists
  void previewPane();
  // Add and remove templates, verify count changes
  void addAndRemoveTemplates();
  // Remove multiple templates and verify cleanup
  void clearTemplates();
  // Add and remove data bindings, verify count changes
  void addAndRemoveDataBindings();
  // Clear all data bindings and verify empty state
  void clearDataBindings();
  // Set and retrieve preview text content
  void previewText();
  // Export report and import template, verify round-trip
  void exportImport();
  // Invalid imports should not clear current templates or preview
  void rejectInvalidImport();
  // Verify signals fire for template, binding, and preview changes
  void signalEmissions();

private:
  ReportDesignerPlugin *plugin_ = nullptr;
};

void TestReportDesignerPlugin::initTestCase() {
  plugin_ = new ReportDesignerPlugin(this);
}

void TestReportDesignerPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

void TestReportDesignerPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("reportdesigner"));
  QCOMPARE(plugin_->displayName(), QString("Report Designer"));
  QCOMPARE(plugin_->displayNameZh(), QString("报告设计器"));
  QCOMPARE(plugin_->defaultOrder(), 345);
  QVERIFY(!plugin_->visible());
}

void TestReportDesignerPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

void TestReportDesignerPlugin::layoutEditor() {
  QVERIFY(plugin_->layoutEditor() != nullptr);
  QVERIFY(plugin_->layoutEditor()->minimumWidth() >= 400);
}

void TestReportDesignerPlugin::templates() {
  QVERIFY(plugin_->templates() != nullptr);
  QVERIFY(plugin_->templateCount() >= 5);
}

void TestReportDesignerPlugin::dataBindings() {
  QVERIFY(plugin_->dataBindings() != nullptr);
  QVERIFY(plugin_->dataBindingCount() >= 7);
}

void TestReportDesignerPlugin::previewPane() {
  QVERIFY(plugin_->previewPane() != nullptr);
}

void TestReportDesignerPlugin::addAndRemoveTemplates() {
  int initial = plugin_->templateCount();

  plugin_->addTemplate("Custom Report");
  QCOMPARE(plugin_->templateCount(), initial + 1);

  plugin_->removeTemplate("Custom Report");
  QCOMPARE(plugin_->templateCount(), initial);

  plugin_->removeTemplate("NonExistent");
  QCOMPARE(plugin_->templateCount(), initial);
}

void TestReportDesignerPlugin::clearTemplates() {
  plugin_->addTemplate("A");
  plugin_->addTemplate("B");
  QVERIFY(plugin_->templateCount() >= 2);

  int before = plugin_->templateCount();
  plugin_->removeTemplate("A");
  plugin_->removeTemplate("B");
  QCOMPARE(plugin_->templateCount(), before - 2);
}

void TestReportDesignerPlugin::addAndRemoveDataBindings() {
  int initial = plugin_->dataBindingCount();

  plugin_->addDataBinding("Custom", "MyField");
  QCOMPARE(plugin_->dataBindingCount(), initial + 1);

  plugin_->removeDataBinding("MyField");
  QCOMPARE(plugin_->dataBindingCount(), initial);
}

void TestReportDesignerPlugin::clearDataBindings() {
  plugin_->addDataBinding("Test", "X");
  plugin_->addDataBinding("Test", "Y");
  QVERIFY(plugin_->dataBindingCount() >= 2);

  plugin_->clearDataBindings();
  QCOMPARE(plugin_->dataBindingCount(), 0);
}

void TestReportDesignerPlugin::previewText() {
  plugin_->setPreviewText("report preview content");
  QCOMPARE(plugin_->previewText(), QString("report preview content"));

  plugin_->setPreviewText("");
  QCOMPARE(plugin_->previewText(), QString(""));
}

void TestReportDesignerPlugin::exportImport() {
  plugin_->addTemplate("ExportTest");
  plugin_->setPreviewText("export preview");

  QTemporaryDir dir;
  QVERIFY(dir.isValid());
  const QString tmpPath = dir.filePath("report_designer_test_export.json");
  QVERIFY(plugin_->exportReport(tmpPath, "PDF"));

  QTest::failOnWarning(QRegularExpression(
      QStringLiteral("QFSFileEngine::open: No file name specified")));
  QVERIFY(!plugin_->exportReport(QString(), "PDF"));
  QVERIFY(!plugin_->importTemplate(QString()));
  QVERIFY(!plugin_->exportReport(dir.path(), "PDF"));

  plugin_->clearTemplates();
  plugin_->setPreviewText("");

  QVERIFY(plugin_->importTemplate(tmpPath));
  QCOMPARE(plugin_->previewText(), QString("export preview"));

  plugin_->clearTemplates();
  plugin_->clearDataBindings();
}

void TestReportDesignerPlugin::rejectInvalidImport() {
  plugin_->clearTemplates();
  plugin_->addTemplate("KeepTemplate");
  plugin_->setPreviewText("keep preview");

  QTemporaryDir dir;
  QVERIFY(dir.isValid());

  const QString emptyObjectPath = dir.filePath("empty-object.json");
  QFile emptyObjectFile(emptyObjectPath);
  QVERIFY(emptyObjectFile.open(QIODevice::WriteOnly));
  QCOMPARE(emptyObjectFile.write(QByteArrayLiteral("{}")), 2);
  emptyObjectFile.close();

  const QString emptyTemplatesPath = dir.filePath("empty-templates.json");
  QFile emptyTemplatesFile(emptyTemplatesPath);
  QVERIFY(emptyTemplatesFile.open(QIODevice::WriteOnly));
  QVERIFY(emptyTemplatesFile.write(QByteArrayLiteral(
              "{\"templates\":[],\"preview\":\"discard\"}")) > 0);
  emptyTemplatesFile.close();

  QVERIFY(!plugin_->importTemplate(emptyObjectPath));
  QVERIFY(!plugin_->importTemplate(emptyTemplatesPath));
  QCOMPARE(plugin_->templateCount(), 1);
  QCOMPARE(plugin_->templates()->item(0)->text(), QString("KeepTemplate"));
  QCOMPARE(plugin_->previewText(), QString("keep preview"));
}

void TestReportDesignerPlugin::signalEmissions() {
  QSignalSpy tmplAddSpy(plugin_, &ReportDesignerPlugin::templateAdded);
  QSignalSpy tmplRemoveSpy(plugin_, &ReportDesignerPlugin::templateRemoved);
  QSignalSpy bindAddSpy(plugin_, &ReportDesignerPlugin::dataBindingAdded);
  QSignalSpy bindRemoveSpy(plugin_, &ReportDesignerPlugin::dataBindingRemoved);
  QSignalSpy previewSpy(plugin_, &ReportDesignerPlugin::previewUpdated);

  plugin_->addTemplate("SignalTemplate");
  QCOMPARE(tmplAddSpy.count(), 1);
  QCOMPARE(tmplAddSpy.at(0).at(0).toString(), QString("SignalTemplate"));

  plugin_->removeTemplate("SignalTemplate");
  QCOMPARE(tmplRemoveSpy.count(), 1);

  plugin_->addDataBinding("Test", "SignalField");
  QCOMPARE(bindAddSpy.count(), 1);

  plugin_->removeDataBinding("SignalField");
  QCOMPARE(bindRemoveSpy.count(), 1);

  plugin_->setPreviewText("preview");
  QCOMPARE(previewSpy.count(), 1);

  plugin_->setPreviewText("");
  plugin_->clearDataBindings();
}

QTEST_MAIN(TestReportDesignerPlugin)
#include "reportdesigner_plugin_test.moc"
