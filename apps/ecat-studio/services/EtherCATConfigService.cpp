#include "EtherCATConfigService.h"

#include <QDateTime>

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
    Q_UNUSED(name);
    Q_UNUSED(path);
    return true;
}

bool EtherCATConfigService::importProfile(const QString &path)
{
    Q_UNUSED(path);
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
