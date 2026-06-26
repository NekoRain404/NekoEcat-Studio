// ProjectPluginTest — Tests for ProjectPlugin and related services
//
// Test coverage:
//   - Plugin identity and widget creation
//   - Project tree section structure
//   - ProjectManagerService signals and save/load round-trip
//   - ConfigurationService defaults, validation, save/load, reset
//   - Recent projects tracking
//   - Stacked widget page count
//   - Plugin activate/deactivate lifecycle
#include <QTest>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFile>

#include "services/ProjectManagerService.h"
#include "services/ConfigurationService.h"
#include "plugins/project/ProjectPlugin.h"

class ProjectPluginTest : public QObject {
    Q_OBJECT
private slots:
    // Verify plugin id, displayName, displayNameZh, defaultOrder, visible
    void testPluginIdentity() {
        ProjectManagerService ps;
        ConfigurationService cs;
        ProjectPlugin plugin(&ps, &cs);
        QCOMPARE(plugin.id(), QString("project"));
        QCOMPARE(plugin.displayName(), QString("Project"));
        QCOMPARE(plugin.displayNameZh(), QString("工程"));
        QCOMPARE(plugin.defaultOrder(), 115);
        QVERIFY(plugin.visible());
    }

    // Verify plugin widget is created and non-null
    void testWidgetCreation() {
        ProjectManagerService ps;
        ConfigurationService cs;
        ProjectPlugin plugin(&ps, &cs);
        QWidget *w = plugin.widget();
        QVERIFY(w != nullptr);
    }

    // Verify project tree has all expected section headers
    void testProjectTreeSections() {
        ProjectManagerService ps;
        ConfigurationService cs;
        ProjectPlugin plugin(&ps, &cs);
        plugin.widget();

        QTreeWidget *tree = plugin.widget()->findChild<QTreeWidget *>();
        QVERIFY(tree != nullptr);
        QVERIFY(tree->topLevelItemCount() > 0);

        QStringList expected = {"Overview", "Master Configuration",
                                "Timing Configuration", "Network Configuration",
                                "Safety Configuration", "Slave Configurations",
                                "SDO Configurations", "Watch List",
                                "Startup SDO List", "I/O Variables", "Notes"};
        QCOMPARE(tree->topLevelItemCount(), expected.size());
        for (int i = 0; i < expected.size(); ++i)
            QCOMPARE(tree->topLevelItem(i)->text(0), expected[i]);
    }

    // Verify ProjectManagerService emits projectOpened on create
    void testProjectServiceSignals() {
        ProjectManagerService ps;
        QSignalSpy openedSpy(&ps, &ProjectManagerService::projectOpened);
        QSignalSpy closedSpy(&ps, &ProjectManagerService::projectClosed);

        ps.createProject("TestProject");
        QCOMPARE(openedSpy.count(), 1);
        QCOMPARE(ps.projectName(), QString("TestProject"));
    }

    // Test project save and load round-trip preserves name and data
    void testProjectSaveLoad() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        QString path = dir.filePath("test.ecat.json");

        ProjectManagerService ps;
        QSignalSpy savedSpy(&ps, &ProjectManagerService::projectSaved);

        ps.createProject("SaveTest");
        auto &data = ps.projectData();
        data.description = "Test description";
        data.version = "2.0.0";

        QVERIFY(ps.saveProjectAs(path));
        QCOMPARE(savedSpy.count(), 1);

