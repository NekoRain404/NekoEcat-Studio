/****************************************************************************
** Meta object code from reading C++ file 'EtherCATSecurityService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/services/EtherCATSecurityService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EtherCATSecurityService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN23EtherCATSecurityServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto EtherCATSecurityService::qt_create_metaobjectdata<qt_meta_tag_ZN23EtherCATSecurityServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "EtherCATSecurityService",
        "policyChanged",
        "",
        "auditCompleted",
        "SecurityAuditResult",
        "result",
        "accessViolation",
        "SecurityAuditEntry",
        "entry"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'policyChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'auditCompleted'
        QtMocHelpers::SignalData<void(const SecurityAuditResult &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Signal 'accessViolation'
        QtMocHelpers::SignalData<void(const SecurityAuditEntry &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<EtherCATSecurityService, qt_meta_tag_ZN23EtherCATSecurityServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject EtherCATSecurityService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23EtherCATSecurityServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23EtherCATSecurityServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN23EtherCATSecurityServiceE_t>.metaTypes,
    nullptr
} };

void EtherCATSecurityService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<EtherCATSecurityService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->policyChanged(); break;
        case 1: _t->auditCompleted((*reinterpret_cast<std::add_pointer_t<SecurityAuditResult>>(_a[1]))); break;
        case 2: _t->accessViolation((*reinterpret_cast<std::add_pointer_t<SecurityAuditEntry>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (EtherCATSecurityService::*)()>(_a, &EtherCATSecurityService::policyChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATSecurityService::*)(const SecurityAuditResult & )>(_a, &EtherCATSecurityService::auditCompleted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATSecurityService::*)(const SecurityAuditEntry & )>(_a, &EtherCATSecurityService::accessViolation, 2))
            return;
    }
}

const QMetaObject *EtherCATSecurityService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EtherCATSecurityService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23EtherCATSecurityServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EtherCATSecurityService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void EtherCATSecurityService::policyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void EtherCATSecurityService::auditCompleted(const SecurityAuditResult & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void EtherCATSecurityService::accessViolation(const SecurityAuditEntry & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
