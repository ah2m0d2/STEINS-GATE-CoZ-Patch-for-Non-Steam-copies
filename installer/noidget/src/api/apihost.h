#pragma once

#include <QObject>
#include <QScriptEngine>
#include <util/exception.h>
#include <QMetaEnum>
#include <api/view/dialog.h>
namespace api {

/*^jsdoc
 * ## How to read this
 *
 * - Symbols without parentheses indicate static ("namespace") objects.
 * - Class parameters (in parentheses) are instance properties set at
 construction time which cannot be changed (and maybe not even read) later on.
 * - Simple instance members are often listed in a "properties" table for
 brevity - these are equivalent to "instance members".
 * - Unless otherwise noted (with a visible constructor), classes are
 instantiated through factory methods. Refer to the class properties/instance
 data members for descriptions of the factory methods' parameters.
 * - Some classes/functions have event handler/callback instance
 members/parameters. The signatures for these are given in their descriptions.
 *
 * @namespace ng
 * @toplevel
 ^jsdoc*/
class ApiHost : public QObject {
    Q_OBJECT

   public:
    explicit ApiHost(QObject *parent = 0);
    ~ApiHost();
    QScriptEngine *engine() { return _engine; }
    QScriptValue root();

    template <typename T>
    static void registerEnum(QScriptValue &parent);

   private:
    QScriptEngine *_engine;

    template <typename T>
    static QScriptValue enumToScriptValue(QScriptEngine *engine, T const &in);
    template <typename T>
    static void enumFromScriptValue(const QScriptValue &object, T &out);
};

template <typename T>
QScriptValue ApiHost::enumToScriptValue(QScriptEngine *, T const &in) {
    return QScriptValue((int)in);
}
template <typename T>
void ApiHost::enumFromScriptValue(const QScriptValue &object, T &out) {
    out = (T)object.toInt32();
}

template <typename T>
void ApiHost::registerEnum(QScriptValue &parent) {
    if (!parent.isQObject() && !parent.isQMetaObject() && !parent.isObject()) {
        throw NgException(
            "Attempted to register enum on script value that cannot have named "
            "properties");
    }
    auto metaEnum = QMetaEnum::fromType<T>();
    QScriptValue scriptEnum = parent.engine()->newObject();
    for (auto i = 0; i < metaEnum.keyCount(); i++) {
        scriptEnum.setProperty(
            metaEnum.key(i), metaEnum.value(i),
            QScriptValue::ReadOnly | QScriptValue::Undeletable);
    }
    parent.setProperty(metaEnum.name(), scriptEnum,
                       QScriptValue::ReadOnly | QScriptValue::Undeletable);

    qScriptRegisterMetaType(parent.engine(), enumToScriptValue<T>,
                            enumFromScriptValue<T>);
}

}  // namespace api