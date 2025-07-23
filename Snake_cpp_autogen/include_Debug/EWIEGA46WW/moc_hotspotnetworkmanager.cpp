/****************************************************************************
** Meta object code from reading C++ file 'hotspotnetworkmanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../hotspotnetworkmanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hotspotnetworkmanager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN21HotspotNetworkManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto HotspotNetworkManager::qt_create_metaobjectdata<qt_meta_tag_ZN21HotspotNetworkManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "HotspotNetworkManager",
        "hostStarted",
        "",
        "roomName",
        "ipAddress",
        "hostStopped",
        "playerConnectedToHost",
        "playerName",
        "playerDisconnectedFromHost",
        "hostDiscovered",
        "hostAddress",
        "playerCount",
        "maxPlayers",
        "connectedToHost",
        "disconnectedFromHost",
        "playerDataReceived",
        "playerData",
        "gameStateReceived",
        "gameState",
        "chatMessageReceived",
        "message",
        "networkError",
        "errorMessage",
        "onNewClientConnection",
        "onClientConnected",
        "onClientDisconnected",
        "onDataReceived",
        "onSocketError",
        "QAbstractSocket::SocketError",
        "error",
        "onDiscoveryTimeout",
        "onHeartbeatTimeout",
        "processHostDiscovery",
        "broadcastHostInfo"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'hostStarted'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 4 },
        }}),
        // Signal 'hostStopped'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'playerConnectedToHost'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'playerDisconnectedFromHost'
        QtMocHelpers::SignalData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'hostDiscovered'
        QtMocHelpers::SignalData<void(const QString &, const QString &, int, int)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 }, { QMetaType::QString, 3 }, { QMetaType::Int, 11 }, { QMetaType::Int, 12 },
        }}),
        // Signal 'connectedToHost'
        QtMocHelpers::SignalData<void(const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Signal 'disconnectedFromHost'
        QtMocHelpers::SignalData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'playerDataReceived'
        QtMocHelpers::SignalData<void(const QString &, const QJsonObject &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { QMetaType::QJsonObject, 16 },
        }}),
        // Signal 'gameStateReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QJsonObject, 18 },
        }}),
        // Signal 'chatMessageReceived'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { QMetaType::QString, 20 },
        }}),
        // Signal 'networkError'
        QtMocHelpers::SignalData<void(const QString &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 22 },
        }}),
        // Slot 'onNewClientConnection'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClientConnected'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onClientDisconnected'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDataReceived'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketError'
        QtMocHelpers::SlotData<void(QAbstractSocket::SocketError)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 28, 29 },
        }}),
        // Slot 'onDiscoveryTimeout'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onHeartbeatTimeout'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'processHostDiscovery'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'broadcastHostInfo'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<HotspotNetworkManager, qt_meta_tag_ZN21HotspotNetworkManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject HotspotNetworkManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21HotspotNetworkManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21HotspotNetworkManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN21HotspotNetworkManagerE_t>.metaTypes,
    nullptr
} };

void HotspotNetworkManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<HotspotNetworkManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->hostStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 1: _t->hostStopped(); break;
        case 2: _t->playerConnectedToHost((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->playerDisconnectedFromHost((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->hostDiscovered((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4]))); break;
        case 5: _t->connectedToHost((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->disconnectedFromHost(); break;
        case 7: _t->playerDataReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 8: _t->gameStateReceived((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 9: _t->chatMessageReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 10: _t->networkError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->onNewClientConnection(); break;
        case 12: _t->onClientConnected(); break;
        case 13: _t->onClientDisconnected(); break;
        case 14: _t->onDataReceived(); break;
        case 15: _t->onSocketError((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 16: _t->onDiscoveryTimeout(); break;
        case 17: _t->onHeartbeatTimeout(); break;
        case 18: _t->processHostDiscovery(); break;
        case 19: _t->broadcastHostInfo(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 15:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (HotspotNetworkManager::*)(const QString & , const QString & )>(_a, &HotspotNetworkManager::hostStarted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotNetworkManager::*)()>(_a, &HotspotNetworkManager::hostStopped, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotNetworkManager::*)(const QString & )>(_a, &HotspotNetworkManager::playerConnectedToHost, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotNetworkManager::*)(const QString & )>(_a, &HotspotNetworkManager::playerDisconnectedFromHost, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotNetworkManager::*)(const QString & , const QString & , int , int )>(_a, &HotspotNetworkManager::hostDiscovered, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotNetworkManager::*)(const QString & )>(_a, &HotspotNetworkManager::connectedToHost, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotNetworkManager::*)()>(_a, &HotspotNetworkManager::disconnectedFromHost, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotNetworkManager::*)(const QString & , const QJsonObject & )>(_a, &HotspotNetworkManager::playerDataReceived, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotNetworkManager::*)(const QJsonObject & )>(_a, &HotspotNetworkManager::gameStateReceived, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotNetworkManager::*)(const QString & , const QString & )>(_a, &HotspotNetworkManager::chatMessageReceived, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotNetworkManager::*)(const QString & )>(_a, &HotspotNetworkManager::networkError, 10))
            return;
    }
}

const QMetaObject *HotspotNetworkManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HotspotNetworkManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21HotspotNetworkManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int HotspotNetworkManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void HotspotNetworkManager::hostStarted(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void HotspotNetworkManager::hostStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void HotspotNetworkManager::playerConnectedToHost(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void HotspotNetworkManager::playerDisconnectedFromHost(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void HotspotNetworkManager::hostDiscovered(const QString & _t1, const QString & _t2, int _t3, int _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 5
void HotspotNetworkManager::connectedToHost(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void HotspotNetworkManager::disconnectedFromHost()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void HotspotNetworkManager::playerDataReceived(const QString & _t1, const QJsonObject & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}

// SIGNAL 8
void HotspotNetworkManager::gameStateReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}

// SIGNAL 9
void HotspotNetworkManager::chatMessageReceived(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2);
}

// SIGNAL 10
void HotspotNetworkManager::networkError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}
QT_WARNING_POP
