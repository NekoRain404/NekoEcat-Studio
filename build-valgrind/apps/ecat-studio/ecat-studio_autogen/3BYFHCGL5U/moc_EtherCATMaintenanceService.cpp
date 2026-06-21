/****************************************************************************
** Meta object code from reading C++ file 'EtherCATMaintenanceService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/services/EtherCATMaintenanceService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EtherCATMaintenanceService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN26EtherCATMaintenanceServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto EtherCATMaintenanceService::qt_create_metaobjectdata<qt_meta_tag_ZN26EtherCATMaintenanceServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "EtherCATMaintenanceService",
        "taskCompleted",
        "",
        "MaintenanceTaskInfo",
        "task",
        "maintenanceScheduled",
        "ScheduledMaintenanceTask",
        "maintenanceCompleted",
        "MaintenanceExecutionRecord",
        "record"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'taskCompleted'
        QtMocHelpers::SignalData<void(const MaintenanceTaskInfo &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'maintenanceScheduled'
        QtMocHelpers::SignalData<void(const ScheduledMaintenanceTask &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 4 },
        }}),
        // Signal 'maintenanceCompleted'
        QtMocHelpers::SignalData<void(const MaintenanceExecutionRecord &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<EtherCATMaintenanceService, qt_meta_tag_ZN26EtherCATMaintenanceServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject EtherCATMaintenanceService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN26EtherCATMaintenanceServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN26EtherCATMaintenanceServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN26EtherCATMaintenanceServiceE_t>.metaTypes,
    nullptr
} };

void EtherCATMaintenanceService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<EtherCATMaintenanceService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->taskCompleted((*reinterpret_cast<std::add_pointer_t<MaintenanceTaskInfo>>(_a[1]))); break;
        case 1: _t->maintenanceScheduled((*reinterpret_cast<std::add_pointer_t<ScheduledMaintenanceTask>>(_a[1]))); break;
        case 2: _t->maintenanceCompleted((*reinterpret_cast<std::add_pointer_t<MaintenanceExecutionRecord>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (EtherCATMaintenanceService::*)(const MaintenanceTaskInfo & )>(_a, &EtherCATMaintenanceService::taskCompleted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATMaintenanceService::*)(const ScheduledMaintenanceTask & )>(_a, &EtherCATMaintenanceService::maintenanceScheduled, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATMaintenanceService::*)(const MaintenanceExecutionRecord & )>(_a, &EtherCATMaintenanceService::maintenanceCompleted, 2))
            return;
    }
}

const QMetaObject *EtherCATMaintenanceService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EtherCATMaintenanceService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN26EtherCATMaintenanceServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EtherCATMaintenanceService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void EtherCATMaintenanceService::taskCompleted(const MaintenanceTaskInfo & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void EtherCATMaintenanceService::maintenanceScheduled(const ScheduledMaintenanceTask & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void EtherCATMaintenanceService::maintenanceCompleted(const MaintenanceExecutionRecord & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
