#include "ProjectManagerService.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

// ProjectData ───────────────────────────────────────────────────────────

bool ProjectData::isValid() const {
    return !name.isEmpty() && fileVersion > 0;
}

QByteArray ProjectData::computeChecksum() const {
    QJsonObject obj = toJson();
    obj.remove("checksum");
    QJsonDocument doc(obj);
    return QCryptographicHash::hash(doc.toJson(QJsonDocument::Compact), QCryptographicHash::Sha256).toHex();
}

QJsonObject ProjectData::toJson() const {
    QJsonObject root;
    root["fileVersion"] = fileVersion;
    root["name"] = name;
    root["description"] = description;
    root["version"] = version;
    root["author"] = author;
    root["createdTimestamp"] = createdTimestamp;
    root["modifiedTimestamp"] = modifiedTimestamp;
    root["checksum"] = QString::fromLatin1(checksum);
    root["topologyBaseline"] = topologyBaseline;
    root["sdoConfigurations"] = sdoConfigurations;
    root["watchList"] = watchList;
    root["startupSdoList"] = startupSdoList;
    root["ioVariableDefinitions"] = ioVariableDefinitions;
    root["notes"] = notes;
    root["esiRepository"] = esiRepository;
    root["alarmHistory"] = alarmHistory;
    root["dashboardConfigs"] = dashboardConfigs;
    root["pluginStates"] = pluginStates;
    root["annotations"] = annotations;
    return root;
}

ProjectData ProjectData::fromJson(const QJsonObject& obj, QString* error) {
    ProjectData data;
    data.fileVersion = obj["fileVersion"].toInt(1);
    data.name = obj["name"].toString("Untitled");
    data.description = obj["description"].toString();
    data.version = obj["version"].toString("1.0.0");
    data.author = obj["author"].toString();
    data.createdTimestamp = obj["createdTimestamp"].toVariant().toLongLong();
    data.modifiedTimestamp = obj["modifiedTimestamp"].toVariant().toLongLong();
    data.checksum = obj["checksum"].toString().toLatin1();
    data.topologyBaseline = obj["topologyBaseline"].toObject();
    data.sdoConfigurations = obj["sdoConfigurations"].toArray();
    data.watchList = obj["watchList"].toArray();
    data.startupSdoList = obj["startupSdoList"].toArray();
    data.ioVariableDefinitions = obj["ioVariableDefinitions"].toArray();
    data.notes = obj["notes"].toObject();
    data.esiRepository = obj["esiRepository"].toArray();
    data.alarmHistory = obj["alarmHistory"].toArray();
    data.dashboardConfigs = obj["dashboardConfigs"].toArray();
    data.pluginStates = obj["pluginStates"].toObject();
    data.annotations = obj["annotations"].toObject();

    if (!data.isValid()) {
        if (error)
            *error = "Invalid project data: missing required fields";
        return ProjectData();
    }
    return data;
}

bool ProjectData::migrate(ProjectData& data, int fromVersion, QString* error) {
    if (fromVersion >= kCurrentVersion)
        return true;

    if (fromVersion < 2) {
        if (data.dashboardConfigs.isEmpty())
            data.dashboardConfigs = QJsonArray();
        if (data.pluginStates.isEmpty())
            data.pluginStates = QJsonObject();
        if (data.annotations.isEmpty())
            data.annotations = QJsonObject();
        if (data.createdTimestamp == 0)
            data.createdTimestamp = QDateTime::currentMSecsSinceEpoch();
        if (data.author.isEmpty())
            data.author = "";
    }

    data.fileVersion = kCurrentVersion;
    return true;
}

// ProjectManagerService ─────────────────────────────────────────────────

ProjectManagerService::ProjectManagerService(QObject* parent) : QObject(parent) {
    QSettings s("NekoEcatStudio", "NekoEcatStudio");
    recentProjects_ = s.value("recentProjects").toStringList();
}

bool ProjectManagerService::createProject(const QString& name) {
    if (name.isEmpty())
        return false;
    data_ = ProjectData();
    data_.name = name;
    data_.createdTimestamp = QDateTime::currentMSecsSinceEpoch();
    data_.modifiedTimestamp = data_.createdTimestamp;
    data_.checksum = data_.computeChecksum();
    filePath_.clear();
    modified_ = false;
    emit projectOpened(name);
    return true;
}

bool ProjectManagerService::openProject(const QString& filePath) {
    if (!readProjectFile(filePath))
        return false;
    filePath_ = filePath;
    modified_ = false;
    addRecentProject(filePath);
    emit projectOpened(data_.name);
    return true;
}

bool ProjectManagerService::saveProject() {
    if (filePath_.isEmpty())
        return false;
    updateTimestamps();
    if (!writeProjectFile(filePath_))
        return false;
    modified_ = false;
    emit projectSaved(data_.name);
    return true;
}

bool ProjectManagerService::saveProjectAs(const QString& filePath) {
    updateTimestamps();
    if (!writeProjectFile(filePath))
        return false;
    filePath_ = filePath;
    modified_ = false;
    addRecentProject(filePath);
    emit projectSaved(data_.name);
    return true;
}

bool ProjectManagerService::exportProject(const QString& filePath) {
    updateTimestamps();
    return writeProjectFile(filePath);
}

bool ProjectManagerService::importProject(const QString& filePath) {
    return openProject(filePath);
}

