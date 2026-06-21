// ScriptLibraryPluginTest — Tests for ScriptLibraryPlugin
//
// Test coverage:
//   - Plugin identity, visibility, and widget creation
//   - Script tree, editor, output console, and doc viewer
//   - Add/remove/clear scripts and current script management
//   - Output append/clear and documentation HTML
//   - Signal emissions on script events

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QTreeWidget>
#include <QtTest/QtTest>

#include "plugins/scriptlibrary/ScriptLibraryPlugin.h"

class TestScriptLibraryPlugin : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();
  void cleanupTestCase();
  // Verify plugin identity fields
  void identity();
  // Verify widget is non-null
  void widgetNotNull();
  // Verify script tree has default items
  void scriptTree();
  // Verify script editor is non-null
  void scriptEditor();
  // Verify output console is non-null and read-only
  void outputConsole();
  // Verify documentation viewer is non-null and read-only
  void docViewer();
  // Test script add and remove operations
  void addAndRemoveScripts();
  // Test bulk script clear
  void clearScripts();
  // Test currentScript set/get round-trip
  void currentScript();
  // Test output clear and append operations
  void output();
  // Test documentation set/get
  void documentation();
  // Verify signal emissions for script operations
  void signalEmissions();

private:
  ScriptLibraryPlugin *plugin_ = nullptr;
};

void TestScriptLibraryPlugin::initTestCase() {
  plugin_ = new ScriptLibraryPlugin(this);
}

void TestScriptLibraryPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

void TestScriptLibraryPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("scriptlibrary"));
  QCOMPARE(plugin_->displayName(), QString("Script Library"));
  QCOMPARE(plugin_->displayNameZh(), QString("脚本库"));
  QCOMPARE(plugin_->defaultOrder(), 200);
  QVERIFY(plugin_->visible());
}

void TestScriptLibraryPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

void TestScriptLibraryPlugin::scriptTree() {
  QVERIFY(plugin_->scriptTree() != nullptr);
  QVERIFY(plugin_->scriptTree()->topLevelItemCount() >= 2);
}

void TestScriptLibraryPlugin::scriptEditor() {
  QVERIFY(plugin_->scriptEditor() != nullptr);
}

void TestScriptLibraryPlugin::outputConsole() {
  QVERIFY(plugin_->outputConsole() != nullptr);
  QVERIFY(plugin_->outputConsole()->isReadOnly());
}

void TestScriptLibraryPlugin::docViewer() {
  QVERIFY(plugin_->docViewer() != nullptr);
  QVERIFY(plugin_->docViewer()->isReadOnly());
}

void TestScriptLibraryPlugin::addAndRemoveScripts() {
  int initialCount = plugin_->scriptCount();

  plugin_->addScript("Custom", "TestScript1", "print('hello')");
  QCOMPARE(plugin_->scriptCount(), initialCount + 1);

  plugin_->addScript("Custom", "TestScript2", "print('world')");
  QCOMPARE(plugin_->scriptCount(), initialCount + 2);

  plugin_->removeScript("TestScript1");
  QCOMPARE(plugin_->scriptCount(), initialCount + 1);

  plugin_->removeScript("NonExistent");
  QCOMPARE(plugin_->scriptCount(), initialCount + 1);

  plugin_->removeScript("TestScript2");
  QCOMPARE(plugin_->scriptCount(), initialCount);
}

void TestScriptLibraryPlugin::clearScripts() {
  plugin_->addScript("TestCategory", "TempA", "");
  plugin_->addScript("TestCategory", "TempB", "");
  QVERIFY(plugin_->scriptCount() >= 2);

  plugin_->clearScripts();
  QCOMPARE(plugin_->scriptCount(), 0);
}

void TestScriptLibraryPlugin::currentScript() {
  plugin_->setCurrentScript("import ecat\nprint(ecat.scan())");
  QCOMPARE(plugin_->currentScript(), QString("import ecat\nprint(ecat.scan())"));

  plugin_->setCurrentScript("");
  QCOMPARE(plugin_->currentScript(), QString(""));
}

void TestScriptLibraryPlugin::output() {
  plugin_->clearOutput();
  QCOMPARE(plugin_->output(), QString(""));

  plugin_->appendOutput("Line 1");
  plugin_->appendOutput("Line 2");
  QVERIFY(plugin_->output().contains("Line 1"));
  QVERIFY(plugin_->output().contains("Line 2"));

  plugin_->clearOutput();
  QCOMPARE(plugin_->output(), QString(""));
}

void TestScriptLibraryPlugin::documentation() {
  plugin_->setDocumentation("<b>Read SDO</b><p>Reads an SDO value</p>");
  QVERIFY(plugin_->documentation().contains("Read SDO"));

  plugin_->setDocumentation("");
  QCOMPARE(plugin_->documentation(), QString(""));
}

void TestScriptLibraryPlugin::signalEmissions() {
  QSignalSpy selectSpy(plugin_, &ScriptLibraryPlugin::scriptSelected);
  QSignalSpy runSpy(plugin_, &ScriptLibraryPlugin::runRequested);
  QSignalSpy addSpy(plugin_, &ScriptLibraryPlugin::scriptAdded);
  QSignalSpy removeSpy(plugin_, &ScriptLibraryPlugin::scriptRemoved);

  plugin_->addScript("Signals", "SignalScript", "code");
  QCOMPARE(addSpy.count(), 1);
  QCOMPARE(addSpy.at(0).at(0).toString(), QString("SignalScript"));

  plugin_->removeScript("SignalScript");
  QCOMPARE(removeSpy.count(), 1);
}

QTEST_MAIN(TestScriptLibraryPlugin)
#include "scriptlibrary_plugin_test.moc"
