/****************************************************************************
** Meta object code from reading C++ file 'networkmanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../networkmanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'networkmanager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN14NetworkManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto NetworkManager::qt_create_metaobjectdata<qt_meta_tag_ZN14NetworkManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NetworkManager",
        "playerConnected",
        "",
        "playerName",
        "playerDisconnected",
        "playerInfoReceived",
        "PlayerInfo",
        "playerInfo",
        "gameStateReceived",
        "gameState",
        "scoreUpdateReceived",
        "score",
        "playerPositionReceived",
        "std::deque<Point>",
        "snakeBody",
        "connectionError",
        "error",
        "roomDiscovered",
        "host",
        "port",
        "onNewConnection",
        "onClientConnected",
        "onClientDisconnected",
        "onDataReceived",
        "onSocketError",
        "QAbstractSocket::SocketError",
        "sendHeartbeat"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'playerConnected'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'playerDisconnected'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'playerInfoReceived'
        QtMocHelpers::SignalData<void(const PlayerInfo &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'gameStateReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QJsonObject, 9 },
        }}),
        // Signal 'scoreUpdateReceived'
        QtMocHelpers::SignalData<void(const QString &, int)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Int, 11 },
        }}),
        // Signal 'playerPositionReceived'
        QtMocHelpers::SignalData<void(const QString &, const std::deque<Point> &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 13, 14 },
        }}),
        // Signal 'connectionError'
        QtMocHelpers::SignalData<void(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 16 },
        }}),
        // Signal 'roomDiscovered'
        QtMocHelpers::SignalData<void(const QString &, int)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 }, { QMetaType::Int, 19 },
        }}),
        // Slot 'onNewConnection'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClientConnected'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClientDisconnected'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDataReceived'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketError'
        QtMocHelpers::SlotData<void(QAbstractSocket::SocketError)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 25, 16 },
        }}),
        // Slot 'sendHeartbeat'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NetworkManager, qt_meta_tag_ZN14NetworkManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NetworkManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14NetworkManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14NetworkManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14NetworkManagerE_t>.metaTypes,
    nullptr
} };

void NetworkManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NetworkManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->playerConnected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->playerDisconnected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->playerInfoReceived((*reinterpret_cast< std::add_pointer_t<PlayerInfo>>(_a[1]))); break;
        case 3: _t->gameStateReceived((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 4: _t->scoreUpdateReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 5: _t->playerPositionReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<std::deque<Point>>>(_a[2]))); break;
        case 6: _t->connectionError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->roomDiscovered((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 8: _t->onNewConnection(); break;
        case 9: _t->onClientConnected(); break;
        case 10: _t->onClientDisconnected(); break;
        case 11: _t->onDataReceived(); break;
        case 12: _t->onSocketError((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 13: _t->sendHeartbeat(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (NetworkManager::*)(const QString & )>(_a, &NetworkManager::playerConnected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkManager::*)(const QString & )>(_a, &NetworkManager::playerDisconnected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkManager::*)(const PlayerInfo & )>(_a, &NetworkManager::playerInfoReceived, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkManager::*)(const QJsonObject & )>(_a, &NetworkManager::gameStateReceived, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkManager::*)(const QString & , int )>(_a, &NetworkManager::scoreUpdateReceived, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkManager::*)(const QString & , const std::deque<Point> & )>(_a, &NetworkManager::playerPositionReceived, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkManager::*)(const QString & )>(_a, &NetworkManager::connectionError, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkManager::*)(const QString & , int )>(_a, &NetworkManager::roomDiscovered, 7))
            return;
    }
}

const QMetaObject *NetworkManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14NetworkManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NetworkManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void NetworkManager::playerConnected(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void NetworkManager::playerDisconnected(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void NetworkManager::playerInfoReceived(const PlayerInfo & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void NetworkManager::gameStateReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void NetworkManager::scoreUpdateReceived(const QString & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void NetworkManager::playerPositionReceived(const QString & _t1, const std::deque<Point> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 6
void NetworkManager::connectionError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void NetworkManager::roomDiscovered(const QString & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}
QT_WARNING_POP
