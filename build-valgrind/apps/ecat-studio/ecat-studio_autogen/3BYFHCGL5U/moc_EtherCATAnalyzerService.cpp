/****************************************************************************
** Meta object code from reading C++ file 'EtherCATAnalyzerService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/services/EtherCATAnalyzerService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EtherCATAnalyzerService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN23EtherCATAnalyzerServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto EtherCATAnalyzerService::qt_create_metaobjectdata<qt_meta_tag_ZN23EtherCATAnalyzerServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "EtherCATAnalyzerService",
        "frameAnalysisCompleted",
        "",
        "FrameAnalysis",
        "analysis",
        "errorAnalysisCompleted",
        "ErrorAnalysis",
        "performanceAnalysisCompleted",
        "PerformanceAnalysis",
        "trendAnalysisCompleted",
        "TrendAnalysis"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'frameAnalysisCompleted'
        QtMocHelpers::SignalData<void(const FrameAnalysis &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'errorAnalysisCompleted'
        QtMocHelpers::SignalData<void(const ErrorAnalysis &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 4 },
        }}),
        // Signal 'performanceAnalysisCompleted'
        QtMocHelpers::SignalData<void(const PerformanceAnalysis &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 4 },
        }}),
        // Signal 'trendAnalysisCompleted'
        QtMocHelpers::SignalData<void(const TrendAnalysis &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 4 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<EtherCATAnalyzerService, qt_meta_tag_ZN23EtherCATAnalyzerServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject EtherCATAnalyzerService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23EtherCATAnalyzerServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23EtherCATAnalyzerServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN23EtherCATAnalyzerServiceE_t>.metaTypes,
    nullptr
} };

void EtherCATAnalyzerService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<EtherCATAnalyzerService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->frameAnalysisCompleted((*reinterpret_cast<std::add_pointer_t<FrameAnalysis>>(_a[1]))); break;
        case 1: _t->errorAnalysisCompleted((*reinterpret_cast<std::add_pointer_t<ErrorAnalysis>>(_a[1]))); break;
        case 2: _t->performanceAnalysisCompleted((*reinterpret_cast<std::add_pointer_t<PerformanceAnalysis>>(_a[1]))); break;
        case 3: _t->trendAnalysisCompleted((*reinterpret_cast<std::add_pointer_t<TrendAnalysis>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (EtherCATAnalyzerService::*)(const FrameAnalysis & )>(_a, &EtherCATAnalyzerService::frameAnalysisCompleted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATAnalyzerService::*)(const ErrorAnalysis & )>(_a, &EtherCATAnalyzerService::errorAnalysisCompleted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATAnalyzerService::*)(const PerformanceAnalysis & )>(_a, &EtherCATAnalyzerService::performanceAnalysisCompleted, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (EtherCATAnalyzerService::*)(const TrendAnalysis & )>(_a, &EtherCATAnalyzerService::trendAnalysisCompleted, 3))
            return;
    }
}

const QMetaObject *EtherCATAnalyzerService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EtherCATAnalyzerService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23EtherCATAnalyzerServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EtherCATAnalyzerService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void EtherCATAnalyzerService::frameAnalysisCompleted(const FrameAnalysis & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void EtherCATAnalyzerService::errorAnalysisCompleted(const ErrorAnalysis & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void EtherCATAnalyzerService::performanceAnalysisCompleted(const PerformanceAnalysis & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void EtherCATAnalyzerService::trendAnalysisCompleted(const TrendAnalysis & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
