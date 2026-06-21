/****************************************************************************
** Meta object code from reading C++ file 'EtherCATCertificationService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/services/EtherCATCertificationService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EtherCATCertificationService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN28EtherCATCertificationServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto EtherCATCertificationService::qt_create_metaobjectdata<qt_meta_tag_ZN28EtherCATCertificationServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "EtherCATCertificationService",
        "requirementAdded",
        "",
        "requirementRemoved",
        "certificationCompleted",
        "CertificationReport",
        "report",
        "deviceCertified",
        "CertificationResult",
        "result"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'requirementAdded'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requirementRemoved'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'certificationCompleted'
        QtMocHelpers::SignalData<void(const CertificationReport &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'deviceCertified'
        QtMocHelpers::SignalData<void(const CertificationResult &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<EtherCATCertificationService, qt_meta_tag_ZN28EtherCATCertificationServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject EtherCATCertificationService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN28EtherCATCertificationServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN28EtherCATCertificationServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN28EtherCATCertificationServiceE_t>.metaTypes,
    nullptr
} };

void EtherCATCertificationService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<EtherCATCertificationService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->requirementAdded(); break;
        case 1: _t->requirementRemoved(); break;
        case 2: _t->certificationCompleted((*reinterpret_cast<std::add_pointer_t<CertificationReport>>(_a[1]))); break;
        case 3: _t->deviceCertified((*reinterpret_cast<std::add_pointer_t<CertificationResult>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (EtherCATCertificationService::*)()>(_a, &EtherCATCertificationService::requirementAdded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATCertificationService::*)()>(_a, &EtherCATCertificationService::requirementRemoved, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATCertificationService::*)(const CertificationReport & )>(_a, &EtherCATCertificationService::certificationCompleted, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATCertificationService::*)(const CertificationResult & )>(_a, &EtherCATCertificationService::deviceCertified, 3))
            return;
    }
}

const QMetaObject *EtherCATCertificationService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EtherCATCertificationService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN28EtherCATCertificationServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EtherCATCertificationService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void EtherCATCertificationService::requirementAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void EtherCATCertificationService::requirementRemoved()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void EtherCATCertificationService::certificationCompleted(const CertificationReport & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void EtherCATCertificationService::deviceCertified(const CertificationResult & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
