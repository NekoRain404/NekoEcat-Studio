/****************************************************************************
** Meta object code from reading C++ file 'FreeRunOptimizationPlugin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/plugins/freerunoptimization/FreeRunOptimizationPlugin.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FreeRunOptimizationPlugin.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN25FreeRunOptimizationPluginE_t {};
} // unnamed namespace

template <> constexpr inline auto FreeRunOptimizationPlugin::qt_create_metaobjectdata<qt_meta_tag_ZN25FreeRunOptimizationPluginE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FreeRunOptimizationPlugin",
        "handleCycleTimeOptimize",
        "",
        "handleDataMappingOptimize",
        "handlePerformanceOptimize",
        "handleErrorHandlingOptimize",
        "handleOptimizationCompleted",
        "FreeRunOptimizationResult",
        "result",
        "handleOptimizationApplied",
        "handleExport"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'handleCycleTimeOptimize'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleDataMappingOptimize'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handlePerformanceOptimize'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleErrorHandlingOptimize'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleOptimizationCompleted'
        QtMocHelpers::SlotData<void(const FreeRunOptimizationResult &)>(6, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Slot 'handleOptimizationApplied'
        QtMocHelpers::SlotData<void(const FreeRunOptimizationResult &)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Slot 'handleExport'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FreeRunOptimizationPlugin, qt_meta_tag_ZN25FreeRunOptimizationPluginE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FreeRunOptimizationPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<WorkspacePlugin::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25FreeRunOptimizationPluginE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25FreeRunOptimizationPluginE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN25FreeRunOptimizationPluginE_t>.metaTypes,
    nullptr
} };

void FreeRunOptimizationPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FreeRunOptimizationPlugin *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->handleCycleTimeOptimize(); break;
        case 1: _t->handleDataMappingOptimize(); break;
        case 2: _t->handlePerformanceOptimize(); break;
        case 3: _t->handleErrorHandlingOptimize(); break;
        case 4: _t->handleOptimizationCompleted((*reinterpret_cast<std::add_pointer_t<FreeRunOptimizationResult>>(_a[1]))); break;
        case 5: _t->handleOptimizationApplied((*reinterpret_cast<std::add_pointer_t<FreeRunOptimizationResult>>(_a[1]))); break;
        case 6: _t->handleExport(); break;
        default: ;
        }
    }
}

const QMetaObject *FreeRunOptimizationPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FreeRunOptimizationPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25FreeRunOptimizationPluginE_t>.strings))
        return static_cast<void*>(this);
    return WorkspacePlugin::qt_metacast(_clname);
}

int FreeRunOptimizationPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = WorkspacePlugin::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
