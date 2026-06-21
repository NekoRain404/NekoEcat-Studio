// TestVisualizationStudioPlugin — Tests for Visualization Studio Plugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Canvas, data sources, chart types, export options widgets
//   - Add/remove/clear data sources
//   - Add/remove/clear chart types
//   - Preview text management
//   - Export/import functionality
//   - Signal emissions for data source, chart type, and preview events

#include <QApplication>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QTreeWidget>
#include <QtTest/QtTest>

#include "plugins/visualizationstudio/VisualizationStudioPlugin.h"

class TestVisualizationStudioPlugin : public QObject {
  Q_OBJECT
private slots:
  // Setup: create plugin instance
  // Set up plugin instance
  void initTestCase();
  // Clean up plugin instance
  void cleanupTestCase();
  // Verify plugin id, display names, order, and visibility
  void identity();
  // Widget is created successfully
  void widgetNotNull();
  // Canvas widget is valid and meets minimum width
  void canvas();
  // Data sources widget exists
  void dataSources();
  // Chart types widget exists
  void chartTypes();
  // Export options widget exists
  void exportOptions();
  // Add and remove data sources, verify count changes
  void addAndRemoveDataSources();
  // Clear all data sources and verify empty state
  void clearDataSources();
  // Add and remove chart types, verify count changes
  void addAndRemoveChartTypes();
  // Clear all chart types and verify empty state
  void clearChartTypes();
  // Set and retrieve preview text content
  void previewText();
  // Export visualization and import config, verify round-trip
  void exportImport();
  // Verify signals fire for data source, chart type, and preview changes
  void signalEmissions();

private:
  VisualizationStudioPlugin *plugin_ = nullptr;
};

void TestVisualizationStudioPlugin::initTestCase() {
  plugin_ = new VisualizationStudioPlugin(this);
}

void TestVisualizationStudioPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

void TestVisualizationStudioPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("visualizationstudio"));
  QCOMPARE(plugin_->displayName(), QString("Visualization Studio"));
  QCOMPARE(plugin_->displayNameZh(), QString("可视化工作室"));
  QCOMPARE(plugin_->defaultOrder(), 340);
  QVERIFY(plugin_->visible());
}

void TestVisualizationStudioPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

void TestVisualizationStudioPlugin::canvas() {
  QVERIFY(plugin_->canvas() != nullptr);
  QVERIFY(plugin_->canvas()->minimumWidth() >= 400);
}

void TestVisualizationStudioPlugin::dataSources() {
  QVERIFY(plugin_->dataSources() != nullptr);
}

void TestVisualizationStudioPlugin::chartTypes() {
  QVERIFY(plugin_->chartTypes() != nullptr);
}

void TestVisualizationStudioPlugin::exportOptions() {
  QVERIFY(plugin_->exportOptions() != nullptr);
}

void TestVisualizationStudioPlugin::addAndRemoveDataSources() {
  plugin_->clearDataSources();
  QCOMPARE(plugin_->dataSourceCount(), 0);

  plugin_->addDataSource("EtherCAT Bus Data");
  QCOMPARE(plugin_->dataSourceCount(), 1);

  plugin_->addDataSource("Slave Diagnostics");
  QCOMPARE(plugin_->dataSourceCount(), 2);

  plugin_->removeDataSource("EtherCAT Bus Data");
  QCOMPARE(plugin_->dataSourceCount(), 1);

  plugin_->removeDataSource("NonExistent");
  QCOMPARE(plugin_->dataSourceCount(), 1);

  plugin_->clearDataSources();
}

void TestVisualizationStudioPlugin::clearDataSources() {
  plugin_->addDataSource("A");
  plugin_->addDataSource("B");
  QCOMPARE(plugin_->dataSourceCount(), 2);

  plugin_->clearDataSources();
  QCOMPARE(plugin_->dataSourceCount(), 0);
}

void TestVisualizationStudioPlugin::addAndRemoveChartTypes() {
  int initial = plugin_->chartTypeCount();

  plugin_->addChartType("Custom", "Heat Map");
  QCOMPARE(plugin_->chartTypeCount(), initial + 1);

  plugin_->removeChartType("Heat Map");
  QCOMPARE(plugin_->chartTypeCount(), initial);
}

void TestVisualizationStudioPlugin::clearChartTypes() {
  plugin_->addChartType("Test", "A");
  plugin_->addChartType("Test", "B");
  QVERIFY(plugin_->chartTypeCount() >= 2);

  plugin_->clearChartTypes();
  QCOMPARE(plugin_->chartTypeCount(), 0);
}

void TestVisualizationStudioPlugin::previewText() {
  plugin_->setPreviewText("chart preview data");
  QCOMPARE(plugin_->previewText(), QString("chart preview data"));

  plugin_->setPreviewText("");
  QCOMPARE(plugin_->previewText(), QString(""));
}

void TestVisualizationStudioPlugin::exportImport() {
  plugin_->clearDataSources();
  plugin_->addDataSource("TestSource");
  plugin_->setPreviewText("test preview");

  QString tmpPath = QDir::tempPath() + "/viz_studio_test_export.json";
  QVERIFY(plugin_->exportVisualization(tmpPath, "PNG"));

  plugin_->clearDataSources();
  plugin_->setPreviewText("");

  QVERIFY(plugin_->importConfig(tmpPath));
  QCOMPARE(plugin_->dataSourceCount(), 1);
  QCOMPARE(plugin_->previewText(), QString("test preview"));

  QFile::remove(tmpPath);
  plugin_->clearDataSources();
}

void TestVisualizationStudioPlugin::signalEmissions() {
  QSignalSpy dsAddSpy(plugin_, &VisualizationStudioPlugin::dataSourceAdded);
  QSignalSpy dsRemoveSpy(plugin_, &VisualizationStudioPlugin::dataSourceRemoved);
  QSignalSpy chartAddSpy(plugin_, &VisualizationStudioPlugin::chartTypeAdded);
  QSignalSpy chartRemoveSpy(plugin_, &VisualizationStudioPlugin::chartTypeRemoved);
  QSignalSpy previewSpy(plugin_, &VisualizationStudioPlugin::previewUpdated);

  plugin_->addDataSource("SignalSource");
  QCOMPARE(dsAddSpy.count(), 1);
  QCOMPARE(dsAddSpy.at(0).at(0).toString(), QString("SignalSource"));

  plugin_->removeDataSource("SignalSource");
  QCOMPARE(dsRemoveSpy.count(), 1);

  plugin_->addChartType("Test", "SignalChart");
  QCOMPARE(chartAddSpy.count(), 1);

  plugin_->removeChartType("SignalChart");
  QCOMPARE(chartRemoveSpy.count(), 1);

  plugin_->setPreviewText("preview");
  QCOMPARE(previewSpy.count(), 1);

  plugin_->clearDataSources();
  plugin_->setPreviewText("");
}

QTEST_MAIN(TestVisualizationStudioPlugin)
#include "visualizationstudio_plugin_test.moc"
