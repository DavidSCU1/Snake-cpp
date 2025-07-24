/****************************************************************************
** Meta object code from reading C++ file 'hotspotgamemanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../hotspotgamemanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hotspotgamemanager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN18HotspotGameManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto HotspotGameManager::qt_create_metaobjectdata<qt_meta_tag_ZN18HotspotGameManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "HotspotGameManager",
        "roomCreated",
        "",
        "roomName",
        "playerJoined",
        "playerName",
        "playerLeft",
        "roomDestroyed",
        "gameStarted",
        "gameEnded",
        "winner",
        "gamePaused",
        "gameResumed",
        "gameReset",
        "playerCharacterChanged",
        "CharacterType",
        "character",
        "playerReadyChanged",
        "ready",
        "playerScoreChanged",
        "score",
        "playerDied",
        "gameStateUpdated",
        "HotspotGameState",
        "state",
        "foodEaten",
        "points",
        "countdownUpdated",
        "seconds",
        "onGameTick",
        "onCountdownTick",
        "onNetworkPlayerData",
        "playerData",
        "onNetworkGameState",
        "gameState",
        "onNetworkPlayerConnected",
        "onNetworkPlayerDisconnected"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'roomCreated'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'playerJoined'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'playerLeft'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'roomDestroyed'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'gameStarted'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'gameEnded'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Signal 'gamePaused'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'gameResumed'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'gameReset'
        QtMocHelpers::SignalData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'playerCharacterChanged'
        QtMocHelpers::SignalData<void(const QString &, CharacterType)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 }, { 0x80000000 | 15, 16 },
        }}),
        // Signal 'playerReadyChanged'
        QtMocHelpers::SignalData<void(const QString &, bool)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 }, { QMetaType::Bool, 18 },
        }}),
        // Signal 'playerScoreChanged'
        QtMocHelpers::SignalData<void(const QString &, int)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 }, { QMetaType::Int, 20 },
        }}),
        // Signal 'playerDied'
        QtMocHelpers::SignalData<void(const QString &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'gameStateUpdated'
        QtMocHelpers::SignalData<void(const HotspotGameState &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 23, 24 },
        }}),
        // Signal 'foodEaten'
        QtMocHelpers::SignalData<void(const QString &, int)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 }, { QMetaType::Int, 26 },
        }}),
        // Signal 'countdownUpdated'
        QtMocHelpers::SignalData<void(int)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 28 },
        }}),
        // Slot 'onGameTick'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCountdownTick'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onNetworkPlayerData'
        QtMocHelpers::SlotData<void(const QString &, const QJsonObject &)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 5 }, { QMetaType::QJsonObject, 32 },
        }}),
        // Slot 'onNetworkGameState'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QJsonObject, 34 },
        }}),
        // Slot 'onNetworkPlayerConnected'
        QtMocHelpers::SlotData<void(const QString &)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Slot 'onNetworkPlayerDisconnected'
        QtMocHelpers::SlotData<void(const QString &)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<HotspotGameManager, qt_meta_tag_ZN18HotspotGameManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject HotspotGameManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18HotspotGameManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18HotspotGameManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN18HotspotGameManagerE_t>.metaTypes,
    nullptr
} };

void HotspotGameManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<HotspotGameManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->roomCreated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->playerJoined((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->playerLeft((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->roomDestroyed(); break;
        case 4: _t->gameStarted(); break;
        case 5: _t->gameEnded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->gamePaused(); break;
        case 7: _t->gameResumed(); break;
        case 8: _t->gameReset(); break;
        case 9: _t->playerCharacterChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<CharacterType>>(_a[2]))); break;
        case 10: _t->playerReadyChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 11: _t->playerScoreChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 12: _t->playerDied((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->gameStateUpdated((*reinterpret_cast< std::add_pointer_t<HotspotGameState>>(_a[1]))); break;
        case 14: _t->foodEaten((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 15: _t->countdownUpdated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 16: _t->onGameTick(); break;
        case 17: _t->onCountdownTick(); break;
        case 18: _t->onNetworkPlayerData((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 19: _t->onNetworkGameState((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 20: _t->onNetworkPlayerConnected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 21: _t->onNetworkPlayerDisconnected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)(const QString & )>(_a, &HotspotGameManager::roomCreated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)(const QString & )>(_a, &HotspotGameManager::playerJoined, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)(const QString & )>(_a, &HotspotGameManager::playerLeft, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)()>(_a, &HotspotGameManager::roomDestroyed, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)()>(_a, &HotspotGameManager::gameStarted, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)(const QString & )>(_a, &HotspotGameManager::gameEnded, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)()>(_a, &HotspotGameManager::gamePaused, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)()>(_a, &HotspotGameManager::gameResumed, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)()>(_a, &HotspotGameManager::gameReset, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)(const QString & , CharacterType )>(_a, &HotspotGameManager::playerCharacterChanged, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)(const QString & , bool )>(_a, &HotspotGameManager::playerReadyChanged, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)(const QString & , int )>(_a, &HotspotGameManager::playerScoreChanged, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)(const QString & )>(_a, &HotspotGameManager::playerDied, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)(const HotspotGameState & )>(_a, &HotspotGameManager::gameStateUpdated, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)(const QString & , int )>(_a, &HotspotGameManager::foodEaten, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotGameManager::*)(int )>(_a, &HotspotGameManager::countdownUpdated, 15))
            return;
    }
}

const QMetaObject *HotspotGameManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HotspotGameManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18HotspotGameManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int HotspotGameManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void HotspotGameManager::roomCreated(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void HotspotGameManager::playerJoined(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void HotspotGameManager::playerLeft(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void HotspotGameManager::roomDestroyed()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void HotspotGameManager::gameStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void HotspotGameManager::gameEnded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void HotspotGameManager::gamePaused()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void HotspotGameManager::gameResumed()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void HotspotGameManager::gameReset()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void HotspotGameManager::playerCharacterChanged(const QString & _t1, CharacterType _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2);
}

// SIGNAL 10
void HotspotGameManager::playerReadyChanged(const QString & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1, _t2);
}

// SIGNAL 11
void HotspotGameManager::playerScoreChanged(const QString & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1, _t2);
}

// SIGNAL 12
void HotspotGameManager::playerDied(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1);
}

// SIGNAL 13
void HotspotGameManager::gameStateUpdated(const HotspotGameState & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 13, nullptr, _t1);
}

// SIGNAL 14
void HotspotGameManager::foodEaten(const QString & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1, _t2);
}

// SIGNAL 15
void HotspotGameManager::countdownUpdated(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 15, nullptr, _t1);
}
QT_WARNING_POP
