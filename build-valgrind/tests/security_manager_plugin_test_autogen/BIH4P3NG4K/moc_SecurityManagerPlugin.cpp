/****************************************************************************
** Meta object code from reading C++ file 'SecurityManagerPlugin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/plugins/security/SecurityManagerPlugin.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SecurityManagerPlugin.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN21SecurityManagerPluginE_t {};
} // unnamed namespace

template <> constexpr inline auto SecurityManagerPlugin::qt_create_metaobjectdata<qt_meta_tag_ZN21SecurityManagerPluginE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SecurityManagerPlugin",
        "auditStateChanged",
        "",
        "SecurityManagerPlugin::AuditState",
        "state",
        "policyChanged",
        "SecurityPolicy",
        "policy",
        "auditCompleted",
        "runAudit",
        "applyPolicy"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'auditStateChanged'
        QtMocHelpers::SignalData<void(SecurityManagerPlugin::AuditState)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'policyChanged'
        QtMocHelpers::SignalData<void(const SecurityPolicy &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'auditCompleted'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'runAudit'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'applyPolicy'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SecurityManagerPlugin, qt_meta_tag_ZN21SecurityManagerPluginE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SecurityManagerPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<WorkspacePlugin::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21SecurityManagerPluginE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21SecurityManagerPluginE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN21SecurityManagerPluginE_t>.metaTypes,
    nullptr
} };

void SecurityManagerPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SecurityManagerPlugin *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->auditStateChanged((*reinterpret_cast<std::add_pointer_t<SecurityManagerPlugin::AuditState>>(_a[1]))); break;
        case 1: _t->policyChanged((*reinterpret_cast<std::add_pointer_t<SecurityPolicy>>(_a[1]))); break;
        case 2: _t->auditCompleted(); break;
        case 3: _t->runAudit(); break;
        case 4: _t->applyPolicy(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SecurityManagerPlugin::*)(SecurityManagerPlugin::AuditState )>(_a, &SecurityManagerPlugin::auditStateChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SecurityManagerPlugin::*)(const SecurityPolicy & )>(_a, &SecurityManagerPlugin::policyChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SecurityManagerPlugin::*)()>(_a, &SecurityManagerPlugin::auditCompleted, 2))
            return;
    }
}

const QMetaObject *SecurityManagerPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SecurityManagerPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21SecurityManagerPluginE_t>.strings))
        return static_cast<void*>(this);
    return WorkspacePlugin::qt_metacast(_clname);
}

int SecurityManagerPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = WorkspacePlugin::qt_metacall(_c, _id, _a);
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
void SecurityManagerPlugin::auditStateChanged(SecurityManagerPlugin::AuditState _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void SecurityManagerPlugin::policyChanged(const SecurityPolicy & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void SecurityManagerPlugin::auditCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
