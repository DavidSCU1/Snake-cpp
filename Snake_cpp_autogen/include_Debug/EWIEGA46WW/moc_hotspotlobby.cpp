/****************************************************************************
** Meta object code from reading C++ file 'hotspotlobby.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../hotspotlobby.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hotspotlobby.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12HotspotLobbyE_t {};
} // unnamed namespace

template <> constexpr inline auto HotspotLobby::qt_create_metaobjectdata<qt_meta_tag_ZN12HotspotLobbyE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "HotspotLobby",
        "gameStartRequested",
        "",
        "backToMainMenu",
        "characterSelectionRequested",
        "onHostStarted",
        "roomName",
        "ipAddress",
        "onHostStopped",
        "onHostDiscovered",
        "hostAddress",
        "playerCount",
        "maxPlayers",
        "onConnectedToHost",
        "onDisconnectedFromHost",
        "onNetworkError",
        "errorMessage",
        "onPlayerJoined",
        "playerName",
        "onPlayerLeft",
        "onPlayerCharacterChanged",
        "CharacterType",
        "character",
        "onPlayerReadyChanged",
        "ready",
        "onGameStarted",
        "onGameEnded",
        "winner",
        "onCountdownUpdated",
        "seconds",
        "onGameStateUpdated",
        "HotspotGameState",
        "gameState",
        "onCreateRoomClicked",
        "onJoinRoomClicked",
        "onRefreshRoomsClicked",
        "onStartGameClicked",
        "onLeaveRoomClicked",
        "onBackClicked",
        "onPlayerNameChanged",
        "onCharacterSelectionChanged",
        "onReadyToggled",
        "onSendChatMessage",
        "onRefreshTimer",
        "onNetworkStatusTimer"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'gameStartRequested'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'backToMainMenu'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'characterSelectionRequested'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onHostStarted'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(5, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onHostStopped'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onHostDiscovered'
        QtMocHelpers::SlotData<void(const QString &, const QString &, int, int)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 10 }, { QMetaType::QString, 6 }, { QMetaType::Int, 11 }, { QMetaType::Int, 12 },
        }}),
        // Slot 'onConnectedToHost'
        QtMocHelpers::SlotData<void(const QString &)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Slot 'onDisconnectedFromHost'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onNetworkError'
        QtMocHelpers::SlotData<void(const QString &)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 16 },
        }}),
        // Slot 'onPlayerJoined'
        QtMocHelpers::SlotData<void(const QString &)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Slot 'onPlayerLeft'
        QtMocHelpers::SlotData<void(const QString &)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Slot 'onPlayerCharacterChanged'
        QtMocHelpers::SlotData<void(const QString &, CharacterType)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 18 }, { 0x80000000 | 21, 22 },
        }}),
        // Slot 'onPlayerReadyChanged'
        QtMocHelpers::SlotData<void(const QString &, bool)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 18 }, { QMetaType::Bool, 24 },
        }}),
        // Slot 'onGameStarted'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGameEnded'
        QtMocHelpers::SlotData<void(const QString &)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 27 },
        }}),
        // Slot 'onCountdownUpdated'
        QtMocHelpers::SlotData<void(int)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 29 },
        }}),
        // Slot 'onGameStateUpdated'
        QtMocHelpers::SlotData<void(const HotspotGameState &)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 31, 32 },
        }}),
        // Slot 'onCreateRoomClicked'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onJoinRoomClicked'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRefreshRoomsClicked'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStartGameClicked'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLeaveRoomClicked'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBackClicked'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPlayerNameChanged'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCharacterSelectionChanged'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onReadyToggled'
        QtMocHelpers::SlotData<void(bool)>(41, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 24 },
        }}),
        // Slot 'onSendChatMessage'
        QtMocHelpers::SlotData<void()>(42, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRefreshTimer'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onNetworkStatusTimer'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<HotspotLobby, qt_meta_tag_ZN12HotspotLobbyE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject HotspotLobby::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12HotspotLobbyE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12HotspotLobbyE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12HotspotLobbyE_t>.metaTypes,
    nullptr
} };

void HotspotLobby::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<HotspotLobby *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->gameStartRequested(); break;
        case 1: _t->backToMainMenu(); break;
        case 2: _t->characterSelectionRequested(); break;
        case 3: _t->onHostStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->onHostStopped(); break;
        case 5: _t->onHostDiscovered((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4]))); break;
        case 6: _t->onConnectedToHost((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onDisconnectedFromHost(); break;
        case 8: _t->onNetworkError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->onPlayerJoined((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->onPlayerLeft((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->onPlayerCharacterChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<CharacterType>>(_a[2]))); break;
        case 12: _t->onPlayerReadyChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 13: _t->onGameStarted(); break;
        case 14: _t->onGameEnded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->onCountdownUpdated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 16: _t->onGameStateUpdated((*reinterpret_cast< std::add_pointer_t<HotspotGameState>>(_a[1]))); break;
        case 17: _t->onCreateRoomClicked(); break;
        case 18: _t->onJoinRoomClicked(); break;
        case 19: _t->onRefreshRoomsClicked(); break;
        case 20: _t->onStartGameClicked(); break;
        case 21: _t->onLeaveRoomClicked(); break;
        case 22: _t->onBackClicked(); break;
        case 23: _t->onPlayerNameChanged(); break;
        case 24: _t->onCharacterSelectionChanged(); break;
        case 25: _t->onReadyToggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 26: _t->onSendChatMessage(); break;
        case 27: _t->onRefreshTimer(); break;
        case 28: _t->onNetworkStatusTimer(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (HotspotLobby::*)()>(_a, &HotspotLobby::gameStartRequested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotLobby::*)()>(_a, &HotspotLobby::backToMainMenu, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (HotspotLobby::*)()>(_a, &HotspotLobby::characterSelectionRequested, 2))
            return;
    }
}

const QMetaObject *HotspotLobby::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HotspotLobby::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12HotspotLobbyE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int HotspotLobby::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 29;
    }
    return _id;
}

// SIGNAL 0
void HotspotLobby::gameStartRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void HotspotLobby::backToMainMenu()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void HotspotLobby::characterSelectionRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
