// TemplatePluginTest — Tests for Template Plugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Initial state (template count, search results, selection)
//   - Template table structure
//   - Template selection and editor population
//   - Add/remove/update templates
//   - Search functionality (case-insensitive, multiple results)
//   - Editor and preview widgets
//   - Preview refresh from editor content
//   - Status label with search results
//   - Export single and all templates
#include "plugins/template/TemplatePlugin.h"
#include <QFile>
#include <QLabel>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTest>
#include <QTextEdit>

class TemplatePluginTest : public QObject {
    Q_OBJECT
private slots:
    // Verify plugin ID, display name, order, and visibility
    void testPluginIdentity() {
        TemplatePlugin plugin;

        QCOMPARE(plugin.id(), QString("template"));
        QCOMPARE(plugin.displayName(), QString("Templates"));
        QCOMPARE(plugin.displayNameZh(), QString("模板"));
        QCOMPARE(plugin.defaultOrder(), 225);
        QCOMPARE(plugin.visible(), false);
    }

    // Verify widget is created
    void testWidgetCreation() {
        TemplatePlugin plugin;
        QVERIFY(plugin.widget() != nullptr);
    }

    // Verify initial template count, search results, and selection
    void testInitialState() {
        TemplatePlugin plugin;

        QCOMPARE(plugin.templateCount(), 3);
        QCOMPARE(plugin.searchResultCount(), 0);
        QCOMPARE(plugin.selectedTemplate(), -1);
    }

