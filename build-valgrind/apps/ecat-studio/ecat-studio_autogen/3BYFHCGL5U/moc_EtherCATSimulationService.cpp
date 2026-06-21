/****************************************************************************
** Meta object code from reading C++ file 'EtherCATSimulationService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/services/EtherCATSimulationService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EtherCATSimulationService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN25EtherCATSimulationServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto EtherCATSimulationService::qt_create_metaobjectdata<qt_meta_tag_ZN25EtherCATSimulationServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "EtherCATSimulationService",
        "simulationStarted",
        "",
        "simulationStopped",
        "virtualSlaveCreated",
        "position",
        "virtualSlaveRemoved",
        "simulationStateChanged",
        "SimulationState",
        "state"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'simulationStarted'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'simulationStopped'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'virtualSlaveCreated'
        QtMocHelpers::SignalData<void(int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Signal 'virtualSlaveRemoved'
        QtMocHelpers::SignalData<void(int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Signal 'simulationStateChanged'
        QtMocHelpers::SignalData<void(const SimulationState &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<EtherCATSimulationService, qt_meta_tag_ZN25EtherCATSimulationServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject EtherCATSimulationService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25EtherCATSimulationServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25EtherCATSimulationServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN25EtherCATSimulationServiceE_t>.metaTypes,
    nullptr
} };

void EtherCATSimulationService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<EtherCATSimulationService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->simulationStarted(); break;
        case 1: _t->simulationStopped(); break;
        case 2: _t->virtualSlaveCreated((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->virtualSlaveRemoved((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->simulationStateChanged((*reinterpret_cast<std::add_pointer_t<SimulationState>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (EtherCATSimulationService::*)()>(_a, &EtherCATSimulationService::simulationStarted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATSimulationService::*)()>(_a, &EtherCATSimulationService::simulationStopped, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATSimulationService::*)(int )>(_a, &EtherCATSimulationService::virtualSlaveCreated, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATSimulationService::*)(int )>(_a, &EtherCATSimulationService::virtualSlaveRemoved, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATSimulationService::*)(const SimulationState & )>(_a, &EtherCATSimulationService::simulationStateChanged, 4))
            return;
    }
}

const QMetaObject *EtherCATSimulationService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EtherCATSimulationService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25EtherCATSimulationServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EtherCATSimulationService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void EtherCATSimulationService::simulationStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void EtherCATSimulationService::simulationStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void EtherCATSimulationService::virtualSlaveCreated(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void EtherCATSimulationService::virtualSlaveRemoved(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void EtherCATSimulationService::simulationStateChanged(const SimulationState & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
