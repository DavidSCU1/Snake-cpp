/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "showMainMenu",
        "",
        "showCharacterSelection",
        "showDifficultySelection",
        "showMultiplayerMenu",
        "showMultiplayerLobby",
        "showMultiplayerModeSelection",
        "showLocalCoopCharacterSelection",
        "showHighScores",
        "showSingleModeSelection",
        "startSinglePlayerGame",
        "startMultiplayerHost",
        "startMultiplayerJoin",
        "onGameOver",
        "finalScore",
        "onCharacterSelected",
        "CharacterType",
        "character",
        "onDifficultySelected",
        "connectToServer",
        "onNetworkError",
        "error",
        "onSingleModeSelected",
        "SinglePlayerMode",
        "mode",
        "onMultiplayerModeSelected",
        "MultiplayerMode",
        "onLocalCoopCharactersSelected",
        "player1Character",
        "player2Character"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'showMainMenu'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showCharacterSelection'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showDifficultySelection'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showMultiplayerMenu'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showMultiplayerLobby'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showMultiplayerModeSelection'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showLocalCoopCharacterSelection'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showHighScores'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showSingleModeSelection'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startSinglePlayerGame'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startMultiplayerHost'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startMultiplayerJoin'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onGameOver'
        QtMocHelpers::SlotData<void(int)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
        // Slot 'onCharacterSelected'
        QtMocHelpers::SlotData<void(CharacterType)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 17, 18 },
        }}),
        // Slot 'onDifficultySelected'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'connectToServer'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onNetworkError'
        QtMocHelpers::SlotData<void(const QString &)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 22 },
        }}),
        // Slot 'onSingleModeSelected'
        QtMocHelpers::SlotData<void(SinglePlayerMode)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 24, 25 },
        }}),
        // Slot 'onMultiplayerModeSelected'
        QtMocHelpers::SlotData<void(MultiplayerMode)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 27, 25 },
        }}),
        // Slot 'onLocalCoopCharactersSelected'
        QtMocHelpers::SlotData<void(CharacterType, CharacterType)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 17, 29 }, { 0x80000000 | 17, 30 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->showMainMenu(); break;
        case 1: _t->showCharacterSelection(); break;
        case 2: _t->showDifficultySelection(); break;
        case 3: _t->showMultiplayerMenu(); break;
        case 4: _t->showMultiplayerLobby(); break;
        case 5: _t->showMultiplayerModeSelection(); break;
        case 6: _t->showLocalCoopCharacterSelection(); break;
        case 7: _t->showHighScores(); break;
        case 8: _t->showSingleModeSelection(); break;
        case 9: _t->startSinglePlayerGame(); break;
        case 10: _t->startMultiplayerHost(); break;
        case 11: _t->startMultiplayerJoin(); break;
        case 12: _t->onGameOver((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->onCharacterSelected((*reinterpret_cast< std::add_pointer_t<CharacterType>>(_a[1]))); break;
        case 14: _t->onDifficultySelected(); break;
        case 15: _t->connectToServer(); break;
        case 16: _t->onNetworkError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 17: _t->onSingleModeSelected((*reinterpret_cast< std::add_pointer_t<SinglePlayerMode>>(_a[1]))); break;
        case 18: _t->onMultiplayerModeSelected((*reinterpret_cast< std::add_pointer_t<MultiplayerMode>>(_a[1]))); break;
        case 19: _t->onLocalCoopCharactersSelected((*reinterpret_cast< std::add_pointer_t<CharacterType>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<CharacterType>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 20;
    }
    return _id;
}
QT_WARNING_POP
