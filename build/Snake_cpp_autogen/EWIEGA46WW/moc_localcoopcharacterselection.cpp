/****************************************************************************
** Meta object code from reading C++ file 'localcoopcharacterselection.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../localcoopcharacterselection.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'localcoopcharacterselection.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN27LocalCoopCharacterSelectionE_t {};
} // unnamed namespace

template <> constexpr inline auto LocalCoopCharacterSelection::qt_create_metaobjectdata<qt_meta_tag_ZN27LocalCoopCharacterSelectionE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LocalCoopCharacterSelection",
        "startLocalCoopGame",
        "",
        "CharacterType",
        "player1Character",
        "player2Character",
        "backToModeSelection",
        "onPlayer1CharacterSelected",
        "character",
        "onPlayer2CharacterSelected",
        "onStartGameClicked",
        "onBackClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'startLocalCoopGame'
        QtMocHelpers::SignalData<void(CharacterType, CharacterType)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 3, 5 },
        }}),
        // Signal 'backToModeSelection'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onPlayer1CharacterSelected'
        QtMocHelpers::SlotData<void(CharacterType)>(7, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 8 },
        }}),
        // Slot 'onPlayer2CharacterSelected'
        QtMocHelpers::SlotData<void(CharacterType)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 8 },
        }}),
        // Slot 'onStartGameClicked'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBackClicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LocalCoopCharacterSelection, qt_meta_tag_ZN27LocalCoopCharacterSelectionE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LocalCoopCharacterSelection::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN27LocalCoopCharacterSelectionE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN27LocalCoopCharacterSelectionE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN27LocalCoopCharacterSelectionE_t>.metaTypes,
    nullptr
} };

void LocalCoopCharacterSelection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LocalCoopCharacterSelection *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->startLocalCoopGame((*reinterpret_cast< std::add_pointer_t<CharacterType>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<CharacterType>>(_a[2]))); break;
        case 1: _t->backToModeSelection(); break;
        case 2: _t->onPlayer1CharacterSelected((*reinterpret_cast< std::add_pointer_t<CharacterType>>(_a[1]))); break;
        case 3: _t->onPlayer2CharacterSelected((*reinterpret_cast< std::add_pointer_t<CharacterType>>(_a[1]))); break;
        case 4: _t->onStartGameClicked(); break;
        case 5: _t->onBackClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (LocalCoopCharacterSelection::*)(CharacterType , CharacterType )>(_a, &LocalCoopCharacterSelection::startLocalCoopGame, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (LocalCoopCharacterSelection::*)()>(_a, &LocalCoopCharacterSelection::backToModeSelection, 1))
            return;
    }
}

const QMetaObject *LocalCoopCharacterSelection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LocalCoopCharacterSelection::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN27LocalCoopCharacterSelectionE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int LocalCoopCharacterSelection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void LocalCoopCharacterSelection::startLocalCoopGame(CharacterType _t1, CharacterType _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void LocalCoopCharacterSelection::backToModeSelection()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
