#include "ConfigurationService.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// ConfigurationService.cpp — EtherCAT network configuration with JSON persistence
//
// Implementation notes:
//   - Manages MasterConfig, SlaveConfig, NetworkConfig, TimingConfig, SafetyConfig
//   - Validates cycle times, slave positions, DC sync, and watchdog settings
//   - Serializes/deserializes the full configuration tree to/from JSON files

QJsonObject MasterConfig::toJson() const {
    QJsonObject obj;
    obj["adapter"] = adapter;
    obj["cycleTimeUs"] = cycleTimeUs;
    obj["distributedClocks"] = distributedClocks;
    obj["dcSync0Shift"] = dcSync0Shift;
    return obj;
}

MasterConfig MasterConfig::fromJson(const QJsonObject &obj) {
    MasterConfig c;
    c.adapter = obj["adapter"].toString();
    c.cycleTimeUs = obj["cycleTimeUs"].toInt(1000);
    c.distributedClocks = obj["distributedClocks"].toBool(true);
    c.dcSync0Shift = obj["dcSync0Shift"].toInt(0);
    return c;
}

QJsonObject SlaveConfig::toJson() const {
    QJsonObject obj;
    obj["position"] = position;
    obj["name"] = name;
    obj["vendorId"] = vendorId;
    obj["productCode"] = productCode;
    obj["sdos"] = sdos;
    return obj;
}

SlaveConfig SlaveConfig::fromJson(const QJsonObject &obj) {
    SlaveConfig c;
    c.position = obj["position"].toInt(-1);
    c.name = obj["name"].toString();
    c.vendorId = obj["vendorId"].toInt(0);
    c.productCode = obj["productCode"].toInt(0);
    c.sdos = obj["sdos"].toArray();
    return c;
}

QJsonObject NetworkConfig::toJson() const {
    QJsonObject obj;
    obj["ipAddress"] = ipAddress;
    obj["subnetMask"] = subnetMask;
    obj["gateway"] = gateway;
    obj["dns"] = dns;
    return obj;
}

NetworkConfig NetworkConfig::fromJson(const QJsonObject &obj) {
    NetworkConfig c;
    c.ipAddress = obj["ipAddress"].toString();
    c.subnetMask = obj["subnetMask"].toString();
    c.gateway = obj["gateway"].toString();
    c.dns = obj["dns"].toString();
    return c;
}

QJsonObject TimingConfig::toJson() const {
    QJsonObject obj;
    obj["cycleTimeUs"] = cycleTimeUs;
    obj["sync0Shift"] = sync0Shift;
    obj["sync1Shift"] = sync1Shift;
    obj["sync0Enabled"] = sync0Enabled;
    obj["sync1Enabled"] = sync1Enabled;
    return obj;
}

TimingConfig TimingConfig::fromJson(const QJsonObject &obj) {
    TimingConfig c;
    c.cycleTimeUs = obj["cycleTimeUs"].toInt(1000);
    c.sync0Shift = obj["sync0Shift"].toInt(0);
    c.sync1Shift = obj["sync1Shift"].toInt(0);
    c.sync0Enabled = obj["sync0Enabled"].toBool(true);
    c.sync1Enabled = obj["sync1Enabled"].toBool(false);
    return c;
}

QJsonObject SafetyConfig::toJson() const {
    QJsonObject obj;
    obj["watchdogTimeoutMs"] = watchdogTimeoutMs;
    obj["errorBehavior"] = errorBehavior;
    obj["autoRecover"] = autoRecover;
    obj["maxRetries"] = maxRetries;
    return obj;
}

SafetyConfig SafetyConfig::fromJson(const QJsonObject &obj) {
    SafetyConfig c;
    c.watchdogTimeoutMs = obj["watchdogTimeoutMs"].toInt(5000);
    c.errorBehavior = obj["errorBehavior"].toString("safeop");
    c.autoRecover = obj["autoRecover"].toBool(false);
    c.maxRetries = obj["maxRetries"].toInt(3);
    return c;
}

ConfigurationService::ConfigurationService(QObject *parent)
    : QObject(parent)
{
}

