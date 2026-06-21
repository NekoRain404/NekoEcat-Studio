/****************************************************************************
** Meta object code from reading C++ file 'CableDiagnosticsService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/services/CableDiagnosticsService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CableDiagnosticsService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN23CableDiagnosticsServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto CableDiagnosticsService::qt_create_metaobjectdata<qt_meta_tag_ZN23CableDiagnosticsServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CableDiagnosticsService",
        "testStarted",
        "",
        "portId",
        "testCompleted",
        "CableTestResult",
        "result",
        "diagnosticsCompleted",
        "CableDiagnosticsReport",
        "report"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'testStarted'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'testCompleted'
        QtMocHelpers::SignalData<void(int, const CableTestResult &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { 0x80000000 | 5, 6 },
        }}),
        // Signal 'diagnosticsCompleted'
        QtMocHelpers::SignalData<void(const CableDiagnosticsReport &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CableDiagnosticsService, qt_meta_tag_ZN23CableDiagnosticsServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CableDiagnosticsService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23CableDiagnosticsServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23CableDiagnosticsServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN23CableDiagnosticsServiceE_t>.metaTypes,
    nullptr
} };

void CableDiagnosticsService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CableDiagnosticsService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->testStarted((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->testCompleted((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<CableTestResult>>(_a[2]))); break;
        case 2: _t->diagnosticsCompleted((*reinterpret_cast<std::add_pointer_t<CableDiagnosticsReport>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (CableDiagnosticsService::*)(int )>(_a, &CableDiagnosticsService::testStarted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (CableDiagnosticsService::*)(int , const CableTestResult & )>(_a, &CableDiagnosticsService::testCompleted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (CableDiagnosticsService::*)(const CableDiagnosticsReport & )>(_a, &CableDiagnosticsService::diagnosticsCompleted, 2))
            return;
    }
}

const QMetaObject *CableDiagnosticsService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CableDiagnosticsService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23CableDiagnosticsServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CableDiagnosticsService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void CableDiagnosticsService::testStarted(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void CableDiagnosticsService::testCompleted(int _t1, const CableTestResult & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void CableDiagnosticsService::diagnosticsCompleted(const CableDiagnosticsReport & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
