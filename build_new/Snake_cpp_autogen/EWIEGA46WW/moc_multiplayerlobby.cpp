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
        "onRefreshClicked",
        "onBackClicked",
        "onRoomSelectionChanged",
        "onPlayerNameChanged",
        "refreshRoomList",
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
        "port"
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
        // Slot 'onRefreshClicked'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBackClicked'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRoomSelectionChanged'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPlayerNameChanged'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'refreshRoomList'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRoomCreated'
        QtMocHelpers::SlotData<void(const QString &, const GameRoom &)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { 0x80000000 | 13, 14 },
        }}),
        // Slot 'onPlayerJoinedRoom'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::QString, 16 },
        }}),
        // Slot 'onPlayerLeftRoom'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::QString, 16 },
        }}),
        // Slot 'onGameStarted'
        QtMocHelpers::SlotData<void(const QString &)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 12 },
        }}),
        // Slot 'onGameEnded'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::QString, 20 },
        }}),
        // Slot 'onRoomDestroyed'
        QtMocHelpers::SlotData<void(const QString &)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 12 },
        }}),
        // Slot 'onRoomDiscovered'
        QtMocHelpers::SlotData<void(const QString &, const QString &, int)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::QString, 23 }, { QMetaType::Int, 24 },
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
        case 4: _t->onRefreshClicked(); break;
        case 5: _t->onBackClicked(); break;
        case 6: _t->onRoomSelectionChanged(); break;
        case 7: _t->onPlayerNameChanged(); break;
        case 8: _t->refreshRoomList(); break;
        case 9: _t->onRoomCreated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<GameRoom>>(_a[2]))); break;
        case 10: _t->onPlayerJoinedRoom((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 11: _t->onPlayerLeftRoom((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 12: _t->onGameStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->onGameEnded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 14: _t->onRoomDestroyed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->onRoomDiscovered((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
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
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 16;
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