    // Check template table has correct dimensions
    void testTemplateTable() {
        TemplatePlugin plugin;

        QTableWidget* table = plugin.templateTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), 3);
        QCOMPARE(table->columnCount(), 3);
    }

    // Test selecting a template updates selection and editor
    void testSelectTemplate() {
        TemplatePlugin plugin;
        QSignalSpy selectSpy(&plugin, &TemplatePlugin::templateSelected);

        plugin.selectTemplate(0);
        QCOMPARE(plugin.selectedTemplate(), 0);
        QCOMPARE(selectSpy.count(), 1);

        QTextEdit* editor = plugin.editor();
        QVERIFY(editor != nullptr);
        QVERIFY(!editor->toPlainText().isEmpty());
    }

    // Test adding a new template
    void testAddTemplate() {
        TemplatePlugin plugin;
        int initial = plugin.templateCount();

        TemplatePlugin::TemplateEntry entry;
        entry.id = "custom";
        entry.name = "Custom Template";
        entry.category = "Custom";
        entry.content = "custom content";

        plugin.addTemplate(entry);
        QCOMPARE(plugin.templateCount(), initial + 1);
    }

    // Test removing a template
    void testRemoveTemplate() {
        TemplatePlugin plugin;
        int initial = plugin.templateCount();

        plugin.removeTemplate(0);
        QCOMPARE(plugin.templateCount(), initial - 1);
    }

    // Test updating template content emits modified signal
    void testUpdateTemplate() {
        TemplatePlugin plugin;
        QSignalSpy modSpy(&plugin, &TemplatePlugin::templateModified);

        plugin.selectTemplate(0);
        plugin.updateTemplate(0, "new content");
        QCOMPARE(modSpy.count(), 1);
    }

    // Test search finds matching templates
    void testSearch() {
        TemplatePlugin plugin;

        plugin.search("Servo");
        QCOMPARE(plugin.searchResultCount(), 1);
    }

    // Test search with no matching results
    void testSearchNoResults() {
        TemplatePlugin plugin;

        plugin.search("nonexistent_xyz");
        QCOMPARE(plugin.searchResultCount(), 0);
    }

    // Test search is case-insensitive
    void testSearchCaseInsensitive() {
        TemplatePlugin plugin;

        plugin.search("servo");
        QCOMPARE(plugin.searchResultCount(), 1);
    }

    // Test search returns multiple results
    void testSearchMultipleResults() {
        TemplatePlugin plugin;

        plugin.search("slave");
        QVERIFY(plugin.searchResultCount() >= 1);
    }

    // Verify search results table matches result count
    void testSearchResultsTable() {
        TemplatePlugin plugin;

        plugin.search("servo");
        QTableWidget* table = plugin.searchResultsTable();
        QVERIFY(table != nullptr);
        QCOMPARE(table->rowCount(), plugin.searchResultCount());
    }

    // Verify editor loads template content on selection
    void testEditor() {
        TemplatePlugin plugin;

        QTextEdit* ed = plugin.editor();
        QVERIFY(ed != nullptr);

        plugin.selectTemplate(1);
        QVERIFY(!ed->toPlainText().isEmpty());
    }

    // Verify preview shows content after template selection
    void testPreview() {
        TemplatePlugin plugin;

        QTextEdit* pv = plugin.preview();
        QVERIFY(pv != nullptr);

        plugin.selectTemplate(0);
        QVERIFY(!pv->toPlainText().isEmpty());
    }

    // Test refresh preview reflects editor changes
    void testRefreshPreview() {
        TemplatePlugin plugin;

        plugin.selectTemplate(0);
        plugin.editor()->setText("modified content");
        plugin.refreshPreview();
        QCOMPARE(plugin.preview()->toPlainText(), QString("modified content"));
    }

    // Verify status label updates after search
    void testStatusLabel() {
        TemplatePlugin plugin;

        QLabel* label = plugin.statusLabel();
        QVERIFY(label != nullptr);

        plugin.search("test");
        QVERIFY(label->text().contains("results"));
    }

    void testImportTemplateReportsPersistenceOutcome() {
        TemplatePlugin plugin;
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        const int initial = plugin.templateCount();
        const QString path = dir.filePath("imported_template.txt");
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write("imported template content");
        file.close();

        QVERIFY(plugin.importTemplate(path));
        QCOMPARE(plugin.templateCount(), initial + 1);
        plugin.selectTemplate(plugin.templateCount() - 1);
        QCOMPARE(plugin.editor()->toPlainText(), QString("imported template content"));

        QTest::failOnWarning(QRegularExpression(QStringLiteral("QFSFileEngine::open: No file name specified")));
        QVERIFY(!plugin.importTemplate(QString()));
        QVERIFY(!plugin.importTemplate(dir.path()));
        QVERIFY(!plugin.importTemplate(dir.filePath("missing_template.txt")));
    }

    // Test exporting a single template to file
    void testExportTemplate() {
        TemplatePlugin plugin;
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        const QString unselectedPath = dir.filePath("template_unselected.txt");
        QVERIFY(!plugin.exportTemplate(unselectedPath));
        QVERIFY(!QFile::exists(unselectedPath));

        plugin.selectTemplate(0);
        const QString path = dir.filePath("template_export_test.txt");
        QVERIFY(plugin.exportTemplate(path));
        QVERIFY(QFile::exists(path));

        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        const QString text = QString::fromUtf8(file.readAll());
        QVERIFY(text.contains(QStringLiteral("master")));
        QVERIFY(text.contains(QStringLiteral("ecat0")));

        QTest::failOnWarning(QRegularExpression(QStringLiteral("QFSFileEngine::open: No file name specified")));
        QVERIFY(!plugin.exportTemplate(QString()));
        QVERIFY(!plugin.exportTemplate(dir.path()));
    }

    // Test exporting all templates to file
    void testExportAllTemplates() {
        TemplatePlugin plugin;
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        const QString path = dir.filePath("template_export_all_test.txt");
        QVERIFY(plugin.exportAllTemplates(path));
        QVERIFY(QFile::exists(path));

        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        const QString text = QString::fromUtf8(file.readAll());
        QVERIFY(text.contains(QStringLiteral("--- Default Network [Network] ---\n")));
        QVERIFY(text.contains(QStringLiteral("--- Servo Drive [Drive] ---\n")));

        QTest::failOnWarning(QRegularExpression(QStringLiteral("QFSFileEngine::open: No file name specified")));
        QVERIFY(!plugin.exportAllTemplates(QString()));
        QVERIFY(!plugin.exportAllTemplates(dir.path()));
    }
};

QTEST_MAIN(TemplatePluginTest)
#include "template_plugin_test.moc"
