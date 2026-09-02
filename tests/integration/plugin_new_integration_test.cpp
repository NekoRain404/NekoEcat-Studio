// PluginNewIntegrationTest — Tests for Diagram, Formula, and ScriptLibrary plugins
//
// Test coverage:
//   - Plugin identity (id, displayName, displayNameZh, defaultOrder)
//   - Widget creation and sub-widget availability
//   - Shape add/remove/clear and zoom control (DiagramPlugin)
//   - Property text and JSON export/import round-trip (DiagramPlugin)
//   - Variable add/remove/clear and history management (FormulaPlugin)
//   - Formula validation with bracket matching (FormulaPlugin)
//   - Script tree defaults, add/remove, output, documentation, editor (ScriptLibraryPlugin)
// PluginNewIntegrationTest — Tests for DiagramPlugin, FormulaPlugin, and ScriptLibraryPlugin
//
// Test coverage:
//   - DiagramPlugin: identity, widget, shapes CRUD, zoom, property text, export/import
//   - FormulaPlugin: identity, widget, variables, history, validation
//   - ScriptLibraryPlugin: identity, widget, defaults, add/remove scripts, output, docs, editor

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTest>
#include <QTextEdit>
#include <QTreeWidget>

#include "plugins/diagram/DiagramPlugin.h"
#include "plugins/formula/FormulaPlugin.h"
#include "plugins/scriptlibrary/ScriptLibraryPlugin.h"

class PluginNewIntegrationTest : public QObject {
    Q_OBJECT
private slots:
    // Verify DiagramPlugin identity
    void testDiagramIdentity() {
        DiagramPlugin p;
        QCOMPARE(p.id(), QString("diagram"));
        QCOMPARE(p.displayName(), QString("Diagram Editor"));
        QCOMPARE(p.displayNameZh(), QString("图表编辑器"));
        QCOMPARE(p.defaultOrder(), 190);
    }
    // Verify DiagramPlugin widget, canvas, shapeLibrary, propertyEditor are non-null
    // Verify DiagramPlugin widget and sub-widgets
    void testDiagramWidget() {
        DiagramPlugin p;
        QVERIFY(p.widget() != nullptr);
        QVERIFY(p.canvas() != nullptr);
        QVERIFY(p.shapeLibrary() != nullptr);
        QVERIFY(p.propertyEditor() != nullptr);
    }
    // Test shape add, remove, and clear operations on DiagramPlugin
    // Test DiagramPlugin shapes add/remove/clear
    void testDiagramShapes() {
        DiagramPlugin p;
        QCOMPARE(p.shapeCount(), 0);
        p.addShape("Basic", "Rectangle");
        QCOMPARE(p.shapeCount(), 1);
        p.addShape("Basic", "Circle");
        QCOMPARE(p.shapeCount(), 2);
        p.removeShape("Rectangle");
        QCOMPARE(p.shapeCount(), 1);
        p.clearShapes();
        QCOMPARE(p.shapeCount(), 0);
    }
    // Test zoom get/set round-trip on DiagramPlugin
    // Test DiagramPlugin zoom control
    void testDiagramZoom() {
        DiagramPlugin p;
        QCOMPARE(p.zoom(), 1.0);
        p.setZoom(2.0);
        QCOMPARE(p.zoom(), 2.0);
    }
    // Test property text get/set on DiagramPlugin
    // Test DiagramPlugin property text
    void testDiagramPropertyText() {
        DiagramPlugin p;
        p.setPropertyText("width=100");
        QCOMPARE(p.propertyText(), QString("width=100"));
    }
    // Test JSON export/import round-trip preserves shapes, zoom, and property text
    // Test DiagramPlugin JSON export and import round-trip
    void testDiagramExportImport() {
        DiagramPlugin p;
        p.addShape("Test", "Shape1");
        p.setZoom(1.5);
        p.setPropertyText("test props");

        QString tmp = QDir::tempPath() + "/integration_diagram_test.json";
        QVERIFY(p.exportToJson(tmp));

        p.clearShapes();
        p.setZoom(1.0);
        p.setPropertyText("");

        QVERIFY(p.importFromJson(tmp));
        QCOMPARE(p.zoom(), 1.5);
        QCOMPARE(p.shapeCount(), 1);
        QCOMPARE(p.propertyText(), QString("test props"));

        QFile::remove(tmp);
    }

