/****************************************************************************
** Meta object code from reading C++ file 'WorkflowBlockchainService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/services/WorkflowBlockchainService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WorkflowBlockchainService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN25WorkflowBlockchainServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto WorkflowBlockchainService::qt_create_metaobjectdata<qt_meta_tag_ZN25WorkflowBlockchainServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "WorkflowBlockchainService",
        "transactionRecorded",
        "",
        "WfTransaction",
        "transaction",
        "smartContractExecuted",
        "WfSmartContract",
        "contract",
        "verificationCompleted",
        "transactionId",
        "valid"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'transactionRecorded'
        QtMocHelpers::SignalData<void(const WfTransaction &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'smartContractExecuted'
        QtMocHelpers::SignalData<void(const WfSmartContract &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'verificationCompleted'
        QtMocHelpers::SignalData<void(const QString &, bool)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 }, { QMetaType::Bool, 10 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<WorkflowBlockchainService, qt_meta_tag_ZN25WorkflowBlockchainServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject WorkflowBlockchainService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25WorkflowBlockchainServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25WorkflowBlockchainServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN25WorkflowBlockchainServiceE_t>.metaTypes,
    nullptr
} };

void WorkflowBlockchainService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WorkflowBlockchainService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->transactionRecorded((*reinterpret_cast<std::add_pointer_t<WfTransaction>>(_a[1]))); break;
        case 1: _t->smartContractExecuted((*reinterpret_cast<std::add_pointer_t<WfSmartContract>>(_a[1]))); break;
        case 2: _t->verificationCompleted((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (WorkflowBlockchainService::*)(const WfTransaction & )>(_a, &WorkflowBlockchainService::transactionRecorded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowBlockchainService::*)(const WfSmartContract & )>(_a, &WorkflowBlockchainService::smartContractExecuted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowBlockchainService::*)(const QString & , bool )>(_a, &WorkflowBlockchainService::verificationCompleted, 2))
            return;
    }
}

const QMetaObject *WorkflowBlockchainService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkflowBlockchainService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25WorkflowBlockchainServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WorkflowBlockchainService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void WorkflowBlockchainService::transactionRecorded(const WfTransaction & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void WorkflowBlockchainService::smartContractExecuted(const WfSmartContract & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void WorkflowBlockchainService::verificationCompleted(const QString & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}
QT_WARNING_POP