bool ConfigurationService::loadConfiguration(const QString &filePath) {
    if (filePath.isEmpty())
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    QJsonObject root = doc.object();
    if (!root.value("master").isObject() ||
        !root.value("network").isObject() ||
        !root.value("timing").isObject() ||
        !root.value("safety").isObject() ||
        !root.value("slaves").isArray()) {
        return false;
    }

    QVector<SlaveConfig> loadedSlaves;
    const QJsonArray slavesArray = root.value("slaves").toArray();
    for (const auto &v : slavesArray) {
        if (!v.isObject())
            return false;
        loadedSlaves.append(SlaveConfig::fromJson(v.toObject()));
    }

    MasterConfig loadedMaster = MasterConfig::fromJson(root["master"].toObject());
    NetworkConfig loadedNetwork = NetworkConfig::fromJson(root["network"].toObject());
    TimingConfig loadedTiming = TimingConfig::fromJson(root["timing"].toObject());
    SafetyConfig loadedSafety = SafetyConfig::fromJson(root["safety"].toObject());

    master_ = loadedMaster;
    network_ = loadedNetwork;
    timing_ = loadedTiming;
    safety_ = loadedSafety;
    slaves_ = loadedSlaves;

    emit configurationChanged();
    return true;
}

bool ConfigurationService::saveConfiguration(const QString &filePath) {
    if (filePath.isEmpty())
        return false;

    QJsonObject root;
    root["master"] = master_.toJson();
    root["network"] = network_.toJson();
    root["timing"] = timing_.toJson();
    root["safety"] = safety_.toJson();

    QJsonArray slaveArr;
    for (const auto &s : slaves_)
        slaveArr.append(s.toJson());
    root["slaves"] = slaveArr;

    QJsonDocument doc(root);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    const QByteArray bytes = doc.toJson(QJsonDocument::Indented);
    if (file.write(bytes) != bytes.size() || !file.flush())
        return false;
    return true;
}

bool ConfigurationService::applyConfiguration() {
    ConfigValidationResult result = validateConfiguration();
    if (!result.valid)
        return false;
    emit configurationChanged();
    return true;
}

ConfigValidationResult ConfigurationService::validateConfiguration() const {
    ConfigValidationResult r;

    if (master_.cycleTimeUs <= 0) {
        r.valid = false;
        r.errors.append("Master cycle time must be positive");
    }
    if (master_.cycleTimeUs < 100)
        r.warnings.append("Cycle time < 100us may cause instability");

    for (const auto &slave : slaves_) {
        if (slave.position < 0) {
            r.valid = false;
            r.errors.append(QString("Slave '%1' has invalid position").arg(slave.name));
        }
    }

    if (timing_.sync0Enabled && timing_.cycleTimeUs <= 0) {
        r.valid = false;
        r.errors.append("DC cycle time must be positive when sync is enabled");
    }

    if (safety_.watchdogTimeoutMs <= 0) {
        r.valid = false;
        r.errors.append("Watchdog timeout must be positive");
    }

    return r;
}

void ConfigurationService::resetToDefaults() {
    master_ = MasterConfig();
    slaves_.clear();
    network_ = NetworkConfig();
    timing_ = TimingConfig();
    safety_ = SafetyConfig();
    emit configurationChanged();
}

MasterConfig &ConfigurationService::masterConfig() {
    return master_;
}

const MasterConfig &ConfigurationService::masterConfig() const {
    return master_;
}

QVector<SlaveConfig> &ConfigurationService::slaveConfigs() {
    return slaves_;
}

const QVector<SlaveConfig> &ConfigurationService::slaveConfigs() const {
    return slaves_;
}

NetworkConfig &ConfigurationService::networkConfig() {
    return network_;
}

const NetworkConfig &ConfigurationService::networkConfig() const {
    return network_;
}

TimingConfig &ConfigurationService::timingConfig() {
    return timing_;
}

const TimingConfig &ConfigurationService::timingConfig() const {
    return timing_;
}

SafetyConfig &ConfigurationService::safetyConfig() {
    return safety_;
}

const SafetyConfig &ConfigurationService::safetyConfig() const {
    return safety_;
}
