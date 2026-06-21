/****************************************************************************
** Meta object code from reading C++ file 'CalibrationPlugin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../apps/ecat-studio/plugins/calibration/CalibrationPlugin.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CalibrationPlugin.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17CalibrationPluginE_t {};
} // unnamed namespace

template <> constexpr inline auto CalibrationPlugin::qt_create_metaobjectdata<qt_meta_tag_ZN17CalibrationPluginE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CalibrationPlugin",
        "stepChanged",
        "",
        "CalibrationPlugin::WizardStep",
        "step",
        "calibrationComplete",
        "sampleCollected",
        "sampleIndex",
        "value",
        "nextStep",
        "prevStep",
        "startCalibration",
        "stopCalibration",
        "collectSample",
        "analyzeResults",
        "resetWizard",
        "exportCalibrationData",
        "path",
        "exportHistory"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'stepChanged'
        QtMocHelpers::SignalData<void(CalibrationPlugin::WizardStep)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'calibrationComplete'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sampleCollected'
        QtMocHelpers::SignalData<void(int, double)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 }, { QMetaType::Double, 8 },
        }}),
        // Slot 'nextStep'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'prevStep'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'startCalibration'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'stopCalibration'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'collectSample'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'analyzeResults'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'resetWizard'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'exportCalibrationData'
        QtMocHelpers::SlotData<void(const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
        // Slot 'exportHistory'
        QtMocHelpers::SlotData<void(const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CalibrationPlugin, qt_meta_tag_ZN17CalibrationPluginE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CalibrationPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<WorkspacePlugin::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17CalibrationPluginE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17CalibrationPluginE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17CalibrationPluginE_t>.metaTypes,
    nullptr
} };

void CalibrationPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CalibrationPlugin *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->stepChanged((*reinterpret_cast<std::add_pointer_t<CalibrationPlugin::WizardStep>>(_a[1]))); break;
        case 1: _t->calibrationComplete(); break;
        case 2: _t->sampleCollected((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 3: _t->nextStep(); break;
        case 4: _t->prevStep(); break;
        case 5: _t->startCalibration(); break;
        case 6: _t->stopCalibration(); break;
        case 7: _t->collectSample(); break;
        case 8: _t->analyzeResults(); break;
        case 9: _t->resetWizard(); break;
        case 10: _t->exportCalibrationData((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->exportHistory((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (CalibrationPlugin::*)(CalibrationPlugin::WizardStep )>(_a, &CalibrationPlugin::stepChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (CalibrationPlugin::*)()>(_a, &CalibrationPlugin::calibrationComplete, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (CalibrationPlugin::*)(int , double )>(_a, &CalibrationPlugin::sampleCollected, 2))
            return;
    }
}

const QMetaObject *CalibrationPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalibrationPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17CalibrationPluginE_t>.strings))
        return static_cast<void*>(this);
    return WorkspacePlugin::qt_metacast(_clname);
}

int CalibrationPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = WorkspacePlugin::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void CalibrationPlugin::stepChanged(CalibrationPlugin::WizardStep _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void CalibrationPlugin::calibrationComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void CalibrationPlugin::sampleCollected(int _t1, double _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}
QT_WARNING_POP
