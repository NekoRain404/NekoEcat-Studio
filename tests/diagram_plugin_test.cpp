// TestDiagramPlugin — Tests for DiagramPlugin
//
// Test coverage:
//   - Plugin identity and visibility
//   - Canvas, shape library, and property editor
//   - Add, remove, and clear shapes
//   - Zoom level control
//   - Property text get/set
//   - Export/import JSON round-trip
//   - Signal emissions (shapeAdded, shapeRemoved, zoomChanged)

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QtTest/QtTest>

#include "plugins/diagram/DiagramPlugin.h"

class TestDiagramPlugin : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();
  void cleanupTestCase();
  // Verify plugin id, display names, order, and visibility
  void identity();
  // Verify main widget is created
  void widgetNotNull();
  // Verify canvas widget exists with minimum width
  void canvas();
  // Verify shape library widget exists
  void shapeLibrary();
  // Verify property editor widget exists
  void propertyEditor();
  // Verify adding and removing shapes updates count correctly
  void addAndRemoveShapes();
  // Verify clearShapes resets count to zero
  void clearShapes();
  // Verify zoom level get/set
  void zoom();
  // Verify property text get/set
  void propertyText();
  // Verify export/import preserves shapes, zoom, and properties
  void exportImport();
  // Verify shapeAdded, shapeRemoved, and zoomChanged signals
  void signalEmissions();

private:
  DiagramPlugin *plugin_ = nullptr;
};

void TestDiagramPlugin::initTestCase() {
  plugin_ = new DiagramPlugin(this);
}

void TestDiagramPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

// Verify plugin id, display names, order, and visibility
void TestDiagramPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("diagram"));
  QCOMPARE(plugin_->displayName(), QString("Diagram Editor"));
  QCOMPARE(plugin_->displayNameZh(), QString("图表编辑器"));
  QCOMPARE(plugin_->defaultOrder(), 190);
  QVERIFY(plugin_->visible());
}

// Verify main widget is created
void TestDiagramPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

// Verify canvas exists and has minimum width
void TestDiagramPlugin::canvas() {
  QVERIFY(plugin_->canvas() != nullptr);
  QVERIFY(plugin_->canvas()->minimumWidth() >= 400);
}

// Verify shape library widget is created
void TestDiagramPlugin::shapeLibrary() {
  QVERIFY(plugin_->shapeLibrary() != nullptr);
}

// Verify property editor widget is created
void TestDiagramPlugin::propertyEditor() {
  QVERIFY(plugin_->propertyEditor() != nullptr);
}

// Verify add, remove, and clear shape operations
void TestDiagramPlugin::addAndRemoveShapes() {
  plugin_->clearShapes();
  QCOMPARE(plugin_->shapeCount(), 0);

  plugin_->addShape("Basic", "Rectangle");
  QCOMPARE(plugin_->shapeCount(), 1);

  plugin_->addShape("Basic", "Circle");
  QCOMPARE(plugin_->shapeCount(), 2);

  plugin_->addShape("EtherCAT", "Slave Node");
  QCOMPARE(plugin_->shapeCount(), 3);

  plugin_->removeShape("Circle");
  QCOMPARE(plugin_->shapeCount(), 2);

  plugin_->removeShape("NonExistent");
  QCOMPARE(plugin_->shapeCount(), 2);

  plugin_->clearShapes();
}

// Verify clearShapes resets count to zero
void TestDiagramPlugin::clearShapes() {
  plugin_->addShape("Basic", "A");
  plugin_->addShape("Basic", "B");
  QCOMPARE(plugin_->shapeCount(), 2);

  plugin_->clearShapes();
  QCOMPARE(plugin_->shapeCount(), 0);
}

// Verify zoom level get/set
void TestDiagramPlugin::zoom() {
  plugin_->setZoom(1.0);
  QCOMPARE(plugin_->zoom(), 1.0);

  plugin_->setZoom(2.5);
  QCOMPARE(plugin_->zoom(), 2.5);
}

// Verify property text get/set
void TestDiagramPlugin::propertyText() {
  plugin_->setPropertyText("width=100\nheight=50");
  QCOMPARE(plugin_->propertyText(), QString("width=100\nheight=50"));

  plugin_->setPropertyText("");
  QCOMPARE(plugin_->propertyText(), QString(""));
}

// Verify JSON export/import round-trip preserves state
void TestDiagramPlugin::exportImport() {
  plugin_->clearShapes();
  plugin_->addShape("Test", "ShapeA");
  plugin_->setPropertyText("test properties");
  plugin_->setZoom(1.5);

  QString tmpPath = QDir::tempPath() + "/diagram_test_export.json";
  QVERIFY(plugin_->exportToJson(tmpPath));

  plugin_->clearShapes();
  plugin_->setPropertyText("");
  plugin_->setZoom(1.0);

  QVERIFY(plugin_->importFromJson(tmpPath));
  QCOMPARE(plugin_->zoom(), 1.5);
  QCOMPARE(plugin_->shapeCount(), 1);
  QCOMPARE(plugin_->propertyText(), QString("test properties"));

  QFile::remove(tmpPath);
  plugin_->clearShapes();
}

// Verify shapeAdded, shapeRemoved, and zoomChanged signals
void TestDiagramPlugin::signalEmissions() {
  QSignalSpy addSpy(plugin_, &DiagramPlugin::shapeAdded);
  QSignalSpy removeSpy(plugin_, &DiagramPlugin::shapeRemoved);
  QSignalSpy zoomSpy(plugin_, &DiagramPlugin::zoomChanged);

  plugin_->addShape("Test", "SignalShape");
  QCOMPARE(addSpy.count(), 1);
  QCOMPARE(addSpy.at(0).at(0).toString(), QString("SignalShape"));

  plugin_->removeShape("SignalShape");
  QCOMPARE(removeSpy.count(), 1);

  plugin_->setZoom(3.0);
  QCOMPARE(zoomSpy.count(), 1);

  plugin_->clearShapes();
  plugin_->setZoom(1.0);
}

QTEST_MAIN(TestDiagramPlugin)
#include "diagram_plugin_test.moc"
