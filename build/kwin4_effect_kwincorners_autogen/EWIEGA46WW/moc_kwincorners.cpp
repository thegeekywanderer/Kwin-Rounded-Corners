/****************************************************************************
** Meta object code from reading C++ file 'kwincorners.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../kwincorners.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kwincorners.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KwinCornersEffect_t {
    QByteArrayData data[5];
    char stringdata0[58];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KwinCornersEffect_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KwinCornersEffect_t qt_meta_stringdata_KwinCornersEffect = {
    {
QT_MOC_LITERAL(0, 0, 17), // "KwinCornersEffect"
QT_MOC_LITERAL(1, 18, 11), // "windowAdded"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 19), // "KWin::EffectWindow*"
QT_MOC_LITERAL(4, 51, 6) // "window"

    },
    "KwinCornersEffect\0windowAdded\0\0"
    "KWin::EffectWindow*\0window"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KwinCornersEffect[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void KwinCornersEffect::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KwinCornersEffect *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->windowAdded((*reinterpret_cast< KWin::EffectWindow*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< KWin::EffectWindow* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject KwinCornersEffect::staticMetaObject = { {
    QMetaObject::SuperData::link<KWin::Effect::staticMetaObject>(),
    qt_meta_stringdata_KwinCornersEffect.data,
    qt_meta_data_KwinCornersEffect,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *KwinCornersEffect::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KwinCornersEffect::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KwinCornersEffect.stringdata0))
        return static_cast<void*>(this);
    return KWin::Effect::qt_metacast(_clname);
}

int KwinCornersEffect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KWin::Effect::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
