/****************************************************************************
** Meta object code from reading C++ file 'WorkflowDigitalTwinService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/services/WorkflowDigitalTwinService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WorkflowDigitalTwinService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN26WorkflowDigitalTwinServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto WorkflowDigitalTwinService::qt_create_metaobjectdata<qt_meta_tag_ZN26WorkflowDigitalTwinServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "WorkflowDigitalTwinService",
        "digitalTwinCreated",
        "",
        "position",
        "syncCompleted",
        "success",
        "simulationFinished",
        "WfTwinSimulationResult",
        "result"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'digitalTwinCreated'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'syncCompleted'
        QtMocHelpers::SignalData<void(int, bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::Bool, 5 },
        }}),
        // Signal 'simulationFinished'
        QtMocHelpers::SignalData<void(const WfTwinSimulationResult &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<WorkflowDigitalTwinService, qt_meta_tag_ZN26WorkflowDigitalTwinServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject WorkflowDigitalTwinService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN26WorkflowDigitalTwinServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN26WorkflowDigitalTwinServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN26WorkflowDigitalTwinServiceE_t>.metaTypes,
    nullptr
} };

void WorkflowDigitalTwinService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WorkflowDigitalTwinService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->digitalTwinCreated((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->syncCompleted((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 2: _t->simulationFinished((*reinterpret_cast<std::add_pointer_t<WfTwinSimulationResult>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (WorkflowDigitalTwinService::*)(int )>(_a, &WorkflowDigitalTwinService::digitalTwinCreated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowDigitalTwinService::*)(int , bool )>(_a, &WorkflowDigitalTwinService::syncCompleted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowDigitalTwinService::*)(const WfTwinSimulationResult & )>(_a, &WorkflowDigitalTwinService::simulationFinished, 2))
            return;
    }
}

const QMetaObject *WorkflowDigitalTwinService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkflowDigitalTwinService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN26WorkflowDigitalTwinServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WorkflowDigitalTwinService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void WorkflowDigitalTwinService::digitalTwinCreated(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void WorkflowDigitalTwinService::syncCompleted(int _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void WorkflowDigitalTwinService::simulationFinished(const WfTwinSimulationResult & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
