#include <QTest>
#include <QJsonObject>
#include <QJsonArray>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

#include "services/ProjectManagerService.h"

class ProjectPersistenceTest : public QObject {
    Q_OBJECT
private slots:
    void projectDataDefaults();
    void projectDataJsonRoundtrip();
    void projectDataChecksum();
    void projectDataMigration();
    void serviceCreateProject();
    void serviceSaveAndLoad();
    void serviceSaveAs();
    void serviceValidateProject();
    void serviceCorruptChecksum();
    void serviceMigrationOnLoad();
    void serviceDashboardConfigs();
    void servicePluginStates();
    void serviceRecentProjects();
    void serviceUnsavedChanges();
    void serviceAnnotations();
};

void ProjectPersistenceTest::projectDataDefaults() {
    ProjectData data;
    QCOMPARE(data.name, QString("Untitled"));
    QCOMPARE(data.version, QString("1.0.0"));
    QCOMPARE(data.fileVersion, ProjectData::kCurrentVersion);
    QVERIFY(data.isValid());
}

void ProjectPersistenceTest::projectDataJsonRoundtrip() {
    ProjectData data;
    data.name = "Test Project";
    data.description = "A test project";
    data.author = "Test Author";

    QJsonObject sdo;
    sdo["index"] = "0x6040";
    sdo["value"] = "0x0006";
    data.sdoConfigurations.append(sdo);

    QJsonObject topo;
    topo["slaveCount"] = 3;
    data.topologyBaseline = topo;

    QJsonObject obj = data.toJson();
    ProjectData restored = ProjectData::fromJson(obj);
    QVERIFY(restored.isValid());
    QCOMPARE(restored.name, QString("Test Project"));
    QCOMPARE(restored.description, QString("A test project"));
    QCOMPARE(restored.author, QString("Test Author"));
    QCOMPARE(restored.sdoConfigurations.size(), 1);
    QCOMPARE(restored.topologyBaseline["slaveCount"].toInt(), 3);
}

void ProjectPersistenceTest::projectDataChecksum() {
    ProjectData data;
    data.name = "Checksum Test";
    QByteArray checksum = data.computeChecksum();
    QVERIFY(!checksum.isEmpty());

    data.description = "Changed";
    QByteArray checksum2 = data.computeChecksum();
    QVERIFY(checksum != checksum2);
}

void ProjectPersistenceTest::projectDataMigration() {
    ProjectData data;
    data.fileVersion = 1;
    data.name = "Old Project";

    QString error;
    QVERIFY(ProjectData::migrate(data, 1, &error));
    QCOMPARE(data.fileVersion, ProjectData::kCurrentVersion);
    QVERIFY(data.createdTimestamp > 0);
}

void ProjectPersistenceTest::serviceCreateProject() {
    ProjectManagerService service;
    QVERIFY(service.createProject("New Project"));
    QCOMPARE(service.projectName(), QString("New Project"));
    QVERIFY(!service.hasUnsavedChanges());
}

void ProjectPersistenceTest::serviceSaveAndLoad() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.path() + "/test.ecatproj";

    ProjectManagerService writer;
    QVERIFY(writer.createProject("Save Load Test"));
    writer.projectData().description = "Testing save/load";
    writer.projectData().dashboardConfigs = QJsonArray{QJsonObject{{"type", "gauge"}}};
    writer.projectData().pluginStates["esi"] = QJsonObject{{"visible", true}};

    QVERIFY(writer.saveProjectAs(path));
    QVERIFY(!writer.hasUnsavedChanges());

    ProjectManagerService reader;
    QVERIFY(reader.openProject(path));
    QCOMPARE(reader.projectName(), QString("Save Load Test"));
    QCOMPARE(reader.projectData().description, QString("Testing save/load"));
    QCOMPARE(reader.projectData().dashboardConfigs.size(), 1);
    QVERIFY(reader.projectData().pluginStates.contains("esi"));
}

void ProjectPersistenceTest::serviceSaveAs() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path1 = dir.path() + "/proj1.ecatproj";
    QString path2 = dir.path() + "/proj2.ecatproj";

    ProjectManagerService service;
    QVERIFY(service.createProject("Save As Test"));
    QVERIFY(service.saveProjectAs(path1));
    QCOMPARE(service.projectPath(), path1);

    service.projectData().name = "Renamed";
    QVERIFY(service.saveProjectAs(path2));
    QCOMPARE(service.projectPath(), path2);

    ProjectManagerService reader;
    QVERIFY(reader.openProject(path2));
    QCOMPARE(reader.projectName(), QString("Renamed"));
}

void ProjectPersistenceTest::serviceValidateProject() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.path() + "/valid.ecatproj";

    ProjectManagerService service;
    QVERIFY(service.createProject("Validation Test"));
    QVERIFY(service.saveProjectAs(path));

    QString error;
    QVERIFY(service.validateProject(path, &error));
    QVERIFY(error.isEmpty());

    QString badPath = dir.path() + "/nonexistent.ecatproj";
    QVERIFY(!service.validateProject(badPath, &error));
    QVERIFY(!error.isEmpty());
}

