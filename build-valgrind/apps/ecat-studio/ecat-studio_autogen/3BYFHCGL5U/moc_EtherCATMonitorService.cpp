/****************************************************************************
** Meta object code from reading C++ file 'EtherCATMonitorService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/services/EtherCATMonitorService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EtherCATMonitorService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN22EtherCATMonitorServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto EtherCATMonitorService::qt_create_metaobjectdata<qt_meta_tag_ZN22EtherCATMonitorServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "EtherCATMonitorService",
        "trafficUpdated",
        "",
        "BusTraffic",
        "traffic",
        "errorRateUpdated",
        "ErrorRate",
        "rate",
        "performanceUpdated",
        "PerformanceMetrics",
        "metrics",
        "healthUpdated",
        "HealthStatus",
        "health"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'trafficUpdated'
        QtMocHelpers::SignalData<void(const BusTraffic &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'errorRateUpdated'
        QtMocHelpers::SignalData<void(const ErrorRate &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'performanceUpdated'
        QtMocHelpers::SignalData<void(const PerformanceMetrics &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Signal 'healthUpdated'
        QtMocHelpers::SignalData<void(const HealthStatus &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<EtherCATMonitorService, qt_meta_tag_ZN22EtherCATMonitorServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject EtherCATMonitorService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22EtherCATMonitorServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22EtherCATMonitorServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN22EtherCATMonitorServiceE_t>.metaTypes,
    nullptr
} };

void EtherCATMonitorService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<EtherCATMonitorService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->trafficUpdated((*reinterpret_cast<std::add_pointer_t<BusTraffic>>(_a[1]))); break;
        case 1: _t->errorRateUpdated((*reinterpret_cast<std::add_pointer_t<ErrorRate>>(_a[1]))); break;
        case 2: _t->performanceUpdated((*reinterpret_cast<std::add_pointer_t<PerformanceMetrics>>(_a[1]))); break;
        case 3: _t->healthUpdated((*reinterpret_cast<std::add_pointer_t<HealthStatus>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (EtherCATMonitorService::*)(const BusTraffic & )>(_a, &EtherCATMonitorService::trafficUpdated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATMonitorService::*)(const ErrorRate & )>(_a, &EtherCATMonitorService::errorRateUpdated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATMonitorService::*)(const PerformanceMetrics & )>(_a, &EtherCATMonitorService::performanceUpdated, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATMonitorService::*)(const HealthStatus & )>(_a, &EtherCATMonitorService::healthUpdated, 3))
            return;
    }
}

const QMetaObject *EtherCATMonitorService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EtherCATMonitorService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22EtherCATMonitorServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EtherCATMonitorService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void EtherCATMonitorService::trafficUpdated(const BusTraffic & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void EtherCATMonitorService::errorRateUpdated(const ErrorRate & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void EtherCATMonitorService::performanceUpdated(const PerformanceMetrics & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void EtherCATMonitorService::healthUpdated(const HealthStatus & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
