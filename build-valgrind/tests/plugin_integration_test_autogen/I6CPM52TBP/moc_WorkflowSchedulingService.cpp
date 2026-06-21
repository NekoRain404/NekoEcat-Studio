/****************************************************************************
** Meta object code from reading C++ file 'WorkflowSchedulingService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/services/WorkflowSchedulingService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WorkflowSchedulingService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN25WorkflowSchedulingServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto WorkflowSchedulingService::qt_create_metaobjectdata<qt_meta_tag_ZN25WorkflowSchedulingServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "WorkflowSchedulingService",
        "workflowScheduled",
        "",
        "WorkflowConfig",
        "config",
        "workflowTriggered",
        "workflowId",
        "workflowPaused",
        "workflowResumed",
        "workflowCompleted",
        "success"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'workflowScheduled'
        QtMocHelpers::SignalData<void(const WorkflowConfig &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'workflowTriggered'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'workflowPaused'
        QtMocHelpers::SignalData<void(const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'workflowResumed'
        QtMocHelpers::SignalData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'workflowCompleted'
        QtMocHelpers::SignalData<void(const QString &, bool)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::Bool, 10 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<WorkflowSchedulingService, qt_meta_tag_ZN25WorkflowSchedulingServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject WorkflowSchedulingService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25WorkflowSchedulingServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25WorkflowSchedulingServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN25WorkflowSchedulingServiceE_t>.metaTypes,
    nullptr
} };

void WorkflowSchedulingService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WorkflowSchedulingService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->workflowScheduled((*reinterpret_cast<std::add_pointer_t<WorkflowConfig>>(_a[1]))); break;
        case 1: _t->workflowTriggered((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->workflowPaused((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->workflowResumed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->workflowCompleted((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (WorkflowSchedulingService::*)(const WorkflowConfig & )>(_a, &WorkflowSchedulingService::workflowScheduled, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowSchedulingService::*)(const QString & )>(_a, &WorkflowSchedulingService::workflowTriggered, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowSchedulingService::*)(const QString & )>(_a, &WorkflowSchedulingService::workflowPaused, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowSchedulingService::*)(const QString & )>(_a, &WorkflowSchedulingService::workflowResumed, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowSchedulingService::*)(const QString & , bool )>(_a, &WorkflowSchedulingService::workflowCompleted, 4))
            return;
    }
}

const QMetaObject *WorkflowSchedulingService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkflowSchedulingService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25WorkflowSchedulingServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WorkflowSchedulingService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void WorkflowSchedulingService::workflowScheduled(const WorkflowConfig & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void WorkflowSchedulingService::workflowTriggered(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void WorkflowSchedulingService::workflowPaused(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void WorkflowSchedulingService::workflowResumed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void WorkflowSchedulingService::workflowCompleted(const QString & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}
QT_WARNING_POP
