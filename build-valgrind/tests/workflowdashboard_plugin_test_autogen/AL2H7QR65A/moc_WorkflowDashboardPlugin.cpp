/****************************************************************************
** Meta object code from reading C++ file 'WorkflowDashboardPlugin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/plugins/workflowdashboard/WorkflowDashboardPlugin.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WorkflowDashboardPlugin.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN23WorkflowDashboardPluginE_t {};
} // unnamed namespace

template <> constexpr inline auto WorkflowDashboardPlugin::qt_create_metaobjectdata<qt_meta_tag_ZN23WorkflowDashboardPluginE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "WorkflowDashboardPlugin",
        "workflowActivated",
        "",
        "workflowId",
        "alertAcknowledged",
        "index",
        "dashboardExported",
        "filePath"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'workflowActivated'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'alertAcknowledged'
        QtMocHelpers::SignalData<void(int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Signal 'dashboardExported'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<WorkflowDashboardPlugin, qt_meta_tag_ZN23WorkflowDashboardPluginE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject WorkflowDashboardPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<WorkspacePlugin::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23WorkflowDashboardPluginE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23WorkflowDashboardPluginE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN23WorkflowDashboardPluginE_t>.metaTypes,
    nullptr
} };

void WorkflowDashboardPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WorkflowDashboardPlugin *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->workflowActivated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->alertAcknowledged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->dashboardExported((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (WorkflowDashboardPlugin::*)(const QString & )>(_a, &WorkflowDashboardPlugin::workflowActivated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowDashboardPlugin::*)(int )>(_a, &WorkflowDashboardPlugin::alertAcknowledged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkflowDashboardPlugin::*)(const QString & )>(_a, &WorkflowDashboardPlugin::dashboardExported, 2))
            return;
    }
}

const QMetaObject *WorkflowDashboardPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkflowDashboardPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23WorkflowDashboardPluginE_t>.strings))
        return static_cast<void*>(this);
    return WorkspacePlugin::qt_metacast(_clname);
}

int WorkflowDashboardPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = WorkspacePlugin::qt_metacall(_c, _id, _a);
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
void WorkflowDashboardPlugin::workflowActivated(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void WorkflowDashboardPlugin::alertAcknowledged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void WorkflowDashboardPlugin::dashboardExported(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
