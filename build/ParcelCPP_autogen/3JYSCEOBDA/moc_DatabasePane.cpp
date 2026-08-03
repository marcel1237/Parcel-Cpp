/****************************************************************************
** Meta object code from reading C++ file 'DatabasePane.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/view/DatabasePane.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DatabasePane.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
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
struct qt_meta_tag_ZN6Parcel4View12DatabasePaneE_t {};
} // unnamed namespace

template <> constexpr inline auto Parcel::View::DatabasePane::qt_create_metaobjectdata<qt_meta_tag_ZN6Parcel4View12DatabasePaneE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Parcel::View::DatabasePane",
        "showChartMenu",
        "",
        "showBarChart",
        "showPieChart",
        "exportFullPdfReport",
        "handleOpenDB",
        "handleExecuteSQL",
        "updateTableList",
        "QSqlDatabase",
        "db",
        "loadTable",
        "index",
        "refreshData",
        "executeDuckQuery",
        "sql",
        "executeRedisQuery",
        "command",
        "executeLevelDBQuery",
        "executeRocksDBQuery",
        "executeBerkeleyDBQuery",
        "executeMongoQuery",
        "showTableContextMenu",
        "QPoint",
        "pos",
        "exportToCSV",
        "importFromCSV",
        "setupCompleter"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'showChartMenu'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showBarChart'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showPieChart'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'exportFullPdfReport'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleOpenDB'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleExecuteSQL'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateTableList'
        QtMocHelpers::SlotData<void(QSqlDatabase)>(8, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Slot 'loadTable'
        QtMocHelpers::SlotData<void(int)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 12 },
        }}),
        // Slot 'refreshData'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'executeDuckQuery'
        QtMocHelpers::SlotData<void(const QString &)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Slot 'executeRedisQuery'
        QtMocHelpers::SlotData<void(const QString &)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
        // Slot 'executeLevelDBQuery'
        QtMocHelpers::SlotData<void(const QString &)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
        // Slot 'executeRocksDBQuery'
        QtMocHelpers::SlotData<void(const QString &)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
        // Slot 'executeBerkeleyDBQuery'
        QtMocHelpers::SlotData<void(const QString &)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
        // Slot 'executeMongoQuery'
        QtMocHelpers::SlotData<void(const QString &)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
        // Slot 'showTableContextMenu'
        QtMocHelpers::SlotData<void(const QPoint &)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 23, 24 },
        }}),
        // Slot 'exportToCSV'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'importFromCSV'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'setupCompleter'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DatabasePane, qt_meta_tag_ZN6Parcel4View12DatabasePaneE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Parcel::View::DatabasePane::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Parcel4View12DatabasePaneE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Parcel4View12DatabasePaneE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6Parcel4View12DatabasePaneE_t>.metaTypes,
    nullptr
} };

void Parcel::View::DatabasePane::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DatabasePane *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->showChartMenu(); break;
        case 1: _t->showBarChart(); break;
        case 2: _t->showPieChart(); break;
        case 3: _t->exportFullPdfReport(); break;
        case 4: _t->handleOpenDB(); break;
        case 5: _t->handleExecuteSQL(); break;
        case 6: _t->updateTableList((*reinterpret_cast<std::add_pointer_t<QSqlDatabase>>(_a[1]))); break;
        case 7: _t->loadTable((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->refreshData(); break;
        case 9: _t->executeDuckQuery((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->executeRedisQuery((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->executeLevelDBQuery((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->executeRocksDBQuery((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->executeBerkeleyDBQuery((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: _t->executeMongoQuery((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->showTableContextMenu((*reinterpret_cast<std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 16: _t->exportToCSV(); break;
        case 17: _t->importFromCSV(); break;
        case 18: _t->setupCompleter(); break;
        default: ;
        }
    }
}

const QMetaObject *Parcel::View::DatabasePane::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Parcel::View::DatabasePane::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Parcel4View12DatabasePaneE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Parcel::View::DatabasePane::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 19;
    }
    return _id;
}
QT_WARNING_POP
