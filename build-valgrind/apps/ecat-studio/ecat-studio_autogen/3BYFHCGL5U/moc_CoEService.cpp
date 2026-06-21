/****************************************************************************
** Meta object code from reading C++ file 'CoEService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/services/CoEService.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CoEService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10CoEServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto CoEService::qt_create_metaobjectdata<qt_meta_tag_ZN10CoEServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CoEService",
        "sdoInfoReceived",
        "",
        "position",
        "SdoInfo",
        "info",
        "dictionaryReceived",
        "QList<CoESdoDictionary>",
        "entries",
        "segmentReceived",
        "index",
        "data",
        "segmentDownloaded",
        "success",
        "emergencyReceived",
        "errorCode",
        "timestampReceived",
        "timestamp",
        "error",
        "message"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'sdoInfoReceived'
        QtMocHelpers::SignalData<void(int, const SdoInfo &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { 0x80000000 | 4, 5 },
        }}),
        // Signal 'dictionaryReceived'
        QtMocHelpers::SignalData<void(int, const QList<CoESdoDictionary> &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { 0x80000000 | 7, 8 },
        }}),
        // Signal 'segmentReceived'
        QtMocHelpers::SignalData<void(int, const QString &, const QByteArray &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::QString, 10 }, { QMetaType::QByteArray, 11 },
        }}),
        // Signal 'segmentDownloaded'
        QtMocHelpers::SignalData<void(int, const QString &, bool)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::QString, 10 }, { QMetaType::Bool, 13 },
        }}),
        // Signal 'emergencyReceived'
        QtMocHelpers::SignalData<void(int, int, const QByteArray &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::Int, 15 }, { QMetaType::QByteArray, 11 },
        }}),
        // Signal 'timestampReceived'
        QtMocHelpers::SignalData<void(int, const QDateTime &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::QDateTime, 17 },
        }}),
        // Signal 'error'
        QtMocHelpers::SignalData<void(const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 19 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CoEService, qt_meta_tag_ZN10CoEServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CoEService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10CoEServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10CoEServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10CoEServiceE_t>.metaTypes,
    nullptr
} };

void CoEService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CoEService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->sdoInfoReceived((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<SdoInfo>>(_a[2]))); break;
        case 1: _t->dictionaryReceived((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QList<CoESdoDictionary>>>(_a[2]))); break;
        case 2: _t->segmentReceived((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[3]))); break;
        case 3: _t->segmentDownloaded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3]))); break;
        case 4: _t->emergencyReceived((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[3]))); break;
        case 5: _t->timestampReceived((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2]))); break;
        case 6: _t->error((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< SdoInfo >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<CoESdoDictionary> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (CoEService::*)(int , const SdoInfo & )>(_a, &CoEService::sdoInfoReceived, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (CoEService::*)(int , const QList<CoESdoDictionary> & )>(_a, &CoEService::dictionaryReceived, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (CoEService::*)(int , const QString & , const QByteArray & )>(_a, &CoEService::segmentReceived, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (CoEService::*)(int , const QString & , bool )>(_a, &CoEService::segmentDownloaded, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (CoEService::*)(int , int , const QByteArray & )>(_a, &CoEService::emergencyReceived, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (CoEService::*)(int , const QDateTime & )>(_a, &CoEService::timestampReceived, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (CoEService::*)(const QString & )>(_a, &CoEService::error, 6))
            return;
    }
}

const QMetaObject *CoEService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CoEService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10CoEServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CoEService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void CoEService::sdoInfoReceived(int _t1, const SdoInfo & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void CoEService::dictionaryReceived(int _t1, const QList<CoESdoDictionary> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void CoEService::segmentReceived(int _t1, const QString & _t2, const QByteArray & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2, _t3);
}

// SIGNAL 3
void CoEService::segmentDownloaded(int _t1, const QString & _t2, bool _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2, _t3);
}

// SIGNAL 4
void CoEService::emergencyReceived(int _t1, int _t2, const QByteArray & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2, _t3);
}

// SIGNAL 5
void CoEService::timestampReceived(int _t1, const QDateTime & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 6
void CoEService::error(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}
QT_WARNING_POP
