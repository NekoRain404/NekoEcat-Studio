#include "EsiService.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QTextStream>

// EsiService.cpp — Parses and exports EtherCAT Slave Information (ESI) XML files
//
// Implementation notes:
//   - QXmlStreamReader-based parser extracts Device, RxPdo, TxPdo, Sm, Entry elements
//   - Devices indexed by vendorId:productCode hex key for O(1) lookup
//   - Can re-export a single device back to ESI XML format

EsiService::EsiService(QObject *parent) : QObject(parent) {}

int EsiService::parseHexOrDec(const QString &s) {
  QString trimmed = s.trimmed();
  if (trimmed.startsWith("0x", Qt::CaseInsensitive))
    return trimmed.mid(2).toInt(nullptr, 16);
  return trimmed.toInt(nullptr, 16);
}

bool EsiService::importEsi(const QString &filePath) {
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    emit error(QStringLiteral("Cannot open file: %1").arg(filePath));
    return false;
  }

  QXmlStreamReader xml(&file);
  EsiDeviceInfo current;
  bool inDescriptions = false;
  bool inDevice = false;
  bool inRxPdo = false;
  bool inTxPdo = false;
  bool inSm = false;
  EsiPdoAssignment currentPdo;
  EsiPdoEntry currentEntry;

  while (!xml.atEnd()) {
    xml.readNext();
    if (!xml.isStartElement()) {
      if (xml.isEndElement()) {
        QStringView name(xml.name());
        if (name == "Descriptions") inDescriptions = false;
        if (name == "Device") {
          if (inDevice && current.vendorId != 0) {
            QString key = QString("%1:%2")
                              .arg(current.vendorId, 8, 16, QChar('0'))
                              .arg(current.productCode, 8, 16, QChar('0'));
            if (!deviceIndex_.contains(key)) {
              current.deviceId = key;
              deviceIndex_[key] = devices_.size();
              devices_.append(current);
            }
          }
          current = EsiDeviceInfo();
          inDevice = false;
        }
        if (name == "RxPdo") {
          if (inRxPdo) {
            current.rxPdos.append(currentPdo);
            currentPdo = EsiPdoAssignment();
          }
          inRxPdo = false;
        }
        if (name == "TxPdo") {
          if (inTxPdo) {
            current.txPdos.append(currentPdo);
            currentPdo = EsiPdoAssignment();
          }
          inTxPdo = false;
        }
        if (name == "Sm") inSm = false;
        if (name == "Entry") {
          if (inRxPdo || inTxPdo) currentPdo.entries.append(currentEntry);
          currentEntry = EsiPdoEntry();
        }
      }
      continue;
    }

    QStringView name(xml.name());
    if (name == "Descriptions") inDescriptions = true;
    if (inDescriptions && name == "Device") {
      inDevice = true;
      current = EsiDeviceInfo();
    }
    if (inDevice) {
      if (name == "Type") {
        QString typeText = xml.attributes().value("Type").toString();
        int vid = parseHexOrDec(xml.attributes().value("VendorId").toString());
        int pcode = parseHexOrDec(xml.attributes().value("ProductCode").toString());
        int rev = parseHexOrDec(xml.attributes().value("RevisionNo").toString());
        if (vid) current.vendorId = vid;
        if (pcode) current.productCode = pcode;
        if (rev) current.revisionNo = rev;
        if (!typeText.isEmpty()) current.type = typeText;
        current.name = xml.readElementText();
        if (current.name.isEmpty()) current.name = current.type;
      } else if (name == "Name") {
        current.name = xml.readElementText();
      } else if (name == "Desc") {
        current.description = xml.readElementText();
      } else if (name == "Sm") {
        inSm = true;
        EsiSyncManager sm;
        sm.index = xml.attributes().value("Index").toString().toInt();
        sm.direction = xml.attributes().value("Dir").toString();
        current.syncManagers.append(sm);
      } else if (name == "RxPdo") {
        inRxPdo = true;
        currentPdo = EsiPdoAssignment();
        currentPdo.index = xml.attributes().value("Index").toString();
        if (currentPdo.index.startsWith("0x"))
          currentPdo.index = currentPdo.index.mid(2);
      } else if (name == "TxPdo") {
        inTxPdo = true;
        currentPdo = EsiPdoAssignment();
        currentPdo.index = xml.attributes().value("Index").toString();
        if (currentPdo.index.startsWith("0x"))
          currentPdo.index = currentPdo.index.mid(2);
      } else if (name == "Entry") {
        currentEntry.index = xml.attributes().value("Index").toString();
        if (currentEntry.index.startsWith("0x"))
          currentEntry.index = currentEntry.index.mid(2);
        currentEntry.subIndex = xml.attributes().value("SubIndex").toString();
        currentEntry.bitSize = xml.attributes().value("BitLen").toString().toInt();
      } else if (name == "Name" && (inRxPdo || inTxPdo)) {
        currentEntry.name = xml.readElementText();
      } else if (name == "DataType" && (inRxPdo || inTxPdo)) {
        currentEntry.type = xml.readElementText();
      }
    }
  }

  if (xml.hasError()) {
    emit error(QStringLiteral("XML parse error: %1").arg(xml.errorString()));
    return false;
  }

  emit esiImported(devices_.size());
  return true;
}

