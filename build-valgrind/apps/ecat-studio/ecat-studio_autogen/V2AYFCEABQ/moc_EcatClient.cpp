/****************************************************************************
** Meta object code from reading C++ file 'EcatClient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/infra/EcatClient.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EcatClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10EcatClientE_t {};
} // unnamed namespace

template <> constexpr inline auto EcatClient::qt_create_metaobjectdata<qt_meta_tag_ZN10EcatClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "EcatClient",
        "connected",
        "",
        "connectionStateChanged",
        "ConnectionState",
        "state",
        "disconnected",
        "errorMessage",
        "message",
        "daemonInfo",
        "text",
        "hostDiagnosticsReady",
        "QJsonArray",
        "checks",
        "masterText",
        "slavesChanged",
        "QList<SlaveInfo>",
        "slaves",
        "slaveTextResult",
        "title",
        "position",
        "sdoValue",
        "index",
        "subIndex",
        "value",
        "startupSdoResults",
        "results",
        "commandSucceeded",
        "freeRunChanged",
        "running",
        "status",
        "freeRunTelemetry",
        "QJsonObject",
        "telemetry",
        "rtTestTelemetry",
        "dcSyncStatusResult",
        "data",
        "alEventLogResult",
        "adaptersListResult",
        "reconnected",
        "attemptReconnect",
        "readSocket"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connected'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'connectionStateChanged'
        QtMocHelpers::SignalData<void(ConnectionState)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Signal 'disconnected'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'errorMessage'
        QtMocHelpers::SignalData<void(const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'daemonInfo'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Signal 'hostDiagnosticsReady'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
        // Signal 'masterText'
        QtMocHelpers::SignalData<void(const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Signal 'slavesChanged'
        QtMocHelpers::SignalData<void(const QVector<SlaveInfo> &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 16, 17 },
        }}),
        // Signal 'slaveTextResult'
        QtMocHelpers::SignalData<void(const QString &, int, const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 19 }, { QMetaType::Int, 20 }, { QMetaType::QString, 10 },
        }}),
        // Signal 'sdoValue'
        QtMocHelpers::SignalData<void(int, const QString &, const QString &, const QString &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 20 }, { QMetaType::QString, 22 }, { QMetaType::QString, 23 }, { QMetaType::QString, 24 },
        }}),
        // Signal 'startupSdoResults'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 12, 26 },
        }}),
        // Signal 'commandSucceeded'
        QtMocHelpers::SignalData<void(const QString &)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'freeRunChanged'
        QtMocHelpers::SignalData<void(bool, const QString &)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 29 }, { QMetaType::QString, 30 },
        }}),
        // Signal 'freeRunTelemetry'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(31, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 32, 33 },
        }}),
        // Signal 'rtTestTelemetry'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 32, 33 },
        }}),
        // Signal 'dcSyncStatusResult'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(35, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 32, 36 },
        }}),
        // Signal 'alEventLogResult'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(37, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 32, 36 },
        }}),
        // Signal 'adaptersListResult'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(38, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 32, 36 },
        }}),
        // Signal 'reconnected'
        QtMocHelpers::SignalData<void()>(39, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'attemptReconnect'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'readSocket'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<EcatClient, qt_meta_tag_ZN10EcatClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject EcatClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10EcatClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10EcatClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10EcatClientE_t>.metaTypes,
    nullptr
} };

void EcatClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<EcatClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->connectionStateChanged((*reinterpret_cast<std::add_pointer_t<ConnectionState>>(_a[1]))); break;
        case 2: _t->disconnected(); break;
        case 3: _t->errorMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->daemonInfo((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->hostDiagnosticsReady((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 6: _t->masterText((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->slavesChanged((*reinterpret_cast<std::add_pointer_t<QList<SlaveInfo>>>(_a[1]))); break;
        case 8: _t->slaveTextResult((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 9: _t->sdoValue((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4]))); break;
        case 10: _t->startupSdoResults((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 11: _t->commandSucceeded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->freeRunChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 13: _t->freeRunTelemetry((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 14: _t->rtTestTelemetry((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 15: _t->dcSyncStatusResult((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 16: _t->alEventLogResult((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 17: _t->adaptersListResult((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 18: _t->reconnected(); break;
        case 19: _t->attemptReconnect(); break;
        case 20: _t->readSocket(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<SlaveInfo> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)()>(_a, &EcatClient::connected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(ConnectionState )>(_a, &EcatClient::connectionStateChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)()>(_a, &EcatClient::disconnected, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QString & )>(_a, &EcatClient::errorMessage, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QString & )>(_a, &EcatClient::daemonInfo, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QJsonArray & )>(_a, &EcatClient::hostDiagnosticsReady, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QString & )>(_a, &EcatClient::masterText, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QVector<SlaveInfo> & )>(_a, &EcatClient::slavesChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QString & , int , const QString & )>(_a, &EcatClient::slaveTextResult, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(int , const QString & , const QString & , const QString & )>(_a, &EcatClient::sdoValue, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QJsonArray & )>(_a, &EcatClient::startupSdoResults, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QString & )>(_a, &EcatClient::commandSucceeded, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(bool , const QString & )>(_a, &EcatClient::freeRunChanged, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QJsonObject & )>(_a, &EcatClient::freeRunTelemetry, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QJsonObject & )>(_a, &EcatClient::rtTestTelemetry, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QJsonObject & )>(_a, &EcatClient::dcSyncStatusResult, 15))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QJsonObject & )>(_a, &EcatClient::alEventLogResult, 16))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)(const QJsonObject & )>(_a, &EcatClient::adaptersListResult, 17))
            return;
        if (QtMocHelpers::indexOfMethod<void (EcatClient::*)()>(_a, &EcatClient::reconnected, 18))
            return;
    }
}

const QMetaObject *EcatClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EcatClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10EcatClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EcatClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
    return _id;
}

// SIGNAL 0
void EcatClient::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void EcatClient::connectionStateChanged(ConnectionState _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void EcatClient::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void EcatClient::errorMessage(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void EcatClient::daemonInfo(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void EcatClient::hostDiagnosticsReady(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void EcatClient::masterText(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void EcatClient::slavesChanged(const QVector<SlaveInfo> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void EcatClient::slaveTextResult(const QString & _t1, int _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2, _t3);
}

// SIGNAL 9
void EcatClient::sdoValue(int _t1, const QString & _t2, const QString & _t3, const QString & _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 10
void EcatClient::startupSdoResults(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}

// SIGNAL 11
void EcatClient::commandSucceeded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void EcatClient::freeRunChanged(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1, _t2);
}

// SIGNAL 13
void EcatClient::freeRunTelemetry(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 13, nullptr, _t1);
}

// SIGNAL 14
void EcatClient::rtTestTelemetry(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1);
}

// SIGNAL 15
void EcatClient::dcSyncStatusResult(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 15, nullptr, _t1);
}

// SIGNAL 16
void EcatClient::alEventLogResult(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 16, nullptr, _t1);
}

// SIGNAL 17
void EcatClient::adaptersListResult(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 17, nullptr, _t1);
}

// SIGNAL 18
void EcatClient::reconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 18, nullptr);
}
QT_WARNING_POP
