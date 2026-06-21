/****************************************************************************
** Meta object code from reading C++ file 'WorkflowComplianceManagerService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/services/WorkflowComplianceManagerService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WorkflowComplianceManagerService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN32WorkflowComplianceManagerServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto WorkflowComplianceManagerService::qt_create_metaobjectdata<qt_meta_tag_ZN32WorkflowComplianceManagerServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "WorkflowComplianceManagerService",
        "ruleAdded",
        "",
        "ruleId",
        "ruleRemoved",
        "ruleActivated",
        "ruleDeactivated",
        "ruleAudited"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'ruleAdded'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'ruleRemoved'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'ruleActivated'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'ruleDeactivated'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'ruleAudited'
        QtMocHelpers::SignalData<void(const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<WorkflowComplianceManagerService, qt_meta_tag_ZN32WorkflowComplianceManagerServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject WorkflowComplianceManagerService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN32WorkflowComplianceManagerServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN32WorkflowComplianceManagerServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN32WorkflowComplianceManagerServiceE_t>.metaTypes,
    nullptr
} };

void WorkflowComplianceManagerService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WorkflowComplianceManagerService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->ruleAdded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->ruleRemoved((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->ruleActivated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->ruleDeactivated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->ruleAudited((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (WorkflowComplianceManagerService::*)(const QString & )>(_a, &WorkflowComplianceManagerService::ruleAdded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowComplianceManagerService::*)(const QString & )>(_a, &WorkflowComplianceManagerService::ruleRemoved, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowComplianceManagerService::*)(const QString & )>(_a, &WorkflowComplianceManagerService::ruleActivated, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowComplianceManagerService::*)(const QString & )>(_a, &WorkflowComplianceManagerService::ruleDeactivated, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowComplianceManagerService::*)(const QString & )>(_a, &WorkflowComplianceManagerService::ruleAudited, 4))
            return;
    }
}

const QMetaObject *WorkflowComplianceManagerService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkflowComplianceManagerService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN32WorkflowComplianceManagerServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WorkflowComplianceManagerService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void WorkflowComplianceManagerService::ruleAdded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void WorkflowComplianceManagerService::ruleRemoved(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void WorkflowComplianceManagerService::ruleActivated(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void WorkflowComplianceManagerService::ruleDeactivated(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void WorkflowComplianceManagerService::ruleAudited(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
