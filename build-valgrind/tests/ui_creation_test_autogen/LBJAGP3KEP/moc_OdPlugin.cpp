/****************************************************************************
** Meta object code from reading C++ file 'OdPlugin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/plugins/od/OdPlugin.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OdPlugin.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN8OdPluginE_t {};
} // unnamed namespace

template <> constexpr inline auto OdPlugin::qt_create_metaobjectdata<qt_meta_tag_ZN8OdPluginE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "OdPlugin",
        "sdoFilterChanged",
        "",
        "text",
        "sdoTableSelectionChanged",
        "sdoTargetTrailSelectionChanged",
        "objectBookmarkSelectionChanged",
        "sdoHistorySelectionChanged",
        "sdoTargetPanelRowDoubleClicked",
        "row",
        "sdoTargetPanelRowActionRequested",
        "sdoTargetPanelCopyRequested"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'sdoFilterChanged'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'sdoTableSelectionChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sdoTargetTrailSelectionChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'objectBookmarkSelectionChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sdoHistorySelectionChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sdoTargetPanelRowDoubleClicked'
        QtMocHelpers::SignalData<void(int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Signal 'sdoTargetPanelRowActionRequested'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sdoTargetPanelCopyRequested'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<OdPlugin, qt_meta_tag_ZN8OdPluginE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject OdPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<WorkspacePlugin::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8OdPluginE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8OdPluginE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8OdPluginE_t>.metaTypes,
    nullptr
} };

void OdPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<OdPlugin *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->sdoFilterChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->sdoTableSelectionChanged(); break;
        case 2: _t->sdoTargetTrailSelectionChanged(); break;
        case 3: _t->objectBookmarkSelectionChanged(); break;
        case 4: _t->sdoHistorySelectionChanged(); break;
        case 5: _t->sdoTargetPanelRowDoubleClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->sdoTargetPanelRowActionRequested(); break;
        case 7: _t->sdoTargetPanelCopyRequested(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (OdPlugin::*)(const QString & )>(_a, &OdPlugin::sdoFilterChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (OdPlugin::*)()>(_a, &OdPlugin::sdoTableSelectionChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (OdPlugin::*)()>(_a, &OdPlugin::sdoTargetTrailSelectionChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (OdPlugin::*)()>(_a, &OdPlugin::objectBookmarkSelectionChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (OdPlugin::*)()>(_a, &OdPlugin::sdoHistorySelectionChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (OdPlugin::*)(int )>(_a, &OdPlugin::sdoTargetPanelRowDoubleClicked, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (OdPlugin::*)()>(_a, &OdPlugin::sdoTargetPanelRowActionRequested, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (OdPlugin::*)()>(_a, &OdPlugin::sdoTargetPanelCopyRequested, 7))
            return;
    }
}

const QMetaObject *OdPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OdPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8OdPluginE_t>.strings))
        return static_cast<void*>(this);
    return WorkspacePlugin::qt_metacast(_clname);
}

int OdPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = WorkspacePlugin::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void OdPlugin::sdoFilterChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void OdPlugin::sdoTableSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void OdPlugin::sdoTargetTrailSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void OdPlugin::objectBookmarkSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void OdPlugin::sdoHistorySelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void OdPlugin::sdoTargetPanelRowDoubleClicked(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void OdPlugin::sdoTargetPanelRowActionRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void OdPlugin::sdoTargetPanelCopyRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
