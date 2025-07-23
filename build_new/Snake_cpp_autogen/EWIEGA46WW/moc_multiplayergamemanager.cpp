/****************************************************************************
** Meta object code from reading C++ file 'multiplayergamemanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../multiplayergamemanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'multiplayergamemanager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN22MultiPlayerGameManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto MultiPlayerGameManager::qt_create_metaobjectdata<qt_meta_tag_ZN22MultiPlayerGameManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MultiPlayerGameManager",
        "roomCreated",
        "",
        "roomId",
        "GameRoom",
        "room",
        "playerJoinedRoom",
        "playerName",
        "playerLeftRoom",
        "gameStarted",
        "gameEnded",
        "winner",
        "gameStateUpdated",
        "MultiPlayerGameState",
        "state",
        "playerCollision",
        "foodEaten",
        "points",
        "roomDestroyed",
        "onGameTick",
        "onNetworkPlayerInfoReceived",
        "PlayerInfo",
        "playerInfo",
        "onNetworkPlayerPositionReceived",
        "std::deque<Point>",
        "snakeBody",
        "onNetworkPlayerDisconnected",
        "onNetworkPlayerConnected",
        "onRoomInfoRequested",
        "QJsonObject&",
        "roomInfo"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'roomCreated'
        QtMocHelpers::SignalData<void(const QString &, const GameRoom &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 4, 5 },
        }}),
        // Signal 'playerJoinedRoom'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 7 },
        }}),
        // Signal 'playerLeftRoom'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 7 },
        }}),
        // Signal 'gameStarted'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'gameEnded'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 11 },
        }}),
        // Signal 'gameStateUpdated'
        QtMocHelpers::SignalData<void(const QString &, const MultiPlayerGameState &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 13, 14 },
        }}),
        // Signal 'playerCollision'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 7 },
        }}),
        // Signal 'foodEaten'
        QtMocHelpers::SignalData<void(const QString &, const QString &, int)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 7 }, { QMetaType::Int, 17 },
        }}),
        // Signal 'roomDestroyed'
        QtMocHelpers::SignalData<void(const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Slot 'onGameTick'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onNetworkPlayerInfoReceived'
        QtMocHelpers::SlotData<void(const PlayerInfo &)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 21, 22 },
        }}),
        // Slot 'onNetworkPlayerPositionReceived'
        QtMocHelpers::SlotData<void(const QString &, const std::deque<Point> &)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { 0x80000000 | 24, 25 },
        }}),
        // Slot 'onNetworkPlayerDisconnected'
        QtMocHelpers::SlotData<void(const QString &)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onNetworkPlayerConnected'
        QtMocHelpers::SlotData<void(const QString &)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onRoomInfoRequested'
        QtMocHelpers::SlotData<void(QJsonObject &)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 29, 30 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MultiPlayerGameManager, qt_meta_tag_ZN22MultiPlayerGameManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MultiPlayerGameManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22MultiPlayerGameManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22MultiPlayerGameManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN22MultiPlayerGameManagerE_t>.metaTypes,
    nullptr
} };

void MultiPlayerGameManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MultiPlayerGameManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->roomCreated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<GameRoom>>(_a[2]))); break;
        case 1: _t->playerJoinedRoom((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->playerLeftRoom((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->gameStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->gameEnded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->gameStateUpdated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<MultiPlayerGameState>>(_a[2]))); break;
        case 6: _t->playerCollision((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->foodEaten((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 8: _t->roomDestroyed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->onGameTick(); break;
        case 10: _t->onNetworkPlayerInfoReceived((*reinterpret_cast< std::add_pointer_t<PlayerInfo>>(_a[1]))); break;
        case 11: _t->onNetworkPlayerPositionReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::deque<Point>>>(_a[2]))); break;
        case 12: _t->onNetworkPlayerDisconnected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->onNetworkPlayerConnected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: _t->onRoomInfoRequested((*reinterpret_cast< std::add_pointer_t<QJsonObject&>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MultiPlayerGameManager::*)(const QString & , const GameRoom & )>(_a, &MultiPlayerGameManager::roomCreated, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MultiPlayerGameManager::*)(const QString & , const QString & )>(_a, &MultiPlayerGameManager::playerJoinedRoom, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MultiPlayerGameManager::*)(const QString & , const QString & )>(_a, &MultiPlayerGameManager::playerLeftRoom, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MultiPlayerGameManager::*)(const QString & )>(_a, &MultiPlayerGameManager::gameStarted, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (MultiPlayerGameManager::*)(const QString & , const QString & )>(_a, &MultiPlayerGameManager::gameEnded, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (MultiPlayerGameManager::*)(const QString & , const MultiPlayerGameState & )>(_a, &MultiPlayerGameManager::gameStateUpdated, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (MultiPlayerGameManager::*)(const QString & , const QString & )>(_a, &MultiPlayerGameManager::playerCollision, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (MultiPlayerGameManager::*)(const QString & , const QString & , int )>(_a, &MultiPlayerGameManager::foodEaten, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (MultiPlayerGameManager::*)(const QString & )>(_a, &MultiPlayerGameManager::roomDestroyed, 8))
            return;
    }
}

const QMetaObject *MultiPlayerGameManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MultiPlayerGameManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22MultiPlayerGameManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MultiPlayerGameManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void MultiPlayerGameManager::roomCreated(const QString & _t1, const GameRoom & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void MultiPlayerGameManager::playerJoinedRoom(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void MultiPlayerGameManager::playerLeftRoom(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void MultiPlayerGameManager::gameStarted(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void MultiPlayerGameManager::gameEnded(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void MultiPlayerGameManager::gameStateUpdated(const QString & _t1, const MultiPlayerGameState & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 6
void MultiPlayerGameManager::playerCollision(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void MultiPlayerGameManager::foodEaten(const QString & _t1, const QString & _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2, _t3);
}

// SIGNAL 8
void MultiPlayerGameManager::roomDestroyed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}
QT_WARNING_POP
