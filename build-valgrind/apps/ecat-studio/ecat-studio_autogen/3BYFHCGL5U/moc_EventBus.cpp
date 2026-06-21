/****************************************************************************
** Meta object code from reading C++ file 'EventBus.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/services/EventBus.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EventBus.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN8EventBusE_t {};
} // unnamed namespace

template <> constexpr inline auto EventBus::qt_create_metaobjectdata<qt_meta_tag_ZN8EventBusE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "EventBus",
        "slaveChanged",
        "",
        "QList<SlaveInfo>",
        "slaves",
        "sdoValueReceived",
        "position",
        "index",
        "subIndex",
        "value",
        "connectionStateChanged",
        "connected",
        "freeRunTelemetry",
        "QJsonObject",
        "telemetry",
        "topologyChanged",
        "dcSyncUpdate",
        "data",
        "alEvent",
        "event",
        "signalData",
        "channel",
        "QList<double>",
        "values",
        "QList<qint64>",
        "timestamps"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'slaveChanged'
        QtMocHelpers::SignalData<void(const QVector<SlaveInfo> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'sdoValueReceived'
        QtMocHelpers::SignalData<void(int, const QString &, const QString &, const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 }, { QMetaType::QString, 7 }, { QMetaType::QString, 8 }, { QMetaType::QString, 9 },
        }}),
        // Signal 'connectionStateChanged'
        QtMocHelpers::SignalData<void(bool)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 11 },
        }}),
        // Signal 'freeRunTelemetry'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
        // Signal 'topologyChanged'
        QtMocHelpers::SignalData<void(const QVector<SlaveInfo> &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'dcSyncUpdate'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 17 },
        }}),
        // Signal 'alEvent'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 19 },
        }}),
        // Signal 'signalData'
        QtMocHelpers::SignalData<void(int, const QVector<double> &, const QVector<qint64> &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 21 }, { 0x80000000 | 22, 23 }, { 0x80000000 | 24, 25 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<EventBus, qt_meta_tag_ZN8EventBusE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject EventBus::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8EventBusE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8EventBusE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8EventBusE_t>.metaTypes,
    nullptr
} };

void EventBus::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<EventBus *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->slaveChanged((*reinterpret_cast<std::add_pointer_t<QList<SlaveInfo>>>(_a[1]))); break;
        case 1: _t->sdoValueReceived((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4]))); break;
        case 2: _t->connectionStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->freeRunTelemetry((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 4: _t->topologyChanged((*reinterpret_cast<std::add_pointer_t<QList<SlaveInfo>>>(_a[1]))); break;
        case 5: _t->dcSyncUpdate((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 6: _t->alEvent((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 7: _t->signalData((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QList<double>>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QList<qint64>>>(_a[3]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<SlaveInfo> >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<SlaveInfo> >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            case 2:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<qint64> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (EventBus::*)(const QVector<SlaveInfo> & )>(_a, &EventBus::slaveChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (EventBus::*)(int , const QString & , const QString & , const QString & )>(_a, &EventBus::sdoValueReceived, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (EventBus::*)(bool )>(_a, &EventBus::connectionStateChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (EventBus::*)(const QJsonObject & )>(_a, &EventBus::freeRunTelemetry, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (EventBus::*)(const QVector<SlaveInfo> & )>(_a, &EventBus::topologyChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (EventBus::*)(const QJsonObject & )>(_a, &EventBus::dcSyncUpdate, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (EventBus::*)(const QJsonObject & )>(_a, &EventBus::alEvent, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (EventBus::*)(int , const QVector<double> & , const QVector<qint64> & )>(_a, &EventBus::signalData, 7))
            return;
    }
}

const QMetaObject *EventBus::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EventBus::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8EventBusE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EventBus::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void EventBus::slaveChanged(const QVector<SlaveInfo> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void EventBus::sdoValueReceived(int _t1, const QString & _t2, const QString & _t3, const QString & _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 2
void EventBus::connectionStateChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void EventBus::freeRunTelemetry(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void EventBus::topologyChanged(const QVector<SlaveInfo> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void EventBus::dcSyncUpdate(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void EventBus::alEvent(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void EventBus::signalData(int _t1, const QVector<double> & _t2, const QVector<qint64> & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2, _t3);
}
QT_WARNING_POP
