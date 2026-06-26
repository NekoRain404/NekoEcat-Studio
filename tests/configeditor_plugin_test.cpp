// ConfigurationEditorPluginTest — Tests for ConfigurationEditorPlugin
//
// Test coverage:
//   - Plugin identity and metadata
//   - Widget creation
//   - Config tree and selection
//   - Add/remove/update config entries
//   - Config validation
//   - Import/export functionality

#include <QFile>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QTest>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QLabel>
#include "plugins/configeditor/ConfigurationEditorPlugin.h"

class ConfigurationEditorPluginTest : public QObject {
  Q_OBJECT
private slots:
  // Verify plugin id, display names, and default order
  void testPluginIdentity() {
    ConfigurationEditorPlugin plugin;
    QCOMPARE(plugin.id(), QString("configeditor"));
    QCOMPARE(plugin.displayName(), QString("Configuration Editor"));
    QCOMPARE(plugin.displayNameZh(), QString("配置编辑器"));
    QCOMPARE(plugin.defaultOrder(), 265);
    QCOMPARE(plugin.visible(), true);
  }

  // Verify widget is created
  void testWidgetCreation() {
    ConfigurationEditorPlugin plugin;
    QVERIFY(plugin.widget() != nullptr);
  }

  // Verify initial config count and selection state
  void testInitialState() {
    ConfigurationEditorPlugin plugin;
    QCOMPARE(plugin.configCount(), 6);
    QCOMPARE(plugin.selectedConfig(), -1);
    QCOMPARE(plugin.errorCount(), 0);
  }

  // Verify config tree widget exists with items
  void testConfigTree() {
    ConfigurationEditorPlugin plugin;
    QTreeWidget *tree = plugin.configTree();
    QVERIFY(tree != nullptr);
    QVERIFY(tree->topLevelItemCount() > 0);
  }

  // Verify select config updates selection and editor
  void testSelectConfig() {
    ConfigurationEditorPlugin plugin;
    QSignalSpy selectSpy(&plugin, &ConfigurationEditorPlugin::configSelected);

    plugin.selectConfig(0);
    QCOMPARE(plugin.selectedConfig(), 0);
    QCOMPARE(selectSpy.count(), 1);

    QTextEdit *editor = plugin.configEditor();
    QVERIFY(editor != nullptr);
    QVERIFY(!editor->toPlainText().isEmpty());
  }

  // Verify adding a config entry increments count
  void testAddConfig() {
    ConfigurationEditorPlugin plugin;
    int initial = plugin.configCount();

    ConfigurationEditorPlugin::ConfigEntry entry;
    entry.category = "Test";
    entry.key = "test.key";
    entry.value = "test_value";
    entry.description = "Test entry";

    plugin.addConfig(entry);
    QCOMPARE(plugin.configCount(), initial + 1);
  }

  // Verify removing a config entry decrements count
  void testRemoveConfig() {
    ConfigurationEditorPlugin plugin;
    int initial = plugin.configCount();

    plugin.removeConfig(0);
    QCOMPARE(plugin.configCount(), initial - 1);
  }

  // Verify updating config emits change signal
  void testUpdateConfig() {
    ConfigurationEditorPlugin plugin;
    QSignalSpy changeSpy(&plugin, &ConfigurationEditorPlugin::configChanged);

    plugin.updateConfig(0, "new_value");
    QCOMPARE(changeSpy.count(), 1);
  }

  // Verify validate passes with valid config
  void testValidate() {
    ConfigurationEditorPlugin plugin;
    QSignalSpy validSpy(&plugin, &ConfigurationEditorPlugin::validationCompleted);

    plugin.validate();
    QCOMPARE(validSpy.count(), 1);
    QCOMPARE(plugin.errorCount(), 0);
  }

  // Verify validate detects empty value
  void testValidateEmptyValue() {
    ConfigurationEditorPlugin plugin;

    ConfigurationEditorPlugin::ConfigEntry entry;
    entry.category = "Test";
    entry.key = "empty.key";
    entry.value = "";
    entry.description = "Empty value";
    plugin.addConfig(entry);

    plugin.validate();
    QVERIFY(plugin.errorCount() > 0);
  }

  // Verify validation table structure
  void testValidationTable() {
    ConfigurationEditorPlugin plugin;
    QTableWidget *table = plugin.validationTable();
    QVERIFY(table != nullptr);
    QCOMPARE(table->columnCount(), 2);
  }

  // Verify preview shows config content
  void testPreview() {
    ConfigurationEditorPlugin plugin;
    QTextEdit *pv = plugin.configPreview();
    QVERIFY(pv != nullptr);

    plugin.selectConfig(0);
    QVERIFY(!pv->toPlainText().isEmpty());
  }

  // Verify refresh preview updates content
  void testRefreshPreview() {
    ConfigurationEditorPlugin plugin;
    plugin.refreshPreview();
    QVERIFY(!plugin.configPreview()->toPlainText().isEmpty());
  }

  // Verify status label updates on validation
  void testStatusLabel() {
    ConfigurationEditorPlugin plugin;
    QLabel *label = plugin.statusLabel();
    QVERIFY(label != nullptr);

    plugin.validate();
    QVERIFY(label->text().contains("Validation"));
  }

  // Verify export config creates file
  void testExportConfig() {
    ConfigurationEditorPlugin plugin;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = dir.filePath("config_export_test.txt");
    QVERIFY(plugin.exportConfig(path));
    QVERIFY(QFile::exists(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString text = QString::fromUtf8(file.readAll());
    QVERIFY(text.contains(QStringLiteral("Network.master.name = ecat0\n")));
    QVERIFY(text.contains(QStringLiteral("Timing.dc.sync0_cycle = 1000\n")));

    QTest::failOnWarning(QRegularExpression(
        QStringLiteral("QFSFileEngine::open: No file name specified")));
    QVERIFY(!plugin.exportConfig(QString()));
    QVERIFY(!plugin.exportConfig(dir.path()));
  }

  // Verify import config adds entries
  void testImportConfig() {
    ConfigurationEditorPlugin plugin;
    int initial = plugin.configCount();

    QString path = QDir::temp().absoluteFilePath("config_import_test.txt");
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&f);
      out << "imported.key = imported_value\n";
    }
    plugin.importConfig(path);
    QVERIFY(plugin.configCount() > initial);
    QFile::remove(path);
  }
};

QTEST_MAIN(ConfigurationEditorPluginTest)
#include "configeditor_plugin_test.moc"
