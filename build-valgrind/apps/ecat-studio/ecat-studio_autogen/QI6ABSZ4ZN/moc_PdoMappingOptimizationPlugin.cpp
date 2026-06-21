/****************************************************************************
** Meta object code from reading C++ file 'PdoMappingOptimizationPlugin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/plugins/pdomappingoptimization/PdoMappingOptimizationPlugin.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PdoMappingOptimizationPlugin.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN28PdoMappingOptimizationPluginE_t {};
} // unnamed namespace

template <> constexpr inline auto PdoMappingOptimizationPlugin::qt_create_metaobjectdata<qt_meta_tag_ZN28PdoMappingOptimizationPluginE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PdoMappingOptimizationPlugin",
        "handleMappingOptimize",
        "",
        "handleSizeOptimize",
        "handleAlignmentOptimize",
        "handlePerformanceOptimize",
        "handleOptimizationCompleted",
        "PdoMappingOptimizationResult",
        "result",
        "handleOptimizationApplied",
        "handleExport"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'handleMappingOptimize'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleSizeOptimize'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleAlignmentOptimize'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handlePerformanceOptimize'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleOptimizationCompleted'
        QtMocHelpers::SlotData<void(const PdoMappingOptimizationResult &)>(6, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Slot 'handleOptimizationApplied'
        QtMocHelpers::SlotData<void(const PdoMappingOptimizationResult &)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Slot 'handleExport'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PdoMappingOptimizationPlugin, qt_meta_tag_ZN28PdoMappingOptimizationPluginE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PdoMappingOptimizationPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<WorkspacePlugin::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN28PdoMappingOptimizationPluginE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN28PdoMappingOptimizationPluginE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN28PdoMappingOptimizationPluginE_t>.metaTypes,
    nullptr
} };

void PdoMappingOptimizationPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PdoMappingOptimizationPlugin *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->handleMappingOptimize(); break;
        case 1: _t->handleSizeOptimize(); break;
        case 2: _t->handleAlignmentOptimize(); break;
        case 3: _t->handlePerformanceOptimize(); break;
        case 4: _t->handleOptimizationCompleted((*reinterpret_cast<std::add_pointer_t<PdoMappingOptimizationResult>>(_a[1]))); break;
        case 5: _t->handleOptimizationApplied((*reinterpret_cast<std::add_pointer_t<PdoMappingOptimizationResult>>(_a[1]))); break;
        case 6: _t->handleExport(); break;
        default: ;
        }
    }
}

const QMetaObject *PdoMappingOptimizationPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PdoMappingOptimizationPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN28PdoMappingOptimizationPluginE_t>.strings))
        return static_cast<void*>(this);
    return WorkspacePlugin::qt_metacast(_clname);
}

int PdoMappingOptimizationPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
