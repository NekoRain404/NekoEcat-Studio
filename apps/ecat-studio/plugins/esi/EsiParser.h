#pragma once

#include <QMap>
#include <QObject>
#include <QString>
#include <QVector>

struct EsiPdoEntry;
struct EsiPdoAssignment;
struct EsiSyncManager;
struct EsiDeviceInfo;

class EsiParser : public QObject {
    Q_OBJECT
public:
    explicit EsiParser(QObject* parent = nullptr);

    struct ParseResult {
        bool valid = false;
        QString errorString;
        QVector<int> deviceIndices;
    };

    ParseResult parseFile(const QString& filePath);
    ParseResult parseXml(const QString& xmlContent);

    QVector<EsiDeviceInfo> devices() const { return devices_; }
    int deviceCount() const { return devices_.size(); }
    EsiDeviceInfo deviceAt(int index) const;
    EsiDeviceInfo matchDevice(int vendorId, int productCode) const;

    bool validateStructure(const QString& filePath) const;
    QStringList validationErrors() const { return validationErrors_; }

    void clear();

signals:
    void parseError(const QString& msg);
    void parseComplete(int deviceCount);

private:
    int parseHexOrDec(const QString& s) const;
    bool parseXmlStream(QIODevice* device);

    QVector<EsiDeviceInfo> devices_;
    QMap<QString, int> deviceIndex_;
    QStringList validationErrors_;
};
