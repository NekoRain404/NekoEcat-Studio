/****************************************************************************
** Meta object code from reading C++ file 'ResourceManagementService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/services/ResourceManagementService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ResourceManagementService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN25ResourceManagementServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto ResourceManagementService::qt_create_metaobjectdata<qt_meta_tag_ZN25ResourceManagementServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ResourceManagementService",
        "resourceAllocated",
        "",
        "Resource",
        "resource",
        "resourceUpdated",
        "ResourceStatusInfo",
        "status"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'resourceAllocated'
        QtMocHelpers::SignalData<void(const Resource &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'resourceUpdated'
        QtMocHelpers::SignalData<void(const ResourceStatusInfo &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ResourceManagementService, qt_meta_tag_ZN25ResourceManagementServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ResourceManagementService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25ResourceManagementServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25ResourceManagementServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN25ResourceManagementServiceE_t>.metaTypes,
    nullptr
} };

void ResourceManagementService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ResourceManagementService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->resourceAllocated((*reinterpret_cast<std::add_pointer_t<Resource>>(_a[1]))); break;
        case 1: _t->resourceUpdated((*reinterpret_cast<std::add_pointer_t<ResourceStatusInfo>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ResourceManagementService::*)(const Resource & )>(_a, &ResourceManagementService::resourceAllocated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ResourceManagementService::*)(const ResourceStatusInfo & )>(_a, &ResourceManagementService::resourceUpdated, 1))
            return;
    }
}

const QMetaObject *ResourceManagementService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ResourceManagementService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25ResourceManagementServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ResourceManagementService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ResourceManagementService::resourceAllocated(const Resource & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void ResourceManagementService::resourceUpdated(const ResourceStatusInfo & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