bool ProjectManagerService::validateProject(const QString& filePath, QString* errorOut) const {
    if (filePath.isEmpty()) {
        if (errorOut)
            *errorOut = QStringLiteral("Project path is empty");
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorOut)
            *errorOut = QStringLiteral("Cannot open file: %1").arg(filePath);
        return false;
    }

    QByteArray content = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(content);
    if (doc.isNull() || !doc.isObject()) {
        if (errorOut)
            *errorOut = "Invalid JSON format";
        return false;
    }

    QJsonObject root = doc.object();
    if (!root.contains("name") || !root.contains("fileVersion")) {
        if (errorOut)
            *errorOut = "Missing required project fields";
        return false;
    }

    QString storedChecksum = root["checksum"].toString();
    if (!storedChecksum.isEmpty()) {
        QJsonObject checkObj = root;
        checkObj.remove("checksum");
        QJsonDocument checkDoc(checkObj);
        QByteArray computed =
            QCryptographicHash::hash(checkDoc.toJson(QJsonDocument::Compact), QCryptographicHash::Sha256).toHex();
        if (storedChecksum.toLatin1() != computed) {
            if (errorOut)
                *errorOut = "Checksum mismatch — file may be corrupted";
            return false;
        }
    }

    int fileVersion = root["fileVersion"].toInt(0);
    if (fileVersion > ProjectData::kCurrentVersion) {
        if (errorOut)
            *errorOut = QStringLiteral("Unsupported file version: %1 (max: %2)")
                            .arg(fileVersion)
                            .arg(ProjectData::kCurrentVersion);
        return false;
    }

    return true;
}

QStringList ProjectManagerService::recentProjects() const {
    return recentProjects_;
}

bool ProjectManagerService::hasUnsavedChanges() const {
    return modified_;
}

QString ProjectManagerService::projectName() const {
    return data_.name;
}

QString ProjectManagerService::projectPath() const {
    return filePath_;
}

int ProjectManagerService::projectFileVersion() const {
    return data_.fileVersion;
}

ProjectData& ProjectManagerService::projectData() {
    modified_ = true;
    return data_;
}

const ProjectData& ProjectManagerService::projectData() const {
    return data_;
}

bool ProjectManagerService::writeProjectFile(const QString& path) {
    if (path.isEmpty()) {
        emit projectError(QStringLiteral("Project path is empty"));
        return false;
    }

    QJsonObject root = data_.toJson();
    data_.checksum = data_.computeChecksum();
    root["checksum"] = QString::fromLatin1(data_.checksum);

    QJsonDocument doc(root);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        emit projectError(QStringLiteral("Cannot write file: %1").arg(path));
        return false;
    }
    const QByteArray bytes = doc.toJson(QJsonDocument::Indented);
    if (file.write(bytes) != bytes.size() || !file.flush()) {
        emit projectError(QStringLiteral("Cannot write file: %1").arg(path));
        return false;
    }
    return true;
}

bool ProjectManagerService::readProjectFile(const QString& path) {
    if (path.isEmpty()) {
        emit projectError(QStringLiteral("Project path is empty"));
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        emit projectError(QStringLiteral("Cannot read file: %1").arg(path));
        return false;
    }
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        emit projectError("Invalid project file format");
        return false;
    }

    QJsonObject root = doc.object();
    const int fileVersion = root["fileVersion"].toInt(0);
    if (fileVersion > ProjectData::kCurrentVersion) {
        emit projectError(QStringLiteral("Unsupported file version: %1 (max: %2)")
                              .arg(fileVersion)
                              .arg(ProjectData::kCurrentVersion));
        return false;
    }

    QString storedChecksum = root["checksum"].toString();
    if (!storedChecksum.isEmpty()) {
        QJsonObject checkObj = root;
        checkObj.remove("checksum");
        QJsonDocument checkDoc(checkObj);
        QByteArray computed =
            QCryptographicHash::hash(checkDoc.toJson(QJsonDocument::Compact), QCryptographicHash::Sha256).toHex();
        if (storedChecksum.toLatin1() != computed) {
            emit projectError("Checksum mismatch — file may be corrupted");
            return false;
        }
    }

    QString error;
    ProjectData loaded = ProjectData::fromJson(root, &error);
    if (!loaded.isValid()) {
        emit projectError(error);
        return false;
    }

    if (loaded.fileVersion < ProjectData::kCurrentVersion) {
        if (!migrateProject(loaded)) {
            emit projectError("Project migration failed");
            return false;
        }
    }

    data_ = loaded;
    return true;
}

bool ProjectManagerService::migrateProject(ProjectData& data) {
    return ProjectData::migrate(data, data.fileVersion, nullptr);
}

void ProjectManagerService::addRecentProject(const QString& path) {
    recentProjects_.removeAll(path);
    recentProjects_.prepend(path);
    while (recentProjects_.size() > kMaxRecentProjects)
        recentProjects_.removeLast();
    QSettings s("NekoEcatStudio", "NekoEcatStudio");
    s.setValue("recentProjects", recentProjects_);
}

void ProjectManagerService::updateTimestamps() {
    data_.modifiedTimestamp = QDateTime::currentMSecsSinceEpoch();
    if (data_.createdTimestamp == 0)
        data_.createdTimestamp = data_.modifiedTimestamp;
    data_.fileVersion = ProjectData::kCurrentVersion;
}
