/****************************************************************************
** Meta object code from reading C++ file 'singleplayergamemanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../singleplayergamemanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'singleplayergamemanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.1. It"
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
struct qt_meta_tag_ZN23SinglePlayerGameManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto SinglePlayerGameManager::qt_create_metaobjectdata<qt_meta_tag_ZN23SinglePlayerGameManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SinglePlayerGameManager",
        "modeChanged",
        "",
        "SinglePlayerMode",
        "mode",
        "statsUpdated",
        "GameStats",
        "stats",
        "achievementUnlocked",
        "Achievement",
        "achievement",
        "timeWarning",
        "secondsLeft",
        "waveCompleted",
        "wave",
        "speedIncreased",
        "multiplier",
        "aiScoreUpdated",
        "aiScore",
        "playerScore",
        "gameEnded",
        "finalStats",
        "onGameTimer",
        "onModeTimer",
        "onSpeedTimer"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'modeChanged'
        QtMocHelpers::SignalData<void(SinglePlayerMode)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'statsUpdated'
        QtMocHelpers::SignalData<void(const GameStats &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'achievementUnlocked'
        QtMocHelpers::SignalData<void(const Achievement &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Signal 'timeWarning'
        QtMocHelpers::SignalData<void(int)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 12 },
        }}),
        // Signal 'waveCompleted'
        QtMocHelpers::SignalData<void(int)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 14 },
        }}),
        // Signal 'speedIncreased'
        QtMocHelpers::SignalData<void(double)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 16 },
        }}),
        // Signal 'aiScoreUpdated'
        QtMocHelpers::SignalData<void(int, int)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 18 }, { QMetaType::Int, 19 },
        }}),
        // Signal 'gameEnded'
        QtMocHelpers::SignalData<void(SinglePlayerMode, const GameStats &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 6, 21 },
        }}),
        // Slot 'onGameTimer'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModeTimer'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSpeedTimer'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SinglePlayerGameManager, qt_meta_tag_ZN23SinglePlayerGameManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SinglePlayerGameManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23SinglePlayerGameManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23SinglePlayerGameManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN23SinglePlayerGameManagerE_t>.metaTypes,
    nullptr
} };

void SinglePlayerGameManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SinglePlayerGameManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->modeChanged((*reinterpret_cast< std::add_pointer_t<SinglePlayerMode>>(_a[1]))); break;
        case 1: _t->statsUpdated((*reinterpret_cast< std::add_pointer_t<GameStats>>(_a[1]))); break;
        case 2: _t->achievementUnlocked((*reinterpret_cast< std::add_pointer_t<Achievement>>(_a[1]))); break;
        case 3: _t->timeWarning((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->waveCompleted((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->speedIncreased((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 6: _t->aiScoreUpdated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 7: _t->gameEnded((*reinterpret_cast< std::add_pointer_t<SinglePlayerMode>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<GameStats>>(_a[2]))); break;
        case 8: _t->onGameTimer(); break;
        case 9: _t->onModeTimer(); break;
        case 10: _t->onSpeedTimer(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SinglePlayerGameManager::*)(SinglePlayerMode )>(_a, &SinglePlayerGameManager::modeChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SinglePlayerGameManager::*)(const GameStats & )>(_a, &SinglePlayerGameManager::statsUpdated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SinglePlayerGameManager::*)(const Achievement & )>(_a, &SinglePlayerGameManager::achievementUnlocked, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SinglePlayerGameManager::*)(int )>(_a, &SinglePlayerGameManager::timeWarning, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SinglePlayerGameManager::*)(int )>(_a, &SinglePlayerGameManager::waveCompleted, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (SinglePlayerGameManager::*)(double )>(_a, &SinglePlayerGameManager::speedIncreased, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (SinglePlayerGameManager::*)(int , int )>(_a, &SinglePlayerGameManager::aiScoreUpdated, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (SinglePlayerGameManager::*)(SinglePlayerMode , const GameStats & )>(_a, &SinglePlayerGameManager::gameEnded, 7))
            return;
    }
}

const QMetaObject *SinglePlayerGameManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SinglePlayerGameManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23SinglePlayerGameManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SinglePlayerGameManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void SinglePlayerGameManager::modeChanged(SinglePlayerMode _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void SinglePlayerGameManager::statsUpdated(const GameStats & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void SinglePlayerGameManager::achievementUnlocked(const Achievement & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void SinglePlayerGameManager::timeWarning(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void SinglePlayerGameManager::waveCompleted(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void SinglePlayerGameManager::speedIncreased(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void SinglePlayerGameManager::aiScoreUpdated(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void SinglePlayerGameManager::gameEnded(SinglePlayerMode _t1, const GameStats & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}
QT_WARNING_POP
