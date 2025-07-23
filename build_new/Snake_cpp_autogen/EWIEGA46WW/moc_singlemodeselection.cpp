/****************************************************************************
** Meta object code from reading C++ file 'singlemodeselection.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../singlemodeselection.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'singlemodeselection.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN19SingleModeSelectionE_t {};
} // unnamed namespace

template <> constexpr inline auto SingleModeSelection::qt_create_metaobjectdata<qt_meta_tag_ZN19SingleModeSelectionE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SingleModeSelection",
        "modeSelected",
        "",
        "SinglePlayerMode",
        "mode",
        "CharacterType",
        "character",
        "backToMenu",
        "achievementsRequested",
        "characterSelected",
        "onModeButtonClicked",
        "onAchievementUnlocked",
        "Achievement",
        "achievement",
        "showAchievements",
        "animateButton",
        "QPushButton*",
        "button",
        "onCharacterButtonClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'modeSelected'
        QtMocHelpers::SignalData<void(SinglePlayerMode, CharacterType)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 5, 6 },
        }}),
        // Signal 'backToMenu'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'achievementsRequested'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'characterSelected'
        QtMocHelpers::SignalData<void(CharacterType)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Slot 'onModeButtonClicked'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAchievementUnlocked'
        QtMocHelpers::SlotData<void(const Achievement &)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
        // Slot 'showAchievements'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'animateButton'
        QtMocHelpers::SlotData<void(QPushButton *)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 16, 17 },
        }}),
        // Slot 'onCharacterButtonClicked'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SingleModeSelection, qt_meta_tag_ZN19SingleModeSelectionE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SingleModeSelection::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19SingleModeSelectionE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19SingleModeSelectionE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN19SingleModeSelectionE_t>.metaTypes,
    nullptr
} };

void SingleModeSelection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SingleModeSelection *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->modeSelected((*reinterpret_cast< std::add_pointer_t<SinglePlayerMode>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<CharacterType>>(_a[2]))); break;
        case 1: _t->backToMenu(); break;
        case 2: _t->achievementsRequested(); break;
        case 3: _t->characterSelected((*reinterpret_cast< std::add_pointer_t<CharacterType>>(_a[1]))); break;
        case 4: _t->onModeButtonClicked(); break;
        case 5: _t->onAchievementUnlocked((*reinterpret_cast< std::add_pointer_t<Achievement>>(_a[1]))); break;
        case 6: _t->showAchievements(); break;
        case 7: _t->animateButton((*reinterpret_cast< std::add_pointer_t<QPushButton*>>(_a[1]))); break;
        case 8: _t->onCharacterButtonClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QPushButton* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SingleModeSelection::*)(SinglePlayerMode , CharacterType )>(_a, &SingleModeSelection::modeSelected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SingleModeSelection::*)()>(_a, &SingleModeSelection::backToMenu, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SingleModeSelection::*)()>(_a, &SingleModeSelection::achievementsRequested, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SingleModeSelection::*)(CharacterType )>(_a, &SingleModeSelection::characterSelected, 3))
            return;
    }
}

const QMetaObject *SingleModeSelection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SingleModeSelection::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19SingleModeSelectionE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SingleModeSelection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void SingleModeSelection::modeSelected(SinglePlayerMode _t1, CharacterType _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void SingleModeSelection::backToMenu()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SingleModeSelection::achievementsRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void SingleModeSelection::characterSelected(CharacterType _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN17AchievementWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto AchievementWidget::qt_create_metaobjectdata<qt_meta_tag_ZN17AchievementWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AchievementWidget"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AchievementWidget, qt_meta_tag_ZN17AchievementWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AchievementWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17AchievementWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17AchievementWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17AchievementWidgetE_t>.metaTypes,
    nullptr
} };

void AchievementWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AchievementWidget *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *AchievementWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AchievementWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17AchievementWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int AchievementWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
