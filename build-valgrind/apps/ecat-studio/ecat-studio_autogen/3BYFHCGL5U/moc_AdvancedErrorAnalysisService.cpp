/****************************************************************************
** Meta object code from reading C++ file 'AdvancedErrorAnalysisService.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../apps/ecat-studio/services/AdvancedErrorAnalysisService.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AdvancedErrorAnalysisService.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN28AdvancedErrorAnalysisServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto AdvancedErrorAnalysisService::qt_create_metaobjectdata<qt_meta_tag_ZN28AdvancedErrorAnalysisServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AdvancedErrorAnalysisService",
        "patternDetected",
        "",
        "ErrorPattern",
        "pattern",
        "correlationAnalyzed",
        "CorrelationMatrix",
        "matrix",
        "predictionGenerated",
        "ErrorPrediction",
        "prediction",
        "rootCauseFound",
        "RootCauseAnalysis",
        "analysis"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'patternDetected'
        QtMocHelpers::SignalData<void(const ErrorPattern &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'correlationAnalyzed'
        QtMocHelpers::SignalData<void(const CorrelationMatrix &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'predictionGenerated'
        QtMocHelpers::SignalData<void(const ErrorPrediction &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Signal 'rootCauseFound'
        QtMocHelpers::SignalData<void(const RootCauseAnalysis &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AdvancedErrorAnalysisService, qt_meta_tag_ZN28AdvancedErrorAnalysisServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AdvancedErrorAnalysisService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN28AdvancedErrorAnalysisServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN28AdvancedErrorAnalysisServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN28AdvancedErrorAnalysisServiceE_t>.metaTypes,
    nullptr
} };

void AdvancedErrorAnalysisService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AdvancedErrorAnalysisService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->patternDetected((*reinterpret_cast<std::add_pointer_t<ErrorPattern>>(_a[1]))); break;
        case 1: _t->correlationAnalyzed((*reinterpret_cast<std::add_pointer_t<CorrelationMatrix>>(_a[1]))); break;
        case 2: _t->predictionGenerated((*reinterpret_cast<std::add_pointer_t<ErrorPrediction>>(_a[1]))); break;
        case 3: _t->rootCauseFound((*reinterpret_cast<std::add_pointer_t<RootCauseAnalysis>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AdvancedErrorAnalysisService::*)(const ErrorPattern & )>(_a, &AdvancedErrorAnalysisService::patternDetected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AdvancedErrorAnalysisService::*)(const CorrelationMatrix & )>(_a, &AdvancedErrorAnalysisService::correlationAnalyzed, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AdvancedErrorAnalysisService::*)(const ErrorPrediction & )>(_a, &AdvancedErrorAnalysisService::predictionGenerated, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AdvancedErrorAnalysisService::*)(const RootCauseAnalysis & )>(_a, &AdvancedErrorAnalysisService::rootCauseFound, 3))
            return;
    }
}

const QMetaObject *AdvancedErrorAnalysisService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AdvancedErrorAnalysisService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN28AdvancedErrorAnalysisServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AdvancedErrorAnalysisService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void AdvancedErrorAnalysisService::patternDetected(const ErrorPattern & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void AdvancedErrorAnalysisService::correlationAnalyzed(const CorrelationMatrix & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void AdvancedErrorAnalysisService::predictionGenerated(const ErrorPrediction & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void AdvancedErrorAnalysisService::rootCauseFound(const RootCauseAnalysis & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
QT_WARNING_POP
