/****************************************************************************
** Meta object code from reading C++ file 'chart_editor_widget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ui/chart_editor_widget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'chart_editor_widget.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN5ibmsc17ChartEditorWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto ibmsc::ChartEditorWidget::qt_create_metaobjectdata<qt_meta_tag_ZN5ibmsc17ChartEditorWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ibmsc::ChartEditorWidget",
        "aboutToEdit",
        "",
        "notePicked",
        "BmsNote",
        "note",
        "documentEdited",
        "selectionChanged",
        "count",
        "focusEntered",
        "ibmsc::ChartEditorWidget*",
        "editor",
        "timeSelectionChanged",
        "start",
        "length"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'aboutToEdit'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'notePicked'
        QtMocHelpers::SignalData<void(const BmsNote &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 5 },
        }}),
        // Signal 'documentEdited'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'selectionChanged'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Signal 'focusEntered'
        QtMocHelpers::SignalData<void(ibmsc::ChartEditorWidget *)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Signal 'timeSelectionChanged'
        QtMocHelpers::SignalData<void(double, double)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 13 }, { QMetaType::Double, 14 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ChartEditorWidget, qt_meta_tag_ZN5ibmsc17ChartEditorWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ibmsc::ChartEditorWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5ibmsc17ChartEditorWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5ibmsc17ChartEditorWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN5ibmsc17ChartEditorWidgetE_t>.metaTypes,
    nullptr
} };

void ibmsc::ChartEditorWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ChartEditorWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->aboutToEdit(); break;
        case 1: _t->notePicked((*reinterpret_cast< std::add_pointer_t<BmsNote>>(_a[1]))); break;
        case 2: _t->documentEdited(); break;
        case 3: _t->selectionChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->focusEntered((*reinterpret_cast< std::add_pointer_t<ibmsc::ChartEditorWidget*>>(_a[1]))); break;
        case 5: _t->timeSelectionChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< ibmsc::ChartEditorWidget* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ChartEditorWidget::*)()>(_a, &ChartEditorWidget::aboutToEdit, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChartEditorWidget::*)(const BmsNote & )>(_a, &ChartEditorWidget::notePicked, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChartEditorWidget::*)()>(_a, &ChartEditorWidget::documentEdited, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChartEditorWidget::*)(int )>(_a, &ChartEditorWidget::selectionChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChartEditorWidget::*)(ibmsc::ChartEditorWidget * )>(_a, &ChartEditorWidget::focusEntered, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ChartEditorWidget::*)(double , double )>(_a, &ChartEditorWidget::timeSelectionChanged, 5))
            return;
    }
}

const QMetaObject *ibmsc::ChartEditorWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ibmsc::ChartEditorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5ibmsc17ChartEditorWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ibmsc::ChartEditorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ibmsc::ChartEditorWidget::aboutToEdit()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ibmsc::ChartEditorWidget::notePicked(const BmsNote & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ibmsc::ChartEditorWidget::documentEdited()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ibmsc::ChartEditorWidget::selectionChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void ibmsc::ChartEditorWidget::focusEntered(ibmsc::ChartEditorWidget * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void ibmsc::ChartEditorWidget::timeSelectionChanged(double _t1, double _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}
QT_WARNING_POP
