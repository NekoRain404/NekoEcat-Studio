#include "EtherCATConfigService.h"

#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

// EtherCATConfigService.cpp — Named configuration profiles with parameter management
//
// Implementation notes:
//   - Profiles stored in-memory; save/load by name with upsert semantics
//   - Validates profile name and parameter names; warns on read-only parameter values
//   - Supports add/remove/update of individual ConfigParameter entries

EtherCATConfigService::EtherCATConfigService(QObject *parent)
    : QObject(parent)
{
}

void EtherCATConfigService::setCurrentProfile(const ConfigProfile &profile)
{
    profile_ = profile;
    emit configurationChanged(profile_);
}

ConfigValidation EtherCATConfigService::validateProfile(const ConfigProfile &profile) const
{
    ConfigValidation result;
    result.valid = true;

    if (profile.name.isEmpty()) {
        result.valid = false;
        result.errors.append(QStringLiteral("Profile name is empty"));
    }

    for (const ConfigParameter &p : profile.parameters) {
        if (p.name.isEmpty()) {
            result.valid = false;
            result.errors.append(QStringLiteral("Parameter has empty name"));
        }
        if (p.readOnly && !p.value.isEmpty()) {
            result.warnings.append(
                QStringLiteral("Read-only parameter '%1' has a value set").arg(p.name));
        }
    }

    return result;
}

bool EtherCATConfigService::saveProfile(const QString &name)
{
    if (name.isEmpty())
        return false;

    for (int i = 0; i < savedProfiles_.size(); ++i) {
        if (savedProfiles_.at(i).name == name) {
            ConfigProfile p = profile_;
            p.name = name;
            p.timestampMs = QDateTime::currentMSecsSinceEpoch();
            savedProfiles_[i] = p;
            emit profileSaved(name);
            return true;
        }
    }

    ConfigProfile p = profile_;
    p.name = name;
    p.timestampMs = QDateTime::currentMSecsSinceEpoch();
    savedProfiles_.append(p);
    emit profileSaved(name);
    return true;
}

bool EtherCATConfigService::loadProfile(const QString &name)
{
    for (const ConfigProfile &p : savedProfiles_) {
        if (p.name == name) {
            profile_ = p;
            emit profileLoaded(name);
            emit configurationChanged(profile_);
            return true;
        }
    }
    return false;
}

bool EtherCATConfigService::deleteProfile(const QString &name)
{
    for (int i = 0; i < savedProfiles_.size(); ++i) {
        if (savedProfiles_.at(i).name == name) {
            savedProfiles_.removeAt(i);
            return true;
        }
    }
    return false;
}

bool EtherCATConfigService::exportProfile(const QString &name, const QString &path) const
{
    if (name.isEmpty() || path.isEmpty())
        return false;

    const ConfigProfile *profile = nullptr;
    for (const ConfigProfile &p : savedProfiles_) {
        if (p.name == name) {
            profile = &p;
            break;
        }
    }
    if (!profile)
        return false;

    QJsonArray params;
    for (const ConfigParameter &param : profile->parameters) {
        QJsonObject obj;
        obj[QStringLiteral("name")] = param.name;
        obj[QStringLiteral("value")] = param.value;
        obj[QStringLiteral("unit")] = param.unit;
        obj[QStringLiteral("description")] = param.description;
        obj[QStringLiteral("readOnly")] = param.readOnly;
        params.append(obj);
    }

    QJsonObject root;
    root[QStringLiteral("name")] = profile->name;
    root[QStringLiteral("description")] = profile->description;
    root[QStringLiteral("timestampMs")] = QString::number(profile->timestampMs);
    root[QStringLiteral("parameters")] = params;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    const QByteArray bytes = QJsonDocument(root).toJson(QJsonDocument::Indented);
    return file.write(bytes) == bytes.size() && file.flush();
}

bool EtherCATConfigService::importProfile(const QString &path)
{
    if (path.isEmpty())
        return false;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    const QJsonObject root = doc.object();
    if (!root.contains(QStringLiteral("parameters")) ||
        !root.value(QStringLiteral("parameters")).isArray()) {
        return false;
    }

    ConfigProfile profile;
    profile.name = root.value(QStringLiteral("name")).toString();
    profile.description = root.value(QStringLiteral("description")).toString();
    const auto timestampValue = root.value(QStringLiteral("timestampMs"));
    profile.timestampMs = timestampValue.isDouble()
                              ? static_cast<qint64>(timestampValue.toDouble())
                              : timestampValue.toString().toLongLong();

    const QJsonArray params = root.value(QStringLiteral("parameters")).toArray();
    for (const QJsonValue &value : params) {
        if (!value.isObject())
            return false;
        const QJsonObject obj = value.toObject();
        ConfigParameter param;
        param.name = obj.value(QStringLiteral("name")).toString();
        param.value = obj.value(QStringLiteral("value")).toString();
        param.unit = obj.value(QStringLiteral("unit")).toString();
        param.description = obj.value(QStringLiteral("description")).toString();
        param.readOnly = obj.value(QStringLiteral("readOnly")).toBool(false);
        profile.parameters.append(param);
    }

    const ConfigValidation validation = validateProfile(profile);
    if (!validation.valid)
        return false;

    for (int i = 0; i < savedProfiles_.size(); ++i) {
        if (savedProfiles_.at(i).name == profile.name) {
            savedProfiles_[i] = profile;
            profile_ = profile;
            emit profileLoaded(profile.name);
            emit configurationChanged(profile_);
            return true;
        }
    }

    savedProfiles_.append(profile);
    profile_ = profile;
    emit profileLoaded(profile.name);
    emit configurationChanged(profile_);
    return true;
}

void EtherCATConfigService::addParameter(const ConfigParameter &param)
{
    profile_.parameters.append(param);
    emit configurationChanged(profile_);
}

void EtherCATConfigService::removeParameter(const QString &name)
{
    for (int i = 0; i < profile_.parameters.size(); ++i) {
        if (profile_.parameters.at(i).name == name) {
            profile_.parameters.removeAt(i);
            emit configurationChanged(profile_);
            return;
        }
    }
}

void EtherCATConfigService::updateParameter(const QString &name, const QString &value)
{
    for (int i = 0; i < profile_.parameters.size(); ++i) {
        if (profile_.parameters.at(i).name == name) {
            profile_.parameters[i].value = value;
            emit configurationChanged(profile_);
            return;
        }
    }
}
