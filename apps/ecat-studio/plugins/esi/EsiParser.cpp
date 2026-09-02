#include "EsiParser.h"
#include "services/EsiService.h"

#include <QBuffer>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>
#include <QXmlStreamReader>

EsiParser::EsiParser(QObject* parent) : QObject(parent) {}

int EsiParser::parseHexOrDec(const QString& s) const {
    QString trimmed = s.trimmed();
    if (trimmed.startsWith("0x", Qt::CaseInsensitive) || trimmed.startsWith("#x", Qt::CaseInsensitive)) {
        int start = trimmed.startsWith("0x", Qt::CaseInsensitive) ? 2 : 2;
        return trimmed.mid(start).toInt(nullptr, 16);
    }
    bool ok;
    int val = trimmed.toInt(&ok, 16);
    return ok ? val : trimmed.toInt();
}

EsiParser::ParseResult EsiParser::parseFile(const QString& filePath) {
    if (filePath.isEmpty()) {
        ParseResult r;
        r.valid = false;
        r.errorString = QStringLiteral("ESI file path is empty");
        emit parseError(r.errorString);
        return r;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ParseResult r;
        r.valid = false;
        r.errorString = QStringLiteral("Cannot open file: %1").arg(filePath);
        emit parseError(r.errorString);
        return r;
    }

    EsiService tempService;
    if (!tempService.importEsi(filePath)) {
        ParseResult r;
        r.valid = false;
        r.errorString = QStringLiteral("Failed to parse ESI file: %1").arg(filePath);
        emit parseError(r.errorString);
        return r;
    }

    devices_ = tempService.listDevices();
    deviceIndex_.clear();
    for (int i = 0; i < devices_.size(); ++i) {
        deviceIndex_[devices_[i].deviceId] = i;
    }

    emit parseComplete(devices_.size());
    return ParseResult{true, {}, {}};
}

EsiParser::ParseResult EsiParser::parseXml(const QString& xmlContent) {
    if (xmlContent.trimmed().isEmpty()) {
        ParseResult r;
        r.valid = false;
        r.errorString = QStringLiteral("ESI XML content is empty");
        emit parseError(r.errorString);
        return r;
    }

    QTemporaryDir dir;
    if (!dir.isValid()) {
        ParseResult r;
        r.valid = false;
        r.errorString = "Cannot create temporary directory";
        emit parseError(r.errorString);
        return r;
    }

    QString tempPath = dir.path() + "/esi.xml";
    QFile file(tempPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        ParseResult r;
        r.valid = false;
        r.errorString = "Cannot write temporary file";
        emit parseError(r.errorString);
        return r;
    }

    const QByteArray bytes = xmlContent.toUtf8();
    if (file.write(bytes) != bytes.size() || !file.flush()) {
        ParseResult r;
        r.valid = false;
        r.errorString = "Cannot write temporary file";
        emit parseError(r.errorString);
        return r;
    }
    file.close();

    return parseFile(tempPath);
}

EsiDeviceInfo EsiParser::deviceAt(int index) const {
    if (index < 0 || index >= devices_.size())
        return EsiDeviceInfo();
    return devices_[index];
}

EsiDeviceInfo EsiParser::matchDevice(int vendorId, int productCode) const {
    QString key = QString("%1:%2").arg(vendorId, 8, 16, QChar('0')).arg(productCode, 8, 16, QChar('0'));
    if (deviceIndex_.contains(key))
        return devices_[deviceIndex_[key]];
    return EsiDeviceInfo();
}

bool EsiParser::validateStructure(const QString& filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QXmlStreamReader xml(&file);
    bool hasDescriptions = false;
    bool hasDevice = false;

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (QStringView(xml.name()) == QStringLiteral("Descriptions"))
                hasDescriptions = true;
            if (hasDescriptions && QStringView(xml.name()) == QStringLiteral("Device"))
                hasDevice = true;
        }
    }

    return hasDescriptions && hasDevice && !xml.hasError();
}

void EsiParser::clear() {
    devices_.clear();
    deviceIndex_.clear();
    validationErrors_.clear();
}