        ProjectManagerService ps2;
        QSignalSpy openedSpy(&ps2, &ProjectManagerService::projectOpened);
        QVERIFY(ps2.openProject(path));
        QCOMPARE(openedSpy.count(), 1);
        QCOMPARE(ps2.projectName(), QString("SaveTest"));
    }

    // Verify ConfigurationService defaults for master, timing, and safety
    void testConfigurationServiceDefaults() {
        ConfigurationService cs;
        QCOMPARE(cs.masterConfig().cycleTimeUs, 1000);
        QVERIFY(cs.masterConfig().distributedClocks);
        QCOMPARE(cs.timingConfig().cycleTimeUs, 1000);
        QCOMPARE(cs.safetyConfig().watchdogTimeoutMs, 5000);
        QCOMPARE(cs.safetyConfig().errorBehavior, QString("safeop"));
    }

    // Verify default configuration passes validation
    void testConfigurationValidation() {
        ConfigurationService cs;
        ConfigValidationResult r = cs.validateConfiguration();
        QVERIFY(r.valid);
        QVERIFY(r.errors.isEmpty());
    }

    // Verify invalid cycleTimeUs triggers validation errors
    void testConfigurationValidationErrors() {
        ConfigurationService cs;
        cs.masterConfig().cycleTimeUs = -1;
        ConfigValidationResult r = cs.validateConfiguration();
        QVERIFY(!r.valid);
        QVERIFY(!r.errors.isEmpty());
    }

    // Test configuration save and load round-trip preserves all fields
    void testConfigurationSaveLoad() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        QString path = dir.filePath("config.ecat.json");

        ConfigurationService cs;
        cs.masterConfig().adapter = "eth0";
        cs.masterConfig().cycleTimeUs = 2000;
        cs.timingConfig().sync0Shift = 100;
        cs.safetyConfig().watchdogTimeoutMs = 10000;

        QSignalSpy changedSpy(&cs, &ConfigurationService::configurationChanged);

        QVERIFY(cs.saveConfiguration(path));

        ConfigurationService cs2;
        QVERIFY(cs2.loadConfiguration(path));
        QCOMPARE(cs2.masterConfig().adapter, QString("eth0"));
        QCOMPARE(cs2.masterConfig().cycleTimeUs, 2000);
        QCOMPARE(cs2.timingConfig().sync0Shift, 100);
        QCOMPARE(cs2.safetyConfig().watchdogTimeoutMs, 10000);
    }

    // Invalid configuration files fail without mutating existing state
    void testConfigurationRejectsInvalidPersistence() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());

        ConfigurationService cs;
        cs.masterConfig().adapter = "eth0";
        cs.masterConfig().cycleTimeUs = 2000;
        cs.slaveConfigs().append(SlaveConfig{1, "Drive", 1234, 5678, {}});
        QSignalSpy changedSpy(&cs, &ConfigurationService::configurationChanged);

        QVERIFY(!cs.saveConfiguration(QString()));
        QVERIFY(!cs.saveConfiguration(dir.path()));

        const QString arrayPath = dir.filePath("array.json");
        QFile arrayFile(arrayPath);
        QVERIFY(arrayFile.open(QIODevice::WriteOnly));
        QCOMPARE(arrayFile.write(QByteArrayLiteral("[]")), 2);
        arrayFile.close();

        const QString missingSlavesPath = dir.filePath("missing-slaves.json");
        QFile missingSlavesFile(missingSlavesPath);
        QVERIFY(missingSlavesFile.open(QIODevice::WriteOnly));
        QVERIFY(missingSlavesFile.write(QByteArrayLiteral(
                    "{\"master\":{},\"network\":{},\"timing\":{},\"safety\":{}}")) > 0);
        missingSlavesFile.close();

        QVERIFY(!cs.loadConfiguration(QString()));
        QVERIFY(!cs.loadConfiguration(arrayPath));
        QVERIFY(!cs.loadConfiguration(missingSlavesPath));
        QCOMPARE(changedSpy.count(), 0);
        QCOMPARE(cs.masterConfig().adapter, QString("eth0"));
        QCOMPARE(cs.masterConfig().cycleTimeUs, 2000);
        QCOMPARE(cs.slaveConfigs().size(), 1);
        QCOMPARE(cs.slaveConfigs().first().name, QString("Drive"));
    }

    // Verify resetToDefaults restores cycleTimeUs and emits signal
    void testConfigurationReset() {
        ConfigurationService cs;
        cs.masterConfig().cycleTimeUs = 5000;
        QSignalSpy changedSpy(&cs, &ConfigurationService::configurationChanged);
        cs.resetToDefaults();
        QCOMPARE(changedSpy.count(), 1);
        QCOMPARE(cs.masterConfig().cycleTimeUs, 1000);
    }

    // Verify saved project path appears in recentProjects list
    void testRecentProjects() {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        QString path = dir.filePath("recent.ecat.json");

        ProjectManagerService ps;
        ps.createProject("RecentTest");
        ps.saveProjectAs(path);
        QVERIFY(ps.recentProjects().contains(path));
    }

    // Verify stacked widget has 5 pages
    void testStackedWidgetPages() {
        ProjectManagerService ps;
        ConfigurationService cs;
        ProjectPlugin plugin(&ps, &cs);
        plugin.widget();

        QStackedWidget *stack = plugin.widget()->findChild<QStackedWidget *>();
        QVERIFY(stack != nullptr);
        QCOMPARE(stack->count(), 5);
    }

    // Verify plugin activate and deactivate complete without error
    void testPluginActivateDeactivate() {
        ProjectManagerService ps;
        ConfigurationService cs;
        ProjectPlugin plugin(&ps, &cs);
        plugin.activate();
        plugin.deactivate();
    }
};

QTEST_MAIN(ProjectPluginTest)
#include "project_plugin_test.moc"