EsiDeviceInfo EsiService::matchDevice(int vendorId, int productCode) const {
  QString key = QString("%1:%2")
                    .arg(vendorId, 8, 16, QChar('0'))
                    .arg(productCode, 8, 16, QChar('0'));
  if (deviceIndex_.contains(key))
    return devices_[deviceIndex_[key]];
  return EsiDeviceInfo();
}

QVector<EsiDeviceInfo> EsiService::listDevices() const { return devices_; }

int EsiService::deviceCount() const { return devices_.size(); }

void EsiService::clear() {
  devices_.clear();
  deviceIndex_.clear();
}

bool EsiService::exportEsi(const QString &deviceId,
                           const QString &outputPath) const {
  if (!deviceIndex_.contains(deviceId)) return false;
  const EsiDeviceInfo &dev = devices_[deviceIndex_[deviceId]];

  QFile file(outputPath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

  QTextStream out(&file);
  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  out << "<EtherCATInfo>\n";
  out << "  <Descriptions>\n";
  out << "    <Device>\n";
  out << "      <Type ProductCode=\"#x"
      << QString::number(dev.productCode, 16)
      << "\" RevisionNo=\"#x"
      << QString::number(dev.revisionNo, 16)
      << "\" VendorId=\"#x"
      << QString::number(dev.vendorId, 16) << "\">"
      << dev.name << "</Type>\n";
  out << "      <Name>" << dev.name << "</Name>\n";
  if (!dev.description.isEmpty())
    out << "      <Desc>" << dev.description << "</Desc>\n";

  for (const auto &sm : dev.syncManagers) {
    out << "      <Sm Index=\"" << sm.index << "\" Dir=\""
        << sm.direction << "\"/>\n";
  }

  auto writePdo = [&](const QString &tag, const EsiPdoAssignment &pdo) {
    out << "      <" << tag << ">\n";
    out << "        <Index>#x" << pdo.index << "</Index>\n";
    out << "        <Name>" << pdo.name << "</Name>\n";
    for (const auto &e : pdo.entries) {
      out << "        <Entry>\n";
      out << "          <Index>#x" << e.index << "</Index>\n";
      out << "          <SubIndex>#x" << e.subIndex << "</SubIndex>\n";
      out << "          <BitLen>" << e.bitSize << "</BitLen>\n";
      if (!e.name.isEmpty())
        out << "          <Name>" << e.name << "</Name>\n";
      if (!e.type.isEmpty())
        out << "          <DataType>#x" << e.type << "</DataType>\n";
      out << "        </Entry>\n";
    }
    out << "      </" << tag << ">\n";
  };

  for (const auto &pdo : dev.rxPdos) writePdo("RxPdo", pdo);
  for (const auto &pdo : dev.txPdos) writePdo("TxPdo", pdo);

  out << "    </Device>\n";
  out << "  </Descriptions>\n";
  out << "</EtherCATInfo>\n";
  return true;
}