    // Verify FormulaPlugin reports correct id, displayName, displayNameZh, defaultOrder
    // Verify FormulaPlugin identity
    void testFormulaIdentity() {
        FormulaPlugin p;
        QCOMPARE(p.id(), QString("formula"));
        QCOMPARE(p.displayName(), QString("Formula Editor"));
        QCOMPARE(p.displayNameZh(), QString("公式编辑器"));
        QCOMPARE(p.defaultOrder(), 195);
    }
    // Verify FormulaPlugin widget and sub-widgets are non-null
    // Verify FormulaPlugin widget and sub-widgets
    void testFormulaWidget() {
        FormulaPlugin p;
        QVERIFY(p.widget() != nullptr);
        QVERIFY(p.formulaEditor() != nullptr);
        QVERIFY(p.variableTable() != nullptr);
        QVERIFY(p.resultLabel() != nullptr);
        QVERIFY(p.historyList() != nullptr);
    }
    // Test variable add, remove, and clear on FormulaPlugin
    // Test FormulaPlugin variable add/remove/clear
    void testFormulaVariables() {
        FormulaPlugin p;
        QCOMPARE(p.variableCount(), 0);
        p.addVariable("x", "10");
        p.addVariable("y", "20");
        QCOMPARE(p.variableCount(), 2);
        p.removeVariable("x");
        QCOMPARE(p.variableCount(), 1);
        p.clearVariables();
        QCOMPARE(p.variableCount(), 0);
    }
    // Test history entry add and clear on FormulaPlugin
    // Test FormulaPlugin history management
    void testFormulaHistory() {
        FormulaPlugin p;
        QCOMPARE(p.historyCount(), 0);
        p.addHistoryEntry("test = 42");
        QCOMPARE(p.historyCount(), 1);
        p.clearHistory();
        QCOMPARE(p.historyCount(), 0);
    }
    // Test formula validation with valid, empty, and unbalanced bracket inputs
    // Test FormulaPlugin formula validation
    void testFormulaValidation() {
        FormulaPlugin p;
        QVERIFY(p.validateFormula("x + y"));
        QVERIFY(p.validateFormula("(a + b)"));
        QVERIFY(!p.validateFormula(""));
        QVERIFY(!p.validateFormula("(x"));
        QVERIFY(!p.validateFormula("x)"));
    }

    // Verify ScriptLibraryPlugin reports correct id, displayName, displayNameZh, defaultOrder
    // Verify ScriptLibraryPlugin identity
    void testScriptLibraryIdentity() {
        ScriptLibraryPlugin p;
        QCOMPARE(p.id(), QString("scriptlibrary"));
        QCOMPARE(p.displayName(), QString("Script Library"));
        QCOMPARE(p.displayNameZh(), QString("脚本库"));
        QCOMPARE(p.defaultOrder(), 200);
    }
    // Verify ScriptLibraryPlugin widget and sub-widgets are non-null
    // Verify ScriptLibraryPlugin widget and sub-widgets
    void testScriptLibraryWidget() {
        ScriptLibraryPlugin p;
        QVERIFY(p.widget() != nullptr);
        QVERIFY(p.scriptTree() != nullptr);
        QVERIFY(p.scriptEditor() != nullptr);
        QVERIFY(p.outputConsole() != nullptr);
        QVERIFY(p.docViewer() != nullptr);
    }
    // Verify ScriptLibraryPlugin tree has at least 2 default top-level items
    // Verify default script tree has built-in entries
    void testScriptLibraryDefaults() {
        ScriptLibraryPlugin p;
        QVERIFY(p.scriptTree()->topLevelItemCount() >= 2);
    }
    // Test script add and remove increments/decrements scriptCount correctly
    // Test adding and removing custom scripts
    void testScriptLibraryAddRemove() {
        ScriptLibraryPlugin p;
        int initial = p.scriptCount();
        p.addScript("Custom", "TestScript", "code");
        QCOMPARE(p.scriptCount(), initial + 1);
        p.removeScript("TestScript");
        QCOMPARE(p.scriptCount(), initial);
    }
    // Verify removing a nonexistent script does not change scriptCount
    // Verify removing nonexistent script is a no-op
    void testScriptLibraryRemoveInvalid() {
        ScriptLibraryPlugin p;
        int before = p.scriptCount();
        p.removeScript("nonexistent");
        QCOMPARE(p.scriptCount(), before);
    }
    // Test output clear and appendOutput on ScriptLibraryPlugin
    // Test output console append and clear
    void testScriptLibraryOutput() {
        ScriptLibraryPlugin p;
        p.clearOutput();
        QCOMPARE(p.output(), QString(""));
        p.appendOutput("hello");
        QVERIFY(p.output().contains("hello"));
    }
    // Test documentation set/get on ScriptLibraryPlugin
    // Test documentation viewer content
    void testScriptLibraryDocumentation() {
        ScriptLibraryPlugin p;
        p.setDocumentation("<b>Test</b>");
        QVERIFY(p.documentation().contains("Test"));
    }
    // Test currentScript set/get round-trip on ScriptLibraryPlugin
    // Test script editor get/set
    void testScriptLibraryEditor() {
        ScriptLibraryPlugin p;
        p.setCurrentScript("test code");
        QCOMPARE(p.currentScript(), QString("test code"));
    }
};

QTEST_MAIN(PluginNewIntegrationTest)
#include "plugin_new_integration_test.moc"
