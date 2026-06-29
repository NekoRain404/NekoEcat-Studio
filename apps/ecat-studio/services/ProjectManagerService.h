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

/// @brief Holds all data for a single EtherCAT project file.
struct ProjectData {
    /// @brief Current project file format version.
    static constexpr int kCurrentVersion = 2;

    /// @brief File extension for EtherCAT project files.
    static constexpr const char *kFileExtension = ".ecatproj";

    int fileVersion = kCurrentVersion;  ///< Project file format version.
    QString name = "Untitled";          ///< Project name.
    QString description;                ///< Project description.
    QString version = "1.0.0";          ///< Project version string.
    QString author;                     ///< Project author.
    qint64 createdTimestamp = 0;        ///< Creation timestamp (ms since epoch).
    qint64 modifiedTimestamp = 0;       ///< Last modification timestamp (ms since epoch).
    QByteArray checksum;                ///< Project file integrity checksum.

    QJsonObject topologyBaseline;       ///< Saved network topology baseline.
    QJsonArray sdoConfigurations;       ///< SDO configuration entries.
    QJsonArray watchList;               ///< Watch list entries.
    QJsonArray startupSdoList;          ///< Startup SDO sequence.
    QJsonArray ioVariableDefinitions;   ///< IO variable definitions.
    QJsonObject notes;                  ///< User notes.
    QJsonArray esiRepository;           ///< ESI repository data.
    QJsonArray alarmHistory;            ///< Alarm history snapshot.
    QJsonArray dashboardConfigs;        ///< Dashboard configuration entries.
    QJsonObject pluginStates;           ///< Plugin state data.
    QJsonObject annotations;            ///< User annotations.

    /// @brief Check if the project data contains valid required fields.
    /// @return true if the data is valid.
    bool isValid() const;

    /// @brief Compute an integrity checksum of the project data.
    /// @return SHA-256 checksum of the serialized project data.
    QByteArray computeChecksum() const;

    /// @brief Serialize the project data to a JSON object.
    /// @return QJsonObject representation of the project.
    QJsonObject toJson() const;

    /// @brief Deserialize project data from a JSON object.
    /// @param obj    JSON object to deserialize from.
    /// @param error  Optional pointer to receive an error message on failure.
    /// @return Deserialized ProjectData structure.
    static ProjectData fromJson(const QJsonObject &obj, QString *error = nullptr);

    /// @brief Migrate project data from an older file format version.
    /// @param data        Project data to migrate (modified in place).
    /// @param fromVersion Source file format version.
    /// @param error       Optional pointer to receive an error message on failure.
    /// @return true if migration was successful.
    static bool migrate(ProjectData &data, int fromVersion, QString *error = nullptr);
};

/// @brief Manages EtherCAT project files (.ecatproj).
///
/// Handles project creation, loading, saving, export/import, and migration.
/// Stores topology baselines, SDO configurations, watch lists, and plugin states.
class ProjectManagerService : public QObject {
    Q_OBJECT
public:
    /// @brief Construct the project manager service.
    /// @param parent  Parent QObject.
    explicit ProjectManagerService(QObject *parent = nullptr);

    /// @brief Create a new project with the given name.
    /// @param name  Project name.
    /// @return true if the project was created successfully.
    bool createProject(const QString &name);

    /// @brief Open an existing project from a file path.
    /// @param filePath  Path to the .ecatproj file.
    /// @return true if the project was opened successfully.
    bool openProject(const QString &filePath);

    /// @brief Save the current project to its file.
    /// @return true if the project was saved successfully.
    bool saveProject();

    /// @brief Save the current project to a new file path.
    /// @param filePath  New file path to save to.
    /// @return true if the project was saved successfully.
    bool saveProjectAs(const QString &filePath);

    /// @brief Export the current project to a standalone file.
    /// @param filePath  Output file path.
    /// @return true if export was successful.
    bool exportProject(const QString &filePath);

    /// @brief Import a project from a standalone file.
    /// @param filePath  Input file path.
    /// @return true if import was successful.
    bool importProject(const QString &filePath);

    /// @brief Validate a project file without loading it.
    /// @param filePath  Path to the .ecatproj file.
    /// @param errorOut  Optional pointer to receive validation error message.
    /// @return true if the project file is valid.
    bool validateProject(const QString &filePath, QString *errorOut = nullptr) const;

    /// @brief Get the list of recently opened project paths.
    /// @return QStringList of file paths, most recent first.
    QStringList recentProjects() const;

    /// @brief Check if the current project has unsaved changes.
    /// @return true if there are unsaved modifications.
    bool hasUnsavedChanges() const;

    /// @brief Get the name of the current project.
    /// @return Project name string.
    QString projectName() const;

    /// @brief Get the file path of the current project.
    /// @return File path string (empty if no project is loaded).
    QString projectPath() const;

    /// @brief Get the file format version of the current project.
    /// @return File version integer.
    int projectFileVersion() const;

    /// @brief Get a mutable reference to the current project data.
    /// @return Reference to the ProjectData structure.
    ProjectData &projectData();

    /// @brief Get a const reference to the current project data.
    /// @return Const reference to the ProjectData structure.
    const ProjectData &projectData() const;

signals:
    /// @brief Emitted when a project is successfully opened.
    /// @param name  Project name.
    void projectOpened(const QString &name);

    /// @brief Emitted when a project is successfully saved.
    /// @param name  Project name.
    void projectSaved(const QString &name);

    /// @brief Emitted when the current project is closed.
    void projectClosed();

    /// @brief Emitted when a project operation error occurs.
    /// @param msg  Human-readable error message.
    void projectError(const QString &msg);

private:
    /// @brief Write project data to a file at the given path.
    /// @param path  File path to write to.
    /// @return true if the write was successful.
    bool writeProjectFile(const QString &path);

    /// @brief Read project data from a file at the given path.
    /// @param path  File path to read from.
    /// @return true if the read was successful.
    bool readProjectFile(const QString &path);

    /// @brief Migrate loaded project data to the current file format version.
    /// @param data  Project data to migrate (modified in place).
    /// @return true if migration was successful.
    bool migrateProject(ProjectData &data);

    /// @brief Add a file path to the recent projects list.
    /// @param path  File path to add.
    void addRecentProject(const QString &path);

    /// @brief Update creation and modification timestamps.
    void updateTimestamps();

    ProjectData data_;                       ///< Current project data.
    QString filePath_;                       ///< Current project file path.
    bool modified_ = false;                  ///< Whether unsaved changes exist.
    QStringList recentProjects_;             ///< Recently opened project paths.
    /// @brief Maximum number of projects retained in the recent list.
    static constexpr int kMaxRecentProjects = 10;
};
