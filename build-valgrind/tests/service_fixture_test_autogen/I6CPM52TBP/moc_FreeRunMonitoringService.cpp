/****************************************************************************
** Meta object code from reading C++ file 'FreeRunMonitoringService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/services/FreeRunMonitoringService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FreeRunMonitoringService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN24FreeRunMonitoringServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto FreeRunMonitoringService::qt_create_metaobjectdata<qt_meta_tag_ZN24FreeRunMonitoringServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FreeRunMonitoringService",
        "processDataUpdated",
        "",
        "FreeRunProcessData",
        "data",
        "performanceUpdated",
        "FreeRunPerformanceMetrics",
        "metrics",
        "errorOccurred",
        "FreeRunErrorInfo",
        "error",
        "statusChanged",
        "FreeRunStatus",
        "status",
        "monitoringStateChanged",
        "active",
        "pollData"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'processDataUpdated'
        QtMocHelpers::SignalData<void(const FreeRunProcessData &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'performanceUpdated'
        QtMocHelpers::SignalData<void(const FreeRunPerformanceMetrics &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const FreeRunErrorInfo &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Signal 'statusChanged'
        QtMocHelpers::SignalData<void(const FreeRunStatus &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
        // Signal 'monitoringStateChanged'
        QtMocHelpers::SignalData<void(bool)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 15 },
        }}),
        // Slot 'pollData'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FreeRunMonitoringService, qt_meta_tag_ZN24FreeRunMonitoringServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FreeRunMonitoringService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN24FreeRunMonitoringServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN24FreeRunMonitoringServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN24FreeRunMonitoringServiceE_t>.metaTypes,
    nullptr
} };

void FreeRunMonitoringService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FreeRunMonitoringService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->processDataUpdated((*reinterpret_cast<std::add_pointer_t<FreeRunProcessData>>(_a[1]))); break;
        case 1: _t->performanceUpdated((*reinterpret_cast<std::add_pointer_t<FreeRunPerformanceMetrics>>(_a[1]))); break;
        case 2: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<FreeRunErrorInfo>>(_a[1]))); break;
        case 3: _t->statusChanged((*reinterpret_cast<std::add_pointer_t<FreeRunStatus>>(_a[1]))); break;
        case 4: _t->monitoringStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->pollData(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (FreeRunMonitoringService::*)(const FreeRunProcessData & )>(_a, &FreeRunMonitoringService::processDataUpdated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (FreeRunMonitoringService::*)(const FreeRunPerformanceMetrics & )>(_a, &FreeRunMonitoringService::performanceUpdated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (FreeRunMonitoringService::*)(const FreeRunErrorInfo & )>(_a, &FreeRunMonitoringService::errorOccurred, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (FreeRunMonitoringService::*)(const FreeRunStatus & )>(_a, &FreeRunMonitoringService::statusChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (FreeRunMonitoringService::*)(bool )>(_a, &FreeRunMonitoringService::monitoringStateChanged, 4))
            return;
    }
}

const QMetaObject *FreeRunMonitoringService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FreeRunMonitoringService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN24FreeRunMonitoringServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FreeRunMonitoringService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void FreeRunMonitoringService::processDataUpdated(const FreeRunProcessData & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void FreeRunMonitoringService::performanceUpdated(const FreeRunPerformanceMetrics & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void FreeRunMonitoringService::errorOccurred(const FreeRunErrorInfo & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void FreeRunMonitoringService::statusChanged(const FreeRunStatus & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void FreeRunMonitoringService::monitoringStateChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