void ProjectPersistenceTest::serviceCorruptChecksum() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.path() + "/corrupt.ecatproj";

    ProjectManagerService writer;
    QVERIFY(writer.createProject("Corrupt Test"));
    QVERIFY(writer.saveProjectAs(path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray content = file.readAll();
    file.close();
    content.replace("\"Corrupt Test\"", "\"Tampered Test\"");
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(content);
    file.close();

    ProjectManagerService reader;
    reader.openProject(path);
    QCOMPARE(reader.projectName(), QString("Tampered Test"));
}

void ProjectPersistenceTest::serviceMigrationOnLoad() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.path() + "/old.ecatproj";

    {
        QJsonObject root;
        root["fileVersion"] = 1;
        root["name"] = "Old Format Project";
        root["version"] = "0.9.0";
        QJsonDocument doc(root);
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(doc.toJson(QJsonDocument::Indented));
    }

    ProjectManagerService service;
    QVERIFY(service.openProject(path));
    QCOMPARE(service.projectName(), QString("Old Format Project"));
    QCOMPARE(service.projectFileVersion(), ProjectData::kCurrentVersion);
}

void ProjectPersistenceTest::serviceDashboardConfigs() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.path() + "/dashboard.ecatproj";

    ProjectManagerService writer;
    QVERIFY(writer.createProject("Dashboard Test"));

    QJsonObject gauge;
    gauge["type"] = "gauge";
    gauge["title"] = "Speed";
    gauge["min"] = 0;
    gauge["max"] = 1000;

    QJsonObject chart;
    chart["type"] = "timeseries";
    chart["title"] = "Torque";

    QJsonArray configs;
    configs.append(gauge);
    configs.append(chart);
    writer.projectData().dashboardConfigs = configs;

    QVERIFY(writer.saveProjectAs(path));

    ProjectManagerService reader;
    QVERIFY(reader.openProject(path));
    QCOMPARE(reader.projectData().dashboardConfigs.size(), 2);
    QCOMPARE(reader.projectData().dashboardConfigs[0].toObject()["title"].toString(),
             QString("Speed"));
}

void ProjectPersistenceTest::servicePluginStates() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.path() + "/plugins.ecatproj";

    ProjectManagerService writer;
    QVERIFY(writer.createProject("Plugin States Test"));

    QJsonObject esiState;
    esiState["visible"] = true;
    esiState["lastFile"] = "/path/to/esi.xml";

    QJsonObject alarmState;
    alarmState["visible"] = false;
    alarmState["threshold"] = 50;

    QJsonObject states;
    states["esi"] = esiState;
    states["alarm"] = alarmState;
    writer.projectData().pluginStates = states;

    QVERIFY(writer.saveProjectAs(path));

    ProjectManagerService reader;
    QVERIFY(reader.openProject(path));
    QVERIFY(reader.projectData().pluginStates.contains("esi"));
    QVERIFY(reader.projectData().pluginStates.contains("alarm"));
    QCOMPARE(reader.projectData().pluginStates["esi"].toObject()["lastFile"].toString(),
             QString("/path/to/esi.xml"));
}

void ProjectPersistenceTest::serviceRecentProjects() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.path() + "/recent.ecatproj";

    ProjectManagerService service;
    service.createProject("Recent Test");
    service.saveProjectAs(path);

    QStringList recent = service.recentProjects();
    QVERIFY(recent.contains(path));
}

void ProjectPersistenceTest::serviceUnsavedChanges() {
    ProjectManagerService service;
    service.createProject("Unsaved Test");
    QVERIFY(!service.hasUnsavedChanges());

    service.projectData().description = "Modified";
    QVERIFY(service.hasUnsavedChanges());
}

void ProjectPersistenceTest::serviceAnnotations() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString path = dir.path() + "/annotated.ecatproj";

    ProjectManagerService writer;
    QVERIFY(writer.createProject("Annotation Test"));

    QJsonObject notes;
    notes["general"] = "This is a test project";
    notes["topology"] = "3 slaves in chain";
    writer.projectData().notes = notes;

    QJsonObject annotations;
    annotations["slave1"] = "Drive at station 1";
    annotations["slave2"] = "IO module at station 2";
    writer.projectData().annotations = annotations;

    QVERIFY(writer.saveProjectAs(path));

    ProjectManagerService reader;
    QVERIFY(reader.openProject(path));
    QCOMPARE(reader.projectData().notes["general"].toString(),
             QString("This is a test project"));
    QCOMPARE(reader.projectData().annotations["slave1"].toString(),
             QString("Drive at station 1"));
}

QTEST_MAIN(ProjectPersistenceTest)
#include "project_persistence_test.moc"
