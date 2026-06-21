/****************************************************************************
** Meta object code from reading C++ file 'MultiMasterService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/services/MultiMasterService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MultiMasterService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN18MultiMasterServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto MultiMasterService::qt_create_metaobjectdata<qt_meta_tag_ZN18MultiMasterServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MultiMasterService",
        "masterDiscovered",
        "",
        "MmMasterInfo",
        "info",
        "masterStatusChanged",
        "masterId",
        "MmMasterStatus",
        "status",
        "masterSyncCompleted",
        "MmMasterSyncResult",
        "result",
        "masterError",
        "error"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'masterDiscovered'
        QtMocHelpers::SignalData<void(const MmMasterInfo &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'masterStatusChanged'
        QtMocHelpers::SignalData<void(int, const MmMasterStatus &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 }, { 0x80000000 | 7, 8 },
        }}),
        // Signal 'masterSyncCompleted'
        QtMocHelpers::SignalData<void(const MmMasterSyncResult &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Signal 'masterError'
        QtMocHelpers::SignalData<void(int, const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 }, { QMetaType::QString, 13 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MultiMasterService, qt_meta_tag_ZN18MultiMasterServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MultiMasterService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18MultiMasterServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18MultiMasterServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN18MultiMasterServiceE_t>.metaTypes,
    nullptr
} };

void MultiMasterService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MultiMasterService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->masterDiscovered((*reinterpret_cast<std::add_pointer_t<MmMasterInfo>>(_a[1]))); break;
        case 1: _t->masterStatusChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<MmMasterStatus>>(_a[2]))); break;
        case 2: _t->masterSyncCompleted((*reinterpret_cast<std::add_pointer_t<MmMasterSyncResult>>(_a[1]))); break;
        case 3: _t->masterError((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MultiMasterService::*)(const MmMasterInfo & )>(_a, &MultiMasterService::masterDiscovered, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MultiMasterService::*)(int , const MmMasterStatus & )>(_a, &MultiMasterService::masterStatusChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MultiMasterService::*)(const MmMasterSyncResult & )>(_a, &MultiMasterService::masterSyncCompleted, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MultiMasterService::*)(int , const QString & )>(_a, &MultiMasterService::masterError, 3))
            return;
    }
}

const QMetaObject *MultiMasterService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MultiMasterService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18MultiMasterServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MultiMasterService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void MultiMasterService::masterDiscovered(const MmMasterInfo & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void MultiMasterService::masterStatusChanged(int _t1, const MmMasterStatus & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void MultiMasterService::masterSyncCompleted(const MmMasterSyncResult & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void MultiMasterService::masterError(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}
QT_WARNING_POP
