/****************************************************************************
** Meta object code from reading C++ file 'ReportDesignerPlugin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/plugins/reportdesigner/ReportDesignerPlugin.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ReportDesignerPlugin.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN20ReportDesignerPluginE_t {};
} // unnamed namespace

template <> constexpr inline auto ReportDesignerPlugin::qt_create_metaobjectdata<qt_meta_tag_ZN20ReportDesignerPluginE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ReportDesignerPlugin",
        "templateAdded",
        "",
        "name",
        "templateRemoved",
        "dataBindingAdded",
        "field",
        "dataBindingRemoved",
        "exportRequested",
        "previewUpdated"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'templateAdded'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'templateRemoved'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'dataBindingAdded'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'dataBindingRemoved'
        QtMocHelpers::SignalData<void(const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'exportRequested'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'previewUpdated'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ReportDesignerPlugin, qt_meta_tag_ZN20ReportDesignerPluginE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ReportDesignerPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<WorkspacePlugin::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ReportDesignerPluginE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ReportDesignerPluginE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN20ReportDesignerPluginE_t>.metaTypes,
    nullptr
} };

void ReportDesignerPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ReportDesignerPlugin *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->templateAdded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->templateRemoved((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->dataBindingAdded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->dataBindingRemoved((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->exportRequested(); break;
        case 5: _t->previewUpdated(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ReportDesignerPlugin::*)(const QString & )>(_a, &ReportDesignerPlugin::templateAdded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ReportDesignerPlugin::*)(const QString & )>(_a, &ReportDesignerPlugin::templateRemoved, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ReportDesignerPlugin::*)(const QString & )>(_a, &ReportDesignerPlugin::dataBindingAdded, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ReportDesignerPlugin::*)(const QString & )>(_a, &ReportDesignerPlugin::dataBindingRemoved, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ReportDesignerPlugin::*)()>(_a, &ReportDesignerPlugin::exportRequested, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ReportDesignerPlugin::*)()>(_a, &ReportDesignerPlugin::previewUpdated, 5))
            return;
    }
}

const QMetaObject *ReportDesignerPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ReportDesignerPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ReportDesignerPluginE_t>.strings))
        return static_cast<void*>(this);
    return WorkspacePlugin::qt_metacast(_clname);
}

int ReportDesignerPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = WorkspacePlugin::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ReportDesignerPlugin::templateAdded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void ReportDesignerPlugin::templateRemoved(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ReportDesignerPlugin::dataBindingAdded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void ReportDesignerPlugin::dataBindingRemoved(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void ReportDesignerPlugin::exportRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ReportDesignerPlugin::previewUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
