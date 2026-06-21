/****************************************************************************
** Meta object code from reading C++ file 'WorkflowEdgeService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/services/WorkflowEdgeService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WorkflowEdgeService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN19WorkflowEdgeServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto WorkflowEdgeService::qt_create_metaobjectdata<qt_meta_tag_ZN19WorkflowEdgeServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "WorkflowEdgeService",
        "edgeProcessed",
        "",
        "WfEdgeResult",
        "result",
        "edgeAnalyzed",
        "WfEdgeAnalysis",
        "analysis"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'edgeProcessed'
        QtMocHelpers::SignalData<void(const WfEdgeResult &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'edgeAnalyzed'
        QtMocHelpers::SignalData<void(const WfEdgeAnalysis &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<WorkflowEdgeService, qt_meta_tag_ZN19WorkflowEdgeServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject WorkflowEdgeService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19WorkflowEdgeServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19WorkflowEdgeServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN19WorkflowEdgeServiceE_t>.metaTypes,
    nullptr
} };

void WorkflowEdgeService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WorkflowEdgeService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->edgeProcessed((*reinterpret_cast<std::add_pointer_t<WfEdgeResult>>(_a[1]))); break;
        case 1: _t->edgeAnalyzed((*reinterpret_cast<std::add_pointer_t<WfEdgeAnalysis>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (WorkflowEdgeService::*)(const WfEdgeResult & )>(_a, &WorkflowEdgeService::edgeProcessed, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowEdgeService::*)(const WfEdgeAnalysis & )>(_a, &WorkflowEdgeService::edgeAnalyzed, 1))
            return;
    }
}

const QMetaObject *WorkflowEdgeService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkflowEdgeService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19WorkflowEdgeServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WorkflowEdgeService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void WorkflowEdgeService::edgeProcessed(const WfEdgeResult & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void WorkflowEdgeService::edgeAnalyzed(const WfEdgeAnalysis & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
