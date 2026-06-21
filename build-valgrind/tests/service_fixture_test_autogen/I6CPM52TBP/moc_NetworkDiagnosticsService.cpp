/****************************************************************************
** Meta object code from reading C++ file 'NetworkDiagnosticsService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/services/NetworkDiagnosticsService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkDiagnosticsService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN25NetworkDiagnosticsServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto NetworkDiagnosticsService::qt_create_metaobjectdata<qt_meta_tag_ZN25NetworkDiagnosticsServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NetworkDiagnosticsService",
        "healthUpdated",
        "",
        "NetworkHealth",
        "health",
        "portStatusChanged",
        "port",
        "PortStatus",
        "status",
        "errorDetected",
        "ErrorInfo",
        "error"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'healthUpdated'
        QtMocHelpers::SignalData<void(const NetworkHealth &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'portStatusChanged'
        QtMocHelpers::SignalData<void(int, const PortStatus &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 }, { 0x80000000 | 7, 8 },
        }}),
        // Signal 'errorDetected'
        QtMocHelpers::SignalData<void(const ErrorInfo &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NetworkDiagnosticsService, qt_meta_tag_ZN25NetworkDiagnosticsServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NetworkDiagnosticsService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25NetworkDiagnosticsServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25NetworkDiagnosticsServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN25NetworkDiagnosticsServiceE_t>.metaTypes,
    nullptr
} };

void NetworkDiagnosticsService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NetworkDiagnosticsService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->healthUpdated((*reinterpret_cast<std::add_pointer_t<NetworkHealth>>(_a[1]))); break;
        case 1: _t->portStatusChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<PortStatus>>(_a[2]))); break;
        case 2: _t->errorDetected((*reinterpret_cast<std::add_pointer_t<ErrorInfo>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (NetworkDiagnosticsService::*)(const NetworkHealth & )>(_a, &NetworkDiagnosticsService::healthUpdated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkDiagnosticsService::*)(int , const PortStatus & )>(_a, &NetworkDiagnosticsService::portStatusChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkDiagnosticsService::*)(const ErrorInfo & )>(_a, &NetworkDiagnosticsService::errorDetected, 2))
            return;
    }
}

const QMetaObject *NetworkDiagnosticsService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkDiagnosticsService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25NetworkDiagnosticsServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NetworkDiagnosticsService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void NetworkDiagnosticsService::healthUpdated(const NetworkHealth & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void NetworkDiagnosticsService::portStatusChanged(int _t1, const PortStatus & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void NetworkDiagnosticsService::errorDetected(const ErrorInfo & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
