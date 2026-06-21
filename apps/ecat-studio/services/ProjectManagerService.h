#pragma once

// ProjectManagerService — manages EtherCAT project files (.ecatproj).
//
// Handles project creation, loading, saving, and migration. Stores topology
// baselines, SDO configurations, watch lists, and plugin states.
//
// Thread safety: main (GUI) thread only.

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QByteArray>

struct ProjectData {
    static constexpr int kCurrentVersion = 2;
    static constexpr const char *kFileExtension = ".ecatproj";

    int fileVersion = kCurrentVersion;
    QString name = "Untitled";
    QString description;
    QString version = "1.0.0";
    QString author;
    qint64 createdTimestamp = 0;
    qint64 modifiedTimestamp = 0;
    QByteArray checksum;

    QJsonObject topologyBaseline;
    QJsonArray sdoConfigurations;
    QJsonArray watchList;
    QJsonArray startupSdoList;
    QJsonArray ioVariableDefinitions;
    QJsonObject notes;
    QJsonArray esiRepository;
    QJsonArray alarmHistory;
    QJsonArray dashboardConfigs;
    QJsonObject pluginStates;
    QJsonObject annotations;

    bool isValid() const;
    QByteArray computeChecksum() const;
    QJsonObject toJson() const;
    static ProjectData fromJson(const QJsonObject &obj, QString *error = nullptr);
    static bool migrate(ProjectData &data, int fromVersion, QString *error = nullptr);
};

class ProjectManagerService : public QObject {
    Q_OBJECT
public:
    explicit ProjectManagerService(QObject *parent = nullptr);

    bool createProject(const QString &name);
    bool openProject(const QString &filePath);
    bool saveProject();
    bool saveProjectAs(const QString &filePath);
    bool exportProject(const QString &filePath);
    bool importProject(const QString &filePath);

    bool validateProject(const QString &filePath, QString *errorOut = nullptr) const;

    QStringList recentProjects() const;
    bool hasUnsavedChanges() const;
    QString projectName() const;
    QString projectPath() const;
    int projectFileVersion() const;

    ProjectData &projectData();
    const ProjectData &projectData() const;

signals:
    void projectOpened(const QString &name);
    void projectSaved(const QString &name);
    void projectClosed();
    void projectError(const QString &msg);

private:
    bool writeProjectFile(const QString &path);
    bool readProjectFile(const QString &path);
    bool migrateProject(ProjectData &data);
    void addRecentProject(const QString &path);
    void updateTimestamps();

    ProjectData data_;
    QString filePath_;
    bool modified_ = false;
    QStringList recentProjects_;
    static constexpr int kMaxRecentProjects = 10;
};
