/****************************************************************************
** Meta object code from reading C++ file 'notestore.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../notes/notestore.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'notestore.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.12.0. It"
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
struct qt_meta_tag_ZN9NoteStoreE_t {};
} // unnamed namespace

template <> constexpr inline auto NoteStore::qt_create_metaobjectdata<qt_meta_tag_ZN9NoteStoreE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NoteStore",
        "currentChanged",
        "",
        "titleChanged",
        "metaChanged",
        "breadcrumbsChanged",
        "pagesChanged",
        "trashChanged",
        "wordCountChanged",
        "statusChanged",
        "create",
        "parentId",
        "open",
        "id",
        "save",
        "setTitle",
        "title",
        "rename",
        "setIcon",
        "icon",
        "setFavorite",
        "favorite",
        "duplicate",
        "move",
        "moveSibling",
        "delta",
        "moveTargets",
        "QVariantList",
        "toggleExpanded",
        "pageTitle",
        "pageIcon",
        "parentOf",
        "exists",
        "addSubpageBlock",
        "row",
        "search",
        "query",
        "exportCurrent",
        "trash",
        "restore",
        "deleteForever",
        "tree",
        "PageTreeModel*",
        "blocks",
        "BlockModel*",
        "currentId",
        "updatedText",
        "breadcrumbs",
        "favorites",
        "recentPages",
        "trashPages",
        "pageCount",
        "wordCount",
        "status"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'currentChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'titleChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'metaChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'breadcrumbsChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'pagesChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'trashChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wordCountChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'statusChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'create'
        QtMocHelpers::MethodData<QString(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 11 },
        }}),
        // Method 'create'
        QtMocHelpers::MethodData<QString()>(10, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::QString),
        // Method 'open'
        QtMocHelpers::MethodData<bool(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 13 },
        }}),
        // Method 'save'
        QtMocHelpers::MethodData<bool()>(14, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'setTitle'
        QtMocHelpers::MethodData<void(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 16 },
        }}),
        // Method 'rename'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 13 }, { QMetaType::QString, 16 },
        }}),
        // Method 'setIcon'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 13 }, { QMetaType::QString, 19 },
        }}),
        // Method 'setFavorite'
        QtMocHelpers::MethodData<bool(const QString &, bool)>(20, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 13 }, { QMetaType::Bool, 21 },
        }}),
        // Method 'duplicate'
        QtMocHelpers::MethodData<QString(const QString &)>(22, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 13 },
        }}),
        // Method 'move'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(23, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 13 }, { QMetaType::QString, 11 },
        }}),
        // Method 'moveSibling'
        QtMocHelpers::MethodData<bool(const QString &, int)>(24, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 13 }, { QMetaType::Int, 25 },
        }}),
        // Method 'moveTargets'
        QtMocHelpers::MethodData<QVariantList(const QString &) const>(26, 2, QMC::AccessPublic, 0x80000000 | 27, {{
            { QMetaType::QString, 13 },
        }}),
        // Method 'toggleExpanded'
        QtMocHelpers::MethodData<void(const QString &)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Method 'pageTitle'
        QtMocHelpers::MethodData<QString(const QString &) const>(29, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 13 },
        }}),
        // Method 'pageIcon'
        QtMocHelpers::MethodData<QString(const QString &) const>(30, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 13 },
        }}),
        // Method 'parentOf'
        QtMocHelpers::MethodData<QString(const QString &) const>(31, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 13 },
        }}),
        // Method 'exists'
        QtMocHelpers::MethodData<bool(const QString &) const>(32, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 13 },
        }}),
        // Method 'addSubpageBlock'
        QtMocHelpers::MethodData<QString(int)>(33, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::Int, 34 },
        }}),
        // Method 'search'
        QtMocHelpers::MethodData<QVariantList(const QString &) const>(35, 2, QMC::AccessPublic, 0x80000000 | 27, {{
            { QMetaType::QString, 36 },
        }}),
        // Method 'exportCurrent'
        QtMocHelpers::MethodData<QString()>(37, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'trash'
        QtMocHelpers::MethodData<bool(const QString &)>(38, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 13 },
        }}),
        // Method 'restore'
        QtMocHelpers::MethodData<bool(const QString &)>(39, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 13 },
        }}),
        // Method 'deleteForever'
        QtMocHelpers::MethodData<bool(const QString &)>(40, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 13 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'tree'
        QtMocHelpers::PropertyData<PageTreeModel*>(41, 0x80000000 | 42, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'blocks'
        QtMocHelpers::PropertyData<BlockModel*>(43, 0x80000000 | 44, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'currentId'
        QtMocHelpers::PropertyData<QString>(45, QMetaType::QString, QMC::DefaultPropertyFlags, 0),
        // property 'title'
        QtMocHelpers::PropertyData<QString>(16, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'icon'
        QtMocHelpers::PropertyData<QString>(19, QMetaType::QString, QMC::DefaultPropertyFlags, 2),
        // property 'favorite'
        QtMocHelpers::PropertyData<bool>(21, QMetaType::Bool, QMC::DefaultPropertyFlags, 2),
        // property 'updatedText'
        QtMocHelpers::PropertyData<QString>(46, QMetaType::QString, QMC::DefaultPropertyFlags, 2),
        // property 'breadcrumbs'
        QtMocHelpers::PropertyData<QVariantList>(47, 0x80000000 | 27, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 3),
        // property 'favorites'
        QtMocHelpers::PropertyData<QVariantList>(48, 0x80000000 | 27, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 4),
        // property 'recentPages'
        QtMocHelpers::PropertyData<QVariantList>(49, 0x80000000 | 27, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 4),
        // property 'trashPages'
        QtMocHelpers::PropertyData<QVariantList>(50, 0x80000000 | 27, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 5),
        // property 'pageCount'
        QtMocHelpers::PropertyData<int>(51, QMetaType::Int, QMC::DefaultPropertyFlags, 4),
        // property 'wordCount'
        QtMocHelpers::PropertyData<int>(52, QMetaType::Int, QMC::DefaultPropertyFlags, 6),
        // property 'status'
        QtMocHelpers::PropertyData<QString>(53, QMetaType::QString, QMC::DefaultPropertyFlags, 7),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NoteStore, qt_meta_tag_ZN9NoteStoreE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NoteStore::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9NoteStoreE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9NoteStoreE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9NoteStoreE_t>.metaTypes,
    nullptr
} };

void NoteStore::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NoteStore *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->currentChanged(); break;
        case 1: _t->titleChanged(); break;
        case 2: _t->metaChanged(); break;
        case 3: _t->breadcrumbsChanged(); break;
        case 4: _t->pagesChanged(); break;
        case 5: _t->trashChanged(); break;
        case 6: _t->wordCountChanged(); break;
        case 7: _t->statusChanged(); break;
        case 8: { QString _r = _t->create((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 9: { QString _r = _t->create();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 10: { bool _r = _t->open((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 11: { bool _r = _t->save();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 12: _t->setTitle((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: { bool _r = _t->rename((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 14: { bool _r = _t->setIcon((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 15: { bool _r = _t->setFavorite((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 16: { QString _r = _t->duplicate((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 17: { bool _r = _t->move((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 18: { bool _r = _t->moveSibling((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 19: { QVariantList _r = _t->moveTargets((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 20: _t->toggleExpanded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 21: { QString _r = _t->pageTitle((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 22: { QString _r = _t->pageIcon((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 23: { QString _r = _t->parentOf((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 24: { bool _r = _t->exists((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 25: { QString _r = _t->addSubpageBlock((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 26: { QVariantList _r = _t->search((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 27: { QString _r = _t->exportCurrent();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 28: { bool _r = _t->trash((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 29: { bool _r = _t->restore((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 30: { bool _r = _t->deleteForever((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (NoteStore::*)()>(_a, &NoteStore::currentChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (NoteStore::*)()>(_a, &NoteStore::titleChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (NoteStore::*)()>(_a, &NoteStore::metaChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (NoteStore::*)()>(_a, &NoteStore::breadcrumbsChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (NoteStore::*)()>(_a, &NoteStore::pagesChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (NoteStore::*)()>(_a, &NoteStore::trashChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (NoteStore::*)()>(_a, &NoteStore::wordCountChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (NoteStore::*)()>(_a, &NoteStore::statusChanged, 7))
            return;
    }
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< BlockModel* >(); break;
        case 0:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PageTreeModel* >(); break;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<PageTreeModel**>(_v) = _t->tree(); break;
        case 1: *reinterpret_cast<BlockModel**>(_v) = _t->blocks(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->currentId(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->title(); break;
        case 4: *reinterpret_cast<QString*>(_v) = _t->icon(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->favorite(); break;
        case 6: *reinterpret_cast<QString*>(_v) = _t->updatedText(); break;
        case 7: *reinterpret_cast<QVariantList*>(_v) = _t->breadcrumbs(); break;
        case 8: *reinterpret_cast<QVariantList*>(_v) = _t->favorites(); break;
        case 9: *reinterpret_cast<QVariantList*>(_v) = _t->recentPages(); break;
        case 10: *reinterpret_cast<QVariantList*>(_v) = _t->trashPages(); break;
        case 11: *reinterpret_cast<int*>(_v) = _t->pageCount(); break;
        case 12: *reinterpret_cast<int*>(_v) = _t->wordCount(); break;
        case 13: *reinterpret_cast<QString*>(_v) = _t->status(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 3: _t->setTitle(*reinterpret_cast<QString*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *NoteStore::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NoteStore::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9NoteStoreE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NoteStore::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 31)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 31;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 31)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 31;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void NoteStore::currentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void NoteStore::titleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void NoteStore::metaChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void NoteStore::breadcrumbsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void NoteStore::pagesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void NoteStore::trashChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void NoteStore::wordCountChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void NoteStore::statusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
