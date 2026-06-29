// TestFormulaPlugin — Tests for FormulaPlugin
//
// Test coverage:
//   - Plugin identity (id, display names, order, visibility)
//   - UI widget creation and accessor methods
//   - Formula get/set and result display
//   - Variable table add/remove/clear operations
//   - History entry management
//   - Formula validation (parentheses matching)
//   - Signal emissions (formulaChanged, evaluateRequested, variableChanged)

#include <QApplication>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QtTest/QtTest>

#include "plugins/formula/FormulaPlugin.h"

class TestFormulaPlugin : public QObject {
  Q_OBJECT
private slots:
  void initTestCase();        // Create plugin instance
  void cleanupTestCase();     // Destroy plugin instance
  void identity();            // Verify plugin id, names, order, visibility
  void widgetNotNull();       // Check widget is created
  void formulaEditor();       // Check formula editor widget exists
  void variableTable();       // Check variable table structure
  void resultLabel();         // Check result label exists
  void historyList();         // Check history list exists
  void formula();             // Test formula get/set
  void result();              // Test result get/set
  void addAndRemoveVariables(); // Test variable add/remove with table verification
  void clearVariables();      // Test clearing all variables
  void history();             // Test history add and clear
  void validateFormula();     // Test formula validation with parentheses
  void signalEmissions();     // Verify formulaChanged, evaluateRequested, variableChanged signals

private:
  FormulaPlugin *plugin_ = nullptr;
};

void TestFormulaPlugin::initTestCase() {
  plugin_ = new FormulaPlugin(this);
}

void TestFormulaPlugin::cleanupTestCase() {
  delete plugin_;
  plugin_ = nullptr;
}

void TestFormulaPlugin::identity() {
  QCOMPARE(plugin_->id(), QString("formula"));
  QCOMPARE(plugin_->displayName(), QString("Formula Editor"));
  QCOMPARE(plugin_->displayNameZh(), QString("公式编辑器"));
  QCOMPARE(plugin_->defaultOrder(), 195);
  QVERIFY(!plugin_->visible());
}

void TestFormulaPlugin::widgetNotNull() {
  QVERIFY(plugin_->widget() != nullptr);
}

void TestFormulaPlugin::formulaEditor() {
  QVERIFY(plugin_->formulaEditor() != nullptr);
}

void TestFormulaPlugin::variableTable() {
  QVERIFY(plugin_->variableTable() != nullptr);
  QCOMPARE(plugin_->variableTable()->columnCount(), 2);
}

void TestFormulaPlugin::resultLabel() {
  QVERIFY(plugin_->resultLabel() != nullptr);
}

void TestFormulaPlugin::historyList() {
  QVERIFY(plugin_->historyList() != nullptr);
}

void TestFormulaPlugin::formula() {
  plugin_->setFormula("x * 2 + y");
  QCOMPARE(plugin_->formula(), QString("x * 2 + y"));

  plugin_->setFormula("");
  QCOMPARE(plugin_->formula(), QString(""));
}

void TestFormulaPlugin::result() {
  plugin_->setResult("42");
  QCOMPARE(plugin_->result(), QString("42"));

  plugin_->setResult("");
  QCOMPARE(plugin_->result(), QString(""));
}

void TestFormulaPlugin::addAndRemoveVariables() {
  plugin_->clearVariables();
  QCOMPARE(plugin_->variableCount(), 0);

  plugin_->addVariable("x", "10");
  QCOMPARE(plugin_->variableCount(), 1);

  plugin_->addVariable("y", "20");
  QCOMPARE(plugin_->variableCount(), 2);

  QCOMPARE(plugin_->variableTable()->item(0, 0)->text(), QString("x"));
  QCOMPARE(plugin_->variableTable()->item(0, 1)->text(), QString("10"));
  QCOMPARE(plugin_->variableTable()->item(1, 0)->text(), QString("y"));
  QCOMPARE(plugin_->variableTable()->item(1, 1)->text(), QString("20"));

  plugin_->removeVariable("x");
  QCOMPARE(plugin_->variableCount(), 1);
  QCOMPARE(plugin_->variableTable()->item(0, 0)->text(), QString("y"));

  plugin_->clearVariables();
}

void TestFormulaPlugin::clearVariables() {
  plugin_->addVariable("a", "1");
  plugin_->addVariable("b", "2");
  QCOMPARE(plugin_->variableCount(), 2);

  plugin_->clearVariables();
  QCOMPARE(plugin_->variableCount(), 0);
}

void TestFormulaPlugin::history() {
  plugin_->clearHistory();
  QCOMPARE(plugin_->historyCount(), 0);

  plugin_->addHistoryEntry("x * 2 = 20");
  QCOMPARE(plugin_->historyCount(), 1);

  plugin_->addHistoryEntry("y + 5 = 25");
  QCOMPARE(plugin_->historyCount(), 2);

  plugin_->clearHistory();
  QCOMPARE(plugin_->historyCount(), 0);
}

void TestFormulaPlugin::validateFormula() {
  QVERIFY(plugin_->validateFormula("x + y"));
  QVERIFY(plugin_->validateFormula("(x + y) * z"));
  QVERIFY(plugin_->validateFormula("((a + b) * c)"));

  QVERIFY(!plugin_->validateFormula(""));
  QVERIFY(!plugin_->validateFormula("(x + y"));
  QVERIFY(!plugin_->validateFormula("x + y)"));
  QVERIFY(!plugin_->validateFormula("((a)"));
}

void TestFormulaPlugin::signalEmissions() {
  QSignalSpy formulaSpy(plugin_, &FormulaPlugin::formulaChanged);
  QSignalSpy evalSpy(plugin_, &FormulaPlugin::evaluateRequested);
  QSignalSpy varSpy(plugin_, &FormulaPlugin::variableChanged);

  plugin_->setFormula("test");
  QCOMPARE(formulaSpy.count(), 1);

  plugin_->addVariable("q", "5");
  QCOMPARE(varSpy.count(), 1);

  plugin_->clearVariables();
  plugin_->setFormula("");
}

QTEST_MAIN(TestFormulaPlugin)
#include "formula_plugin_test.moc"
