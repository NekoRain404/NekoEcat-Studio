/****************************************************************************
** Meta object code from reading C++ file 'WorkflowSecurityManagerService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/services/WorkflowSecurityManagerService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WorkflowSecurityManagerService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN30WorkflowSecurityManagerServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto WorkflowSecurityManagerService::qt_create_metaobjectdata<qt_meta_tag_ZN30WorkflowSecurityManagerServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "WorkflowSecurityManagerService",
        "policyAdded",
        "",
        "policyId",
        "policyRemoved",
        "policyEnabled",
        "policyDisabled",
        "policyEnforced"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'policyAdded'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'policyRemoved'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'policyEnabled'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'policyDisabled'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'policyEnforced'
        QtMocHelpers::SignalData<void(const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<WorkflowSecurityManagerService, qt_meta_tag_ZN30WorkflowSecurityManagerServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject WorkflowSecurityManagerService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN30WorkflowSecurityManagerServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN30WorkflowSecurityManagerServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN30WorkflowSecurityManagerServiceE_t>.metaTypes,
    nullptr
} };

void WorkflowSecurityManagerService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WorkflowSecurityManagerService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->policyAdded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->policyRemoved((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->policyEnabled((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->policyDisabled((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->policyEnforced((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (WorkflowSecurityManagerService::*)(const QString & )>(_a, &WorkflowSecurityManagerService::policyAdded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowSecurityManagerService::*)(const QString & )>(_a, &WorkflowSecurityManagerService::policyRemoved, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowSecurityManagerService::*)(const QString & )>(_a, &WorkflowSecurityManagerService::policyEnabled, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowSecurityManagerService::*)(const QString & )>(_a, &WorkflowSecurityManagerService::policyDisabled, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowSecurityManagerService::*)(const QString & )>(_a, &WorkflowSecurityManagerService::policyEnforced, 4))
            return;
    }
}

const QMetaObject *WorkflowSecurityManagerService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkflowSecurityManagerService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN30WorkflowSecurityManagerServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WorkflowSecurityManagerService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void WorkflowSecurityManagerService::policyAdded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void WorkflowSecurityManagerService::policyRemoved(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void WorkflowSecurityManagerService::policyEnabled(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void WorkflowSecurityManagerService::policyDisabled(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void WorkflowSecurityManagerService::policyEnforced(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
