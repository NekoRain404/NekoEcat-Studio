#include "TestDataGenerator.h"

#include <QRandomGenerator>

QVector<SlaveInfo> TestDataGenerator::generateSlaveInfo(int count, DataPattern pattern) {
    QVector<SlaveInfo> result;
    result.reserve(count);
    for (int i = 0; i < count; i++)
        result.append(makeSlave(i, pattern));
    return result;
}

QVector<SdoValue> TestDataGenerator::generateSdoValues(int count, DataPattern pattern) {
    QVector<SdoValue> result;
    result.reserve(count);
    for (int i = 0; i < count; i++)
        result.append(makeSdo(i % 16, i, pattern));
    return result;
}

QVector<PdoMapping> TestDataGenerator::generatePdoMappings(int count, DataPattern pattern) {
    QVector<PdoMapping> result;
    result.reserve(count);
    for (int i = 0; i < count; i++)
        result.append(makePdo(i % 16, i, pattern));
    return result;
}

QVector<SlaveInfo> TestDataGenerator::generateTopology(int depth, int breadth) {
    QVector<SlaveInfo> result;
    int pos = 0;
    for (int d = 0; d < depth; d++) {
        for (int b = 0; b < breadth; b++) {
            SlaveInfo info;
            info.position = pos++;
            info.name = QString("Branch%1_Slave%2").arg(d).arg(b);
            info.state = (d == 0 && b == 0) ? "OP" : "PREOP";
            info.flags = (b == 0) ? "-" : "";
            result.append(info);
        }
    }
    return result;
}

SlaveInfo TestDataGenerator::makeSlave(int position, DataPattern pattern) {
    SlaveInfo info;
    switch (pattern) {
        case DataPattern::Sequential:
            info.position = position;
            info.name = QString("Slave_%1").arg(position);
            info.state = "OP";
            info.flags = "-";
            break;
        case DataPattern::Random:
            info.position = QRandomGenerator::global()->bounded(0, 256);
            info.name = QString("Slave_%1").arg(info.position);
            info.state = (QRandomGenerator::global()->bounded(2) == 0) ? "OP" : "PREOP";
            break;
        case DataPattern::Boundary:
            info.position = (position == 0) ? 0 : (position == 1) ? 255 : position;
            info.name = (position == 0) ? QString() : QString("Slave_%1").arg(info.position);
            info.state = (position == 0) ? "INIT" : "OP";
            break;
        case DataPattern::Error:
            info.position = -1;
            info.name = "ERROR_SLAVE";
            info.state = "ERROR";
            info.flags = "E";
            info.rawLine = "Error: device not found";
            break;
    }
    return info;
}

SdoValue TestDataGenerator::makeSdo(int position, int index, DataPattern pattern) {
    SdoValue sdo;
    sdo.position = position;
    switch (pattern) {
        case DataPattern::Sequential:
            sdo.index = QString("0x%1").arg(0x6000 + index, 4, 16, QChar('0'));
            sdo.subIndex = "0x00";
            sdo.value = QString("0x%1").arg(index, 4, 16, QChar('0'));
            sdo.type = "UINT16";
            break;
        case DataPattern::Random:
            sdo.index = QString("0x%1").arg(QRandomGenerator::global()->bounded(0x1000, 0x9FFF), 4, 16, QChar('0'));
            sdo.subIndex = QString("0x%1").arg(QRandomGenerator::global()->bounded(0, 16), 2, 16, QChar('0'));
            sdo.value = QString("0x%1").arg(QRandomGenerator::global()->bounded(0, 0xFFFF), 4, 16, QChar('0'));
            sdo.type = "UINT16";
            break;
        case DataPattern::Boundary:
            sdo.index = (index == 0) ? "0x0000" : "0xffff";
            sdo.subIndex = "0x00";
            sdo.value = (index == 0) ? "0x0000" : "0xffff";
            sdo.type = "UINT16";
            break;
        case DataPattern::Error:
            sdo.index = "0xFFFF";
            sdo.subIndex = "0xFF";
            sdo.value = "";
            sdo.type = "UNKNOWN";
            break;
    }
    return sdo;
}

PdoMapping TestDataGenerator::makePdo(int position, int index, DataPattern pattern) {
    PdoMapping pdo;
    pdo.position = position;
    switch (pattern) {
        case DataPattern::Sequential:
            pdo.syncManager = (index % 2 == 0) ? "SM2" : "SM3";
            pdo.pdoIndex = QString("0x1%1").arg(index % 2 == 0 ? "600" : "A00");
            pdo.index = QString("0x%1").arg(0x6000 + index, 4, 16, QChar('0')).toUpper();
            pdo.subIndex = "0x00";
            pdo.bits = 16;
            pdo.name = QString("Var_%1").arg(index);
            break;
        case DataPattern::Random:
            pdo.syncManager = (QRandomGenerator::global()->bounded(2) == 0) ? "SM2" : "SM3";
            pdo.pdoIndex = QString("0x1%1").arg(QRandomGenerator::global()->bounded(600, 700), 3, 16, QChar('0'));
            pdo.index =
                QString("0x%1").arg(QRandomGenerator::global()->bounded(0x6000, 0x6FFF), 4, 16, QChar('0')).toUpper();
            pdo.subIndex = "0x00";
            pdo.bits = QRandomGenerator::global()->bounded(1, 3) * 8;
            pdo.name = QString("RandomVar_%1").arg(index);
            break;
        case DataPattern::Boundary:
            pdo.syncManager = "SM2";
            pdo.pdoIndex = "0x1600";
            pdo.index = (index == 0) ? "0x0000" : "0xFFFF";
            pdo.subIndex = "0x00";
            pdo.bits = (index == 0) ? 1 : 64;
            pdo.name = (index == 0) ? QString() : QString("BoundaryVar_%1").arg(index);
            break;
        case DataPattern::Error:
            pdo.syncManager = "INVALID";
            pdo.pdoIndex = "0x0000";
            pdo.index = "0x0000";
            pdo.subIndex = "0x00";
            pdo.bits = 0;
            pdo.name = "ERROR_PDO";
            break;
    }
    return pdo;
}
