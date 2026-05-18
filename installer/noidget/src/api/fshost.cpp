#include "fshost.h"
#include "apihost.h"
#include "fs.h"
#include "installerapplication.h"
#include <api/exception.h>
#include <QScriptValueIterator>

static QScriptValue addMacros(QScriptContext *context, QScriptEngine *engine);

static void modifyFsInstance(QScriptValue &o) {
    o.setProperty("addMacros", o.engine()->newFunction(addMacros));
}

QScriptValue fsToScriptValue(QScriptEngine *engine, Fs *const &in) {
    auto ret = engine->newQObject(in);
    modifyFsInstance(ret);
    return ret;
}
void fsFromScriptValue(const QScriptValue &object, Fs *&out) {
    out = qobject_cast<Fs *>(object.toQObject());
}

/*^jsdoc
 * Adds/changes `macros`, name => value. Throws for invalid arguments.
 * @method addMacros
 * @param {Object} macros
 * @memberof ng.fs.Fs
 * @instance
 ^jsdoc*/
static QScriptValue addMacros(QScriptContext *context, QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    Fs *fs;
    fsFromScriptValue(_this, fs);
    QScriptValue ret;
    if (context->argumentCount() < 1) {
        SCRIPT_THROW_FUN("Missing required macros parameter")
        return ret;
    }
    QScriptValue macros = context->argument(0);
    if (!macros.isObject()) {
        SCRIPT_THROW_FUN("Macros parameter has invalid type")
        return ret;
    }
    QScriptValueIterator it(macros);
    SCRIPT_EX_GUARD_START_FUN
    while (it.hasNext()) {
        it.next();
        if (it.value().isString()) {
            fs->setMacro(it.name(), it.value().toString());
        }
    }
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

namespace api {
FsHost::FsHost(ApiHost *parent) : QObject(parent) {
    QScriptEngine *engine = parent->engine();
    qScriptRegisterMetaType(engine, fsToScriptValue, fsFromScriptValue);
}
void FsHost::setupScriptObject(QScriptValue &o) {}
FsHost::~FsHost() {}

/*^jsdoc
 * Creates a new {@link ng.fs.Fs}
 * @method createFs
 * @memberof ng.fs
 * @static
 * @returns {ng.fs.Fs}
 ^jsdoc*/
QScriptValue FsHost::createFs() {
    ENFORCE_SCRIPT
    Fs *fs = new Fs();
    auto ret =
        context()->engine()->newQObject(fs, QScriptEngine::ScriptOwnership);
    modifyFsInstance(ret);
    return ret;
}
/*^jsdoc
 * Returns a global {@link ng.fs.Fs} instance.
 *
 * Care must be taken when setting macros on this in a page, as they are not
 reset if the user goes back.
 * @method global
 * @memberof ng.fs
 * @static
 * @returns {ng.fs.Fs}
 ^jsdoc*/
Fs *FsHost::global() { return ngApp->globalFs(); }

}  // namespace api