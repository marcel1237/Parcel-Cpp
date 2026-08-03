/****************************************************************************
** Meta object code from reading C++ file 'OpenProjectView.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/view/OpenProjectView.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OpenProjectView.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN6Parcel4View15OpenProjectViewE_t {};
} // unnamed namespace

template <> constexpr inline auto Parcel::View::OpenProjectView::qt_create_metaobjectdata<qt_meta_tag_ZN6Parcel4View15OpenProjectViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Parcel::View::OpenProjectView",
        "handleBrowse",
        "",
        "analyzeProject",
        "path",
        "extractBuildDetails",
        "std::filesystem::path",
        "projectPath",
        "extractIDEProjectDetails",
        "extractDependencies",
        "formatSize",
        "bytes",
        "handleOpen",
        "handleCancel"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'handleBrowse'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'analyzeProject'
        QtMocHelpers::SlotData<void(const QString &)>(3, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Slot 'extractBuildDetails'
        QtMocHelpers::SlotData<QString(const std::filesystem::path &)>(5, 2, QMC::AccessPrivate, QMetaType::QString, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Slot 'extractIDEProjectDetails'
        QtMocHelpers::SlotData<QString(const std::filesystem::path &)>(8, 2, QMC::AccessPrivate, QMetaType::QString, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Slot 'extractDependencies'
        QtMocHelpers::SlotData<QString(const std::filesystem::path &)>(9, 2, QMC::AccessPrivate, QMetaType::QString, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Slot 'formatSize'
        QtMocHelpers::SlotData<QString(long long)>(10, 2, QMC::AccessPrivate, QMetaType::QString, {{
            { QMetaType::LongLong, 11 },
        }}),
        // Slot 'handleOpen'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleCancel'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<OpenProjectView, qt_meta_tag_ZN6Parcel4View15OpenProjectViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Parcel::View::OpenProjectView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Parcel4View15OpenProjectViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Parcel4View15OpenProjectViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6Parcel4View15OpenProjectViewE_t>.metaTypes,
    nullptr
} };

void Parcel::View::OpenProjectView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<OpenProjectView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->handleBrowse(); break;
        case 1: _t->analyzeProject((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: { QString _r = _t->extractBuildDetails((*reinterpret_cast<std::add_pointer_t<std::filesystem::path>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 3: { QString _r = _t->extractIDEProjectDetails((*reinterpret_cast<std::add_pointer_t<std::filesystem::path>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 4: { QString _r = _t->extractDependencies((*reinterpret_cast<std::add_pointer_t<std::filesystem::path>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 5: { QString _r = _t->formatSize((*reinterpret_cast<std::add_pointer_t<qlonglong>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 6: _t->handleOpen(); break;
        case 7: _t->handleCancel(); break;
        default: ;
        }
    }
}

const QMetaObject *Parcel::View::OpenProjectView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Parcel::View::OpenProjectView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6Parcel4View15OpenProjectViewE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Parcel::View::OpenProjectView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
