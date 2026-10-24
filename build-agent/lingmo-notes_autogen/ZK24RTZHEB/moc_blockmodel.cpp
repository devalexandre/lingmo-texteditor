/****************************************************************************
** Meta object code from reading C++ file 'blockmodel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../notes/blockmodel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'blockmodel.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10BlockModelE_t {};
} // unnamed namespace

template <> constexpr inline auto BlockModel::qt_create_metaobjectdata<qt_meta_tag_ZN10BlockModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "BlockModel",
        "countChanged",
        "",
        "edited",
        "typeAt",
        "row",
        "textAt",
        "indentAt",
        "setText",
        "QVariantMap",
        "text",
        "cursor",
        "setType",
        "type",
        "applyType",
        "split",
        "pos",
        "backspaceAtStart",
        "deleteBlock",
        "insertBlock",
        "indent",
        "changeIndent",
        "delta",
        "moveBlock",
        "duplicateBlock",
        "setChecked",
        "checked",
        "setCollapsed",
        "collapsed",
        "setEmoji",
        "emoji",
        "setLanguage",
        "language",
        "previousEditable",
        "nextEditable",
        "count"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'countChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'edited'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'typeAt'
        QtMocHelpers::MethodData<QString(int) const>(4, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::Int, 5 },
        }}),
        // Method 'textAt'
        QtMocHelpers::MethodData<QString(int) const>(6, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::Int, 5 },
        }}),
        // Method 'indentAt'
        QtMocHelpers::MethodData<int(int) const>(7, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::Int, 5 },
        }}),
        // Method 'setText'
        QtMocHelpers::MethodData<QVariantMap(int, const QString &, int)>(8, 2, QMC::AccessPublic, 0x80000000 | 9, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 10 }, { QMetaType::Int, 11 },
        }}),
        // Method 'setText'
        QtMocHelpers::MethodData<QVariantMap(int, const QString &)>(8, 2, QMC::AccessPublic | QMC::MethodCloned, 0x80000000 | 9, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 10 },
        }}),
        // Method 'setType'
        QtMocHelpers::MethodData<void(int, const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 13 },
        }}),
        // Method 'applyType'
        QtMocHelpers::MethodData<QVariantMap(int, const QString &)>(14, 2, QMC::AccessPublic, 0x80000000 | 9, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 13 },
        }}),
        // Method 'split'
        QtMocHelpers::MethodData<QVariantMap(int, int)>(15, 2, QMC::AccessPublic, 0x80000000 | 9, {{
            { QMetaType::Int, 5 }, { QMetaType::Int, 16 },
        }}),
        // Method 'backspaceAtStart'
        QtMocHelpers::MethodData<QVariantMap(int)>(17, 2, QMC::AccessPublic, 0x80000000 | 9, {{
            { QMetaType::Int, 5 },
        }}),
        // Method 'deleteBlock'
        QtMocHelpers::MethodData<QVariantMap(int)>(18, 2, QMC::AccessPublic, 0x80000000 | 9, {{
            { QMetaType::Int, 5 },
        }}),
        // Method 'insertBlock'
        QtMocHelpers::MethodData<int(int, const QString &, int, const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 13 }, { QMetaType::Int, 20 }, { QMetaType::QString, 10 },
        }}),
        // Method 'insertBlock'
        QtMocHelpers::MethodData<int(int, const QString &, int)>(19, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Int, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 13 }, { QMetaType::Int, 20 },
        }}),
        // Method 'insertBlock'
        QtMocHelpers::MethodData<int(int, const QString &)>(19, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Int, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 13 },
        }}),
        // Method 'changeIndent'
        QtMocHelpers::MethodData<bool(int, int)>(21, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Int, 5 }, { QMetaType::Int, 22 },
        }}),
        // Method 'moveBlock'
        QtMocHelpers::MethodData<int(int, int)>(23, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::Int, 5 }, { QMetaType::Int, 22 },
        }}),
        // Method 'duplicateBlock'
        QtMocHelpers::MethodData<int(int)>(24, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::Int, 5 },
        }}),
        // Method 'setChecked'
        QtMocHelpers::MethodData<void(int, bool)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::Bool, 26 },
        }}),
        // Method 'setCollapsed'
        QtMocHelpers::MethodData<void(int, bool)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::Bool, 28 },
        }}),
        // Method 'setEmoji'
        QtMocHelpers::MethodData<void(int, const QString &)>(29, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 30 },
        }}),
        // Method 'setLanguage'
        QtMocHelpers::MethodData<void(int, const QString &)>(31, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 32 },
        }}),
        // Method 'previousEditable'
        QtMocHelpers::MethodData<int(int) const>(33, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::Int, 5 },
        }}),
        // Method 'nextEditable'
        QtMocHelpers::MethodData<int(int) const>(34, 2, QMC::AccessPublic, QMetaType::Int, {{
            { QMetaType::Int, 5 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'count'
        QtMocHelpers::PropertyData<int>(35, QMetaType::Int, QMC::DefaultPropertyFlags, 0),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<BlockModel, qt_meta_tag_ZN10BlockModelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject BlockModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10BlockModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10BlockModelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10BlockModelE_t>.metaTypes,
    nullptr
} };

void BlockModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<BlockModel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->countChanged(); break;
        case 1: _t->edited(); break;
        case 2: { QString _r = _t->typeAt((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 3: { QString _r = _t->textAt((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 4: { int _r = _t->indentAt((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 5: { QVariantMap _r = _t->setText((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 6: { QVariantMap _r = _t->setText((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 7: _t->setType((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: { QVariantMap _r = _t->applyType((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 9: { QVariantMap _r = _t->split((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 10: { QVariantMap _r = _t->backspaceAtStart((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 11: { QVariantMap _r = _t->deleteBlock((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 12: { int _r = _t->insertBlock((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 13: { int _r = _t->insertBlock((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 14: { int _r = _t->insertBlock((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 15: { bool _r = _t->changeIndent((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 16: { int _r = _t->moveBlock((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 17: { int _r = _t->duplicateBlock((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 18: _t->setChecked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 19: _t->setCollapsed((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 20: _t->setEmoji((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 21: _t->setLanguage((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 22: { int _r = _t->previousEditable((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        case 23: { int _r = _t->nextEditable((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<int*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (BlockModel::*)()>(_a, &BlockModel::countChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (BlockModel::*)()>(_a, &BlockModel::edited, 1))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_v) = _t->count(); break;
        default: break;
        }
    }
}

const QMetaObject *BlockModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BlockModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10BlockModelE_t>.strings))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int BlockModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 24;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void BlockModel::countChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void BlockModel::edited()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
