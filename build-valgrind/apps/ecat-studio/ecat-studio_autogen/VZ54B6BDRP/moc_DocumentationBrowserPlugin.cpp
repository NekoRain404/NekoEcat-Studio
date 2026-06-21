/****************************************************************************
** Meta object code from reading C++ file 'DocumentationBrowserPlugin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/plugins/documentationbrowser/DocumentationBrowserPlugin.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DocumentationBrowserPlugin.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN26DocumentationBrowserPluginE_t {};
} // unnamed namespace

template <> constexpr inline auto DocumentationBrowserPlugin::qt_create_metaobjectdata<qt_meta_tag_ZN26DocumentationBrowserPluginE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DocumentationBrowserPlugin",
        "documentAdded",
        "",
        "title",
        "documentRemoved",
        "bookmarkAdded",
        "bookmarkRemoved",
        "searchTriggered",
        "query",
        "exportRequested"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'documentAdded'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'documentRemoved'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'bookmarkAdded'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'bookmarkRemoved'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'searchTriggered'
        QtMocHelpers::SignalData<void(const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'exportRequested'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DocumentationBrowserPlugin, qt_meta_tag_ZN26DocumentationBrowserPluginE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DocumentationBrowserPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<WorkspacePlugin::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN26DocumentationBrowserPluginE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN26DocumentationBrowserPluginE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN26DocumentationBrowserPluginE_t>.metaTypes,
    nullptr
} };

void DocumentationBrowserPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DocumentationBrowserPlugin *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->documentAdded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->documentRemoved((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->bookmarkAdded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->bookmarkRemoved((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->searchTriggered((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->exportRequested(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DocumentationBrowserPlugin::*)(const QString & )>(_a, &DocumentationBrowserPlugin::documentAdded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentationBrowserPlugin::*)(const QString & )>(_a, &DocumentationBrowserPlugin::documentRemoved, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentationBrowserPlugin::*)(const QString & )>(_a, &DocumentationBrowserPlugin::bookmarkAdded, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentationBrowserPlugin::*)(const QString & )>(_a, &DocumentationBrowserPlugin::bookmarkRemoved, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentationBrowserPlugin::*)(const QString & )>(_a, &DocumentationBrowserPlugin::searchTriggered, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentationBrowserPlugin::*)()>(_a, &DocumentationBrowserPlugin::exportRequested, 5))
            return;
    }
}

const QMetaObject *DocumentationBrowserPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DocumentationBrowserPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN26DocumentationBrowserPluginE_t>.strings))
        return static_cast<void*>(this);
    return WorkspacePlugin::qt_metacast(_clname);
}

int DocumentationBrowserPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void DocumentationBrowserPlugin::documentAdded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void DocumentationBrowserPlugin::documentRemoved(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void DocumentationBrowserPlugin::bookmarkAdded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void DocumentationBrowserPlugin::bookmarkRemoved(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void DocumentationBrowserPlugin::searchTriggered(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void DocumentationBrowserPlugin::exportRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
