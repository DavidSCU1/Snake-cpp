/****************************************************************************
** Meta object code from reading C++ file 'multiplayerlobby.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../multiplayerlobby.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'multiplayerlobby.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16MultiPlayerLobbyE_t {};
} // unnamed namespace

template <> constexpr inline auto MultiPlayerLobby::qt_create_metaobjectdata<qt_meta_tag_ZN16MultiPlayerLobbyE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MultiPlayerLobby",
        "backToMenu",
        "",
        "gameStarted",
        "onCreateRoomClicked",
        "onJoinRoomClicked",
        "onManualConnectClicked",
        "onRefreshClicked",
        "onBackClicked",
        "onRoomSelectionChanged",
        "onPlayerNameChanged",
        "refreshRoomList",
        "onStartGameClicked",
        "onLeaveRoomClicked",
        "showWaitingInterface",
        "hideWaitingInterface",
        "showCharacterSelection",
        "hideCharacterSelection",
        "onCharacterSelected",
        "CharacterType",
        "character",
        "onCharacterSelectionStart",
        "onCharacterSelectionBack",
        "onRoomCreated",
        "roomId",
        "GameRoom",
        "room",
        "onPlayerJoinedRoom",
        "playerName",
        "onPlayerLeftRoom",
        "onGameStarted",
        "onGameEnded",
        "winner",
        "onRoomDestroyed",
        "onRoomDiscovered",
        "host",
        "port",
        "onConnectionError",
        "error",
        "onCharacterSelectionStarted",
        "onPlayerCharacterSelected",
        "onAllPlayersReady",
        "onPlayerReadyChanged",
        "ready",
        "onAllPlayersReadyInSelection",
        "onPlayerReadyReceived",
        "onGameCountdownReceived",
        "countdown"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'backToMenu'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'gameStarted'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onCreateRoomClicked'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onJoinRoomClicked'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onManualConnectClicked'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRefreshClicked'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBackClicked'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRoomSelectionChanged'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPlayerNameChanged'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'refreshRoomList'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStartGameClicked'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLeaveRoomClicked'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showWaitingInterface'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'hideWaitingInterface'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showCharacterSelection'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'hideCharacterSelection'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCharacterSelected'
        QtMocHelpers::SlotData<void(CharacterType)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 19, 20 },
        }}),
        // Slot 'onCharacterSelectionStart'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCharacterSelectionBack'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRoomCreated'
        QtMocHelpers::SlotData<void(const QString &, const GameRoom &)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 }, { 0x80000000 | 25, 26 },
        }}),
        // Slot 'onPlayerJoinedRoom'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 }, { QMetaType::QString, 28 },
        }}),
        // Slot 'onPlayerLeftRoom'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(29, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 }, { QMetaType::QString, 28 },
        }}),
        // Slot 'onGameStarted'
        QtMocHelpers::SlotData<void(const QString &)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 },
        }}),
        // Slot 'onGameEnded'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 }, { QMetaType::QString, 32 },
        }}),
        // Slot 'onRoomDestroyed'
        QtMocHelpers::SlotData<void(const QString &)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 },
        }}),
        // Slot 'onRoomDiscovered'
        QtMocHelpers::SlotData<void(const QString &, int)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 35 }, { QMetaType::Int, 36 },
        }}),
        // Slot 'onConnectionError'
        QtMocHelpers::SlotData<void(const QString &)>(37, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 38 },
        }}),
        // Slot 'onCharacterSelectionStarted'
        QtMocHelpers::SlotData<void(const QString &)>(39, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 },
        }}),
        // Slot 'onPlayerCharacterSelected'
        QtMocHelpers::SlotData<void(const QString &, const QString &, int)>(40, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 }, { QMetaType::QString, 28 }, { QMetaType::Int, 20 },
        }}),
        // Slot 'onAllPlayersReady'
        QtMocHelpers::SlotData<void(const QString &)>(41, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 },
        }}),
        // Slot 'onPlayerReadyChanged'
        QtMocHelpers::SlotData<void(bool)>(42, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 43 },
        }}),
        // Slot 'onAllPlayersReadyInSelection'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPlayerReadyReceived'
        QtMocHelpers::SlotData<void(const QString &, const QString &, bool)>(45, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 }, { QMetaType::QString, 28 }, { QMetaType::Bool, 43 },
        }}),
        // Slot 'onGameCountdownReceived'
        QtMocHelpers::SlotData<void(const QString &, int)>(46, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 }, { QMetaType::Int, 47 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MultiPlayerLobby, qt_meta_tag_ZN16MultiPlayerLobbyE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MultiPlayerLobby::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16MultiPlayerLobbyE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16MultiPlayerLobbyE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16MultiPlayerLobbyE_t>.metaTypes,
    nullptr
} };

void MultiPlayerLobby::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MultiPlayerLobby *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->backToMenu(); break;
        case 1: _t->gameStarted(); break;
        case 2: _t->onCreateRoomClicked(); break;
        case 3: _t->onJoinRoomClicked(); break;
        case 4: _t->onManualConnectClicked(); break;
        case 5: _t->onRefreshClicked(); break;
        case 6: _t->onBackClicked(); break;
        case 7: _t->onRoomSelectionChanged(); break;
        case 8: _t->onPlayerNameChanged(); break;
        case 9: _t->refreshRoomList(); break;
        case 10: _t->onStartGameClicked(); break;
        case 11: _t->onLeaveRoomClicked(); break;
        case 12: _t->showWaitingInterface(); break;
        case 13: _t->hideWaitingInterface(); break;
        case 14: _t->showCharacterSelection(); break;
        case 15: _t->hideCharacterSelection(); break;
        case 16: _t->onCharacterSelected((*reinterpret_cast< std::add_pointer_t<CharacterType>>(_a[1]))); break;
        case 17: _t->onCharacterSelectionStart(); break;
        case 18: _t->onCharacterSelectionBack(); break;
        case 19: _t->onRoomCreated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<GameRoom>>(_a[2]))); break;
        case 20: _t->onPlayerJoinedRoom((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 21: _t->onPlayerLeftRoom((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 22: _t->onGameStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 23: _t->onGameEnded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 24: _t->onRoomDestroyed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 25: _t->onRoomDiscovered((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 26: _t->onConnectionError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 27: _t->onCharacterSelectionStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 28: _t->onPlayerCharacterSelected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 29: _t->onAllPlayersReady((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 30: _t->onPlayerReadyChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 31: _t->onAllPlayersReadyInSelection(); break;
        case 32: _t->onPlayerReadyReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 33: _t->onGameCountdownReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MultiPlayerLobby::*)()>(_a, &MultiPlayerLobby::backToMenu, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MultiPlayerLobby::*)()>(_a, &MultiPlayerLobby::gameStarted, 1))
            return;
    }
}

const QMetaObject *MultiPlayerLobby::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MultiPlayerLobby::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16MultiPlayerLobbyE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int MultiPlayerLobby::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 34)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 34;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 34)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 34;
    }
    return _id;
}

// SIGNAL 0
void MultiPlayerLobby::backToMenu()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MultiPlayerLobby::gameStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
