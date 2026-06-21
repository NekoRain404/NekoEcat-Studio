/****************************************************************************
** Meta object code from reading C++ file 'DigitalTwinStudioPlugin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/plugins/digitaltwinstudio/DigitalTwinStudioPlugin.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DigitalTwinStudioPlugin.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN23DigitalTwinStudioPluginE_t {};
} // unnamed namespace

template <> constexpr inline auto DigitalTwinStudioPlugin::qt_create_metaobjectdata<qt_meta_tag_ZN23DigitalTwinStudioPluginE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DigitalTwinStudioPlugin",
        "modelAdded",
        "",
        "name",
        "syncStatusChanged",
        "deviceId",
        "status",
        "simulationStarted",
        "predictionUpdated",
        "metric",
        "value"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'modelAdded'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'syncStatusChanged'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 }, { QMetaType::QString, 6 },
        }}),
        // Signal 'simulationStarted'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'predictionUpdated'
        QtMocHelpers::SignalData<void(const QString &, double)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 }, { QMetaType::Double, 10 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DigitalTwinStudioPlugin, qt_meta_tag_ZN23DigitalTwinStudioPluginE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DigitalTwinStudioPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<WorkspacePlugin::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23DigitalTwinStudioPluginE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23DigitalTwinStudioPluginE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN23DigitalTwinStudioPluginE_t>.metaTypes,
    nullptr
} };

void DigitalTwinStudioPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DigitalTwinStudioPlugin *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->modelAdded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->syncStatusChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->simulationStarted(); break;
        case 3: _t->predictionUpdated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DigitalTwinStudioPlugin::*)(const QString & )>(_a, &DigitalTwinStudioPlugin::modelAdded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DigitalTwinStudioPlugin::*)(const QString & , const QString & )>(_a, &DigitalTwinStudioPlugin::syncStatusChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DigitalTwinStudioPlugin::*)()>(_a, &DigitalTwinStudioPlugin::simulationStarted, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DigitalTwinStudioPlugin::*)(const QString & , double )>(_a, &DigitalTwinStudioPlugin::predictionUpdated, 3))
            return;
    }
}

const QMetaObject *DigitalTwinStudioPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DigitalTwinStudioPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23DigitalTwinStudioPluginE_t>.strings))
        return static_cast<void*>(this);
    return WorkspacePlugin::qt_metacast(_clname);
}

int DigitalTwinStudioPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = WorkspacePlugin::qt_metacall(_c, _id, _a);
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
void DigitalTwinStudioPlugin::modelAdded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void DigitalTwinStudioPlugin::syncStatusChanged(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void DigitalTwinStudioPlugin::simulationStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void DigitalTwinStudioPlugin::predictionUpdated(const QString & _t1, double _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}
QT_WARNING_POP
