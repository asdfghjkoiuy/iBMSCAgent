/****************************************************************************
** Meta object code from reading C++ file 'main_window.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ui/main_window.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
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
struct qt_meta_tag_ZN5ibmsc10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto ibmsc::MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN5ibmsc10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ibmsc::MainWindow",
        "onOpen",
        "",
        "onSave",
        "onSaveAs",
        "onThemeChanged",
        "index",
        "onPlaySelectedAudio",
        "onStopAudio",
        "onWavSelectionChanged",
        "onBmpSelectionChanged",
        "onHeaderEdited",
        "onChartEdited",
        "onUndo",
        "onRedo",
        "onModeSelect",
        "onModeWrite",
        "onModeTimeSelect",
        "onEditorFocus",
        "ibmsc::ChartEditorWidget*",
        "editor",
        "onEditorSelectionChanged",
        "count",
        "onEditorTimeSelectionChanged",
        "start",
        "length",
        "onEditorAboutToEdit",
        "onOpenRecent",
        "onStatistics",
        "onFindByValue",
        "onReplaceValue",
        "onMyO2",
        "onAutoSaveTick",
        "onToggleLeftSplit",
        "checked",
        "onToggleRightSplit"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onOpen'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSave'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveAs'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onThemeChanged'
        QtMocHelpers::SlotData<void(int)>(5, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Slot 'onPlaySelectedAudio'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStopAudio'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onWavSelectionChanged'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBmpSelectionChanged'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onHeaderEdited'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onChartEdited'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onUndo'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRedo'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModeSelect'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModeWrite'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onModeTimeSelect'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onEditorFocus'
        QtMocHelpers::SlotData<void(ibmsc::ChartEditorWidget *)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 19, 20 },
        }}),
        // Slot 'onEditorSelectionChanged'
        QtMocHelpers::SlotData<void(int)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 22 },
        }}),
        // Slot 'onEditorTimeSelectionChanged'
        QtMocHelpers::SlotData<void(double, double)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 24 }, { QMetaType::Double, 25 },
        }}),
        // Slot 'onEditorAboutToEdit'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenRecent'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStatistics'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onFindByValue'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onReplaceValue'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMyO2'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAutoSaveTick'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onToggleLeftSplit'
        QtMocHelpers::SlotData<void(bool)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 34 },
        }}),
        // Slot 'onToggleRightSplit'
        QtMocHelpers::SlotData<void(bool)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 34 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN5ibmsc10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ibmsc::MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5ibmsc10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5ibmsc10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN5ibmsc10MainWindowE_t>.metaTypes,
    nullptr
} };

void ibmsc::MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onOpen(); break;
        case 1: _t->onSave(); break;
        case 2: _t->onSaveAs(); break;
        case 3: _t->onThemeChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->onPlaySelectedAudio(); break;
        case 5: _t->onStopAudio(); break;
        case 6: _t->onWavSelectionChanged(); break;
        case 7: _t->onBmpSelectionChanged(); break;
        case 8: _t->onHeaderEdited(); break;
        case 9: _t->onChartEdited(); break;
        case 10: _t->onUndo(); break;
        case 11: _t->onRedo(); break;
        case 12: _t->onModeSelect(); break;
        case 13: _t->onModeWrite(); break;
        case 14: _t->onModeTimeSelect(); break;
        case 15: _t->onEditorFocus((*reinterpret_cast< std::add_pointer_t<ibmsc::ChartEditorWidget*>>(_a[1]))); break;
        case 16: _t->onEditorSelectionChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->onEditorTimeSelectionChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 18: _t->onEditorAboutToEdit(); break;
        case 19: _t->onOpenRecent(); break;
        case 20: _t->onStatistics(); break;
        case 21: _t->onFindByValue(); break;
        case 22: _t->onReplaceValue(); break;
        case 23: _t->onMyO2(); break;
        case 24: _t->onAutoSaveTick(); break;
        case 25: _t->onToggleLeftSplit((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 26: _t->onToggleRightSplit((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
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
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ibmsc::ChartEditorWidget* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *ibmsc::MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ibmsc::MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5ibmsc10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int ibmsc::MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    }
    return _id;
}
QT_WARNING_POP
