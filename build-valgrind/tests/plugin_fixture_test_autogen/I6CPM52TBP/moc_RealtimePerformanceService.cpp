/****************************************************************************
** Meta object code from reading C++ file 'RealtimePerformanceService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/services/RealtimePerformanceService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RealtimePerformanceService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN26RealtimePerformanceServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto RealtimePerformanceService::qt_create_metaobjectdata<qt_meta_tag_ZN26RealtimePerformanceServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RealtimePerformanceService",
        "latencyUpdated",
        "",
        "LatencyMetrics",
        "metrics",
        "throughputUpdated",
        "ThroughputMetrics",
        "resourceUpdated",
        "ResourceMetrics",
        "qualityUpdated",
        "QualityAssessment",
        "quality",
        "monitoringStateChanged",
        "active"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'latencyUpdated'
        QtMocHelpers::SignalData<void(const LatencyMetrics &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'throughputUpdated'
        QtMocHelpers::SignalData<void(const ThroughputMetrics &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 4 },
        }}),
        // Signal 'resourceUpdated'
        QtMocHelpers::SignalData<void(const ResourceMetrics &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 4 },
        }}),
        // Signal 'qualityUpdated'
        QtMocHelpers::SignalData<void(const QualityAssessment &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Signal 'monitoringStateChanged'
        QtMocHelpers::SignalData<void(bool)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 13 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RealtimePerformanceService, qt_meta_tag_ZN26RealtimePerformanceServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RealtimePerformanceService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN26RealtimePerformanceServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN26RealtimePerformanceServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN26RealtimePerformanceServiceE_t>.metaTypes,
    nullptr
} };

void RealtimePerformanceService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RealtimePerformanceService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->latencyUpdated((*reinterpret_cast<std::add_pointer_t<LatencyMetrics>>(_a[1]))); break;
        case 1: _t->throughputUpdated((*reinterpret_cast<std::add_pointer_t<ThroughputMetrics>>(_a[1]))); break;
        case 2: _t->resourceUpdated((*reinterpret_cast<std::add_pointer_t<ResourceMetrics>>(_a[1]))); break;
        case 3: _t->qualityUpdated((*reinterpret_cast<std::add_pointer_t<QualityAssessment>>(_a[1]))); break;
        case 4: _t->monitoringStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RealtimePerformanceService::*)(const LatencyMetrics & )>(_a, &RealtimePerformanceService::latencyUpdated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RealtimePerformanceService::*)(const ThroughputMetrics & )>(_a, &RealtimePerformanceService::throughputUpdated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (RealtimePerformanceService::*)(const ResourceMetrics & )>(_a, &RealtimePerformanceService::resourceUpdated, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (RealtimePerformanceService::*)(const QualityAssessment & )>(_a, &RealtimePerformanceService::qualityUpdated, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (RealtimePerformanceService::*)(bool )>(_a, &RealtimePerformanceService::monitoringStateChanged, 4))
            return;
    }
}

const QMetaObject *RealtimePerformanceService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RealtimePerformanceService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN26RealtimePerformanceServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RealtimePerformanceService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void RealtimePerformanceService::latencyUpdated(const LatencyMetrics & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void RealtimePerformanceService::throughputUpdated(const ThroughputMetrics & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void RealtimePerformanceService::resourceUpdated(const ResourceMetrics & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void RealtimePerformanceService::qualityUpdated(const QualityAssessment & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void RealtimePerformanceService::monitoringStateChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
