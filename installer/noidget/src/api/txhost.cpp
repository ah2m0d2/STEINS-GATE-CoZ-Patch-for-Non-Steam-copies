#include "txhost.h"
#include "apihost.h"
#include "receiptwriter.h"
#include "tx/transaction.h"
#include "tx/txsection.h"
#include "tx/txaction.h"
#include "tx/copyfilesaction.h"
#include "tx/logaction.h"
#include "tx/createdirectoryaction.h"
#include "tx/streamopenaction.h"
#include "tx/streamcloseaction.h"
#include "tx/streamseekaction.h"
#include "tx/writestreamaction.h"
#include "tx/buildmpkaction.h"
#include "tx/binarysearchreplaceaction.h"
#include "tx/txfilestream.h"
#include "tx/txxdelta3stream.h"
#include "tx/txmpkinputstream.h"
#include "installerapplication.h"
#include "progresspage.h"
#include "installerwindow.h"
#include <api/exception.h>
#include "tx/setregistryvalueaction.h"
#include "tx/createshortcutaction.h"
#include "tx/removedirectoryaction.h"
#include "tx/rollbackreceiptaction.h"
#include "tx/removefileaction.h"

static QScriptValue txSectionCopyFiles(QScriptContext *context,
                                       QScriptEngine *engine);
static QScriptValue txSectionLog(QScriptContext *context,
                                 QScriptEngine *engine);
static QScriptValue txSectionCreateDirectory(QScriptContext *context,
                                             QScriptEngine *engine);
static QScriptValue txSectionStreamOpen(QScriptContext *context,
                                        QScriptEngine *engine);
static QScriptValue txSectionStreamClose(QScriptContext *context,
                                         QScriptEngine *engine);
static QScriptValue txSectionStreamSeek(QScriptContext *context,
                                        QScriptEngine *engine);
static QScriptValue txSectionWriteStream(QScriptContext *context,
                                         QScriptEngine *engine);
static QScriptValue txSectionBuildMpk(QScriptContext *context,
                                      QScriptEngine *engine);
static QScriptValue txSectionBinarySearchReplace(QScriptContext *context,
                                                 QScriptEngine *engine);
#ifdef Q_OS_WIN32
static QScriptValue txSectionSetRegistryValue(QScriptContext *context,
                                              QScriptEngine *engine);
#endif
static QScriptValue txSectionCreateShortcut(QScriptContext *context,
                                            QScriptEngine *engine);
static QScriptValue txSectionRemoveDirectory(QScriptContext *context,
                                             QScriptEngine *engine);
static QScriptValue txSectionRollbackReceipt(QScriptContext *context,
                                             QScriptEngine *engine);
static QScriptValue txSectionRemoveFile(QScriptContext *context,
                                        QScriptEngine *engine);
static void modifyTxSectionInstance(QScriptValue &o) {
    o.setProperty("copyFiles", o.engine()->newFunction(txSectionCopyFiles));
    o.setProperty("log", o.engine()->newFunction(txSectionLog));
    o.setProperty("createDirectory",
                  o.engine()->newFunction(txSectionCreateDirectory));
    o.setProperty("streamOpen", o.engine()->newFunction(txSectionStreamOpen));
    o.setProperty("streamClose", o.engine()->newFunction(txSectionStreamClose));
    o.setProperty("streamSeek", o.engine()->newFunction(txSectionStreamSeek));
    o.setProperty("writeStream", o.engine()->newFunction(txSectionWriteStream));
    o.setProperty("buildMpk", o.engine()->newFunction(txSectionBuildMpk));
    o.setProperty("binarySearchReplace",
                  o.engine()->newFunction(txSectionBinarySearchReplace));
#ifdef Q_OS_WIN32
    o.setProperty("setRegistryValue",
                  o.engine()->newFunction(txSectionSetRegistryValue));
#endif
    o.setProperty("createShortcut",
                  o.engine()->newFunction(txSectionCreateShortcut));
    o.setProperty("removeDirectory",
                  o.engine()->newFunction(txSectionRemoveDirectory));
    o.setProperty("rollbackReceipt",
                  o.engine()->newFunction(txSectionRollbackReceipt));
    o.setProperty("removeFile", o.engine()->newFunction(txSectionRemoveFile));
}

static QScriptValue mpkAddEntry(QScriptContext *context, QScriptEngine *engine);
static void modifyBuildMpkActionInstance(QScriptValue &o) {
    o.setProperty("addEntry", o.engine()->newFunction(mpkAddEntry));
}

QScriptValue transactionToScriptValue(QScriptEngine *engine,
                                      Transaction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void transactionFromScriptValue(const QScriptValue &object, Transaction *&out) {
    out = qobject_cast<Transaction *>(object.toQObject());
}
QScriptValue txSectionToScriptValue(QScriptEngine *engine,
                                    TxSection *const &in) {
    auto ret = engine->newQObject(in);
    modifyTxSectionInstance(ret);
    return ret;
}
void txSectionFromScriptValue(const QScriptValue &object, TxSection *&out) {
    out = qobject_cast<TxSection *>(object.toQObject());
}
QScriptValue txActionToScriptValue(QScriptEngine *engine, TxAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void txActionFromScriptValue(const QScriptValue &object, TxAction *&out) {
    out = qobject_cast<TxAction *>(object.toQObject());
}
QScriptValue copyFilesActionToScriptValue(QScriptEngine *engine,
                                          CopyFilesAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void copyFilesActionFromScriptValue(const QScriptValue &object,
                                    CopyFilesAction *&out) {
    out = qobject_cast<CopyFilesAction *>(object.toQObject());
}
QScriptValue logActionToScriptValue(QScriptEngine *engine,
                                    LogAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void logActionFromScriptValue(const QScriptValue &object, LogAction *&out) {
    out = qobject_cast<LogAction *>(object.toQObject());
}
QScriptValue createDirectoryActionToScriptValue(
    QScriptEngine *engine, CreateDirectoryAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void createDirectoryActionFromScriptValue(const QScriptValue &object,
                                          CreateDirectoryAction *&out) {
    out = qobject_cast<CreateDirectoryAction *>(object.toQObject());
}
QScriptValue streamOpenActionToScriptValue(QScriptEngine *engine,
                                           StreamOpenAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void streamOpenActionFromScriptValue(const QScriptValue &object,
                                     StreamOpenAction *&out) {
    out = qobject_cast<StreamOpenAction *>(object.toQObject());
}
QScriptValue streamCloseActionToScriptValue(QScriptEngine *engine,
                                            StreamCloseAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void streamCloseActionFromScriptValue(const QScriptValue &object,
                                      StreamCloseAction *&out) {
    out = qobject_cast<StreamCloseAction *>(object.toQObject());
}
QScriptValue streamSeekActionToScriptValue(QScriptEngine *engine,
                                           StreamSeekAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void streamSeekActionFromScriptValue(const QScriptValue &object,
                                     StreamSeekAction *&out) {
    out = qobject_cast<StreamSeekAction *>(object.toQObject());
}
QScriptValue writeStreamActionToScriptValue(QScriptEngine *engine,
                                            WriteStreamAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void writeStreamActionFromScriptValue(const QScriptValue &object,
                                      WriteStreamAction *&out) {
    out = qobject_cast<WriteStreamAction *>(object.toQObject());
}
QScriptValue buildMpkActionToScriptValue(QScriptEngine *engine,
                                         BuildMpkAction *const &in) {
    auto ret = engine->newQObject(in);
    modifyBuildMpkActionInstance(ret);
    return ret;
}
void buildMpkActionFromScriptValue(const QScriptValue &object,
                                   BuildMpkAction *&out) {
    out = qobject_cast<BuildMpkAction *>(object.toQObject());
}
QScriptValue binarySearchReplaceActionToScriptValue(
    QScriptEngine *engine, BinarySearchReplaceAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void binarySearchReplaceActionFromScriptValue(const QScriptValue &object,
                                              BinarySearchReplaceAction *&out) {
    out = qobject_cast<BinarySearchReplaceAction *>(object.toQObject());
}
#ifdef Q_OS_WIN32
QScriptValue setRegistryValueActionToScriptValue(
    QScriptEngine *engine, SetRegistryValueAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void setRegistryValueActionFromScriptValue(const QScriptValue &object,
                                           SetRegistryValueAction *&out) {
    out = qobject_cast<SetRegistryValueAction *>(object.toQObject());
}
#endif
QScriptValue createShortcutActionToScriptValue(
    QScriptEngine *engine, CreateShortcutAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void createShortcutActionFromScriptValue(const QScriptValue &object,
                                         CreateShortcutAction *&out) {
    out = qobject_cast<CreateShortcutAction *>(object.toQObject());
}
QScriptValue removeDirectoryActionToScriptValue(
    QScriptEngine *engine, RemoveDirectoryAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void removeDirectoryActionFromScriptValue(const QScriptValue &object,
                                          RemoveDirectoryAction *&out) {
    out = qobject_cast<RemoveDirectoryAction *>(object.toQObject());
}
QScriptValue rollbackReceiptActionToScriptValue(
    QScriptEngine *engine, RollbackReceiptAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void rollbackReceiptActionFromScriptValue(const QScriptValue &object,
                                          RollbackReceiptAction *&out) {
    out = qobject_cast<RollbackReceiptAction *>(object.toQObject());
}
QScriptValue removeFileActionToScriptValue(QScriptEngine *engine,
                                           RemoveFileAction *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void removeFileActionFromScriptValue(const QScriptValue &object,
                                     RemoveFileAction *&out) {
    out = qobject_cast<RemoveFileAction *>(object.toQObject());
}
QScriptValue txStreamToScriptValue(QScriptEngine *engine, TxStream *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void txStreamFromScriptValue(const QScriptValue &object, TxStream *&out) {
    out = qobject_cast<TxStream *>(object.toQObject());
}
QScriptValue txFileStreamToScriptValue(QScriptEngine *engine,
                                       TxFileStream *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void txFileStreamFromScriptValue(const QScriptValue &object,
                                 TxFileStream *&out) {
    out = qobject_cast<TxFileStream *>(object.toQObject());
}
QScriptValue txXdelta3StreamToScriptValue(QScriptEngine *engine,
                                          TxXdelta3Stream *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void txXdelta3StreamFromScriptValue(const QScriptValue &object,
                                    TxXdelta3Stream *&out) {
    out = qobject_cast<TxXdelta3Stream *>(object.toQObject());
}
QScriptValue txMpkInputStreamToScriptValue(QScriptEngine *engine,
                                           TxMpkInputStream *const &in) {
    auto ret = engine->newQObject(in);
    return ret;
}
void txMpkInputStreamFromScriptValue(const QScriptValue &object,
                                     TxMpkInputStream *&out) {
    out = qobject_cast<TxMpkInputStream *>(object.toQObject());
}

/*^jsdoc
 * Add a file to the archive
 * 
 * @method addEntry
 * @param {Object} params
 * @param {Number} params.id
 * @param {string} params.name - archive-internal file name/path (ISO-8859-1, max. 223 characters)
 * @param {Number} [params.displaySize=0] - file size in bytes (purely for progress indicator, BuildMpkAction always copies all data)
 * @param params.source - either a file path or a {@link ng.tx.TxStream}
 * @memberof ng.tx.BuildMpkAction
 * @instance
 ^jsdoc*/
static QScriptValue mpkAddEntry(QScriptContext *context,
                                QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    BuildMpkAction *action;
    buildMpkActionFromScriptValue(_this, action);
    QScriptValue ret;
    if (context->argumentCount() < 1) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue obj = context->argument(0);
    if (!obj.isObject()) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }
    if (!obj.property("id").isNumber() || !obj.property("name").isString() ||
        (!obj.property("source").isString() &&
         (!obj.property("source").isQObject() ||
          qobject_cast<TxStream *>(obj.property("source").toQObject()) == 0))) {
        SCRIPT_THROW_FUN("Missing required parameter / invalid type")
        return ret;
    }
    int id = obj.property("id").toInt32();
    QString name = obj.property("name").toString();
    qint64 displaySize = 0;
    if (obj.property("displaySize").isNumber()) {
        displaySize = (qint64)obj.property("displaySize").toInteger();
    }
    SCRIPT_EX_GUARD_START_FUN
    if (obj.property("source").isString()) {
        QString path = obj.property("source").toString();
        action->addEntry(id, name, path, displaySize);
    } else {
        TxStream *stream =
            qobject_cast<TxStream *>(obj.property("source").toQObject());
        action->addEntry(id, name, stream, displaySize);
    }
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

/*^jsdoc
 * File copy operation
 * 
 * @method copyFiles
 * @param {string} src
 * @param {string} dest
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.CopyFilesAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionCopyFiles(QScriptContext *context,
                                       QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 2) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue src = context->argument(0);
    QScriptValue dest = context->argument(1);
    if (!src.isString() || !dest.isString()) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }
    SCRIPT_EX_GUARD_START_FUN
    CopyFilesAction *action = new CopyFilesAction(section);
    action->setSrc(src.toString());
    action->setDest(dest.toString());
    section->addAction(action);
    ret = copyFilesActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

/*^jsdoc
 * Write plain text to the log during section
 * 
 * @method log
 * @param {string} text
 * @param {boolean} [fileOnly=false]
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.LogAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionLog(QScriptContext *context,
                                 QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 1) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue text = context->argument(0);
    if (!text.isString()) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }
    bool fileOnly = false;
    if (context->argumentCount() >= 2) {
        QScriptValue _fileOnly = context->argument(1);
        if (_fileOnly.isBool()) fileOnly = _fileOnly.toBool();
    }
    SCRIPT_EX_GUARD_START_FUN
    LogAction *action = new LogAction(section);
    action->setText(text.toString());
    action->setFileOnly(fileOnly);
    section->addAction(action);
    ret = logActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

/*^jsdoc
 * Create directory operation
 * 
 * Parent directories will be created when this is executed.
 * 
 * @method createDirectory
 * @param {string} path
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.CreateDirectoryAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionCreateDirectory(QScriptContext *context,
                                             QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 1) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue path = context->argument(0);
    if (!path.isString()) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }
    SCRIPT_EX_GUARD_START_FUN
    CreateDirectoryAction *action = new CreateDirectoryAction(section);
    action->setPath(path.toString());
    section->addAction(action);
    ret = createDirectoryActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

/*^jsdoc
 * Queue the opening of a stream
 * 
 * @method streamOpen
 * @param {ng.tx.TxStream} stream
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.StreamOpenAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionStreamOpen(QScriptContext *context,
                                        QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 1) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue _stream = context->argument(0);
    TxStream *stream;
    if (!_stream.isQObject() ||
        (stream = qobject_cast<TxStream *>(_stream.toQObject())) == 0) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }
    SCRIPT_EX_GUARD_START_FUN
    StreamOpenAction *action = new StreamOpenAction(section);
    action->setStream(stream);
    section->addAction(action);
    ret = streamOpenActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

/*^jsdoc
 * Queue the closing of a previously opened stream
 * 
 * @method streamClose
 * @param {ng.tx.TxStream} stream
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.StreamCloseAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionStreamClose(QScriptContext *context,
                                         QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 1) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue _stream = context->argument(0);
    TxStream *stream;
    if (!_stream.isQObject() ||
        (stream = qobject_cast<TxStream *>(_stream.toQObject())) == 0) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }
    SCRIPT_EX_GUARD_START_FUN
    StreamCloseAction *action = new StreamCloseAction(section);
    action->setStream(stream);
    section->addAction(action);
    ret = streamCloseActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

/*^jsdoc
 * Queue the seeking of a previously opened stream, relative to its last read 
 * position
 * 
 * @method streamSeek
 * @param {ng.tx.TxStream} stream
 * @param {Number} count
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.StreamSeekAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionStreamSeek(QScriptContext *context,
                                        QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 2) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue _stream = context->argument(0);
    TxStream *stream;
    QScriptValue count = context->argument(1);
    if (!_stream.isQObject() ||
        (stream = qobject_cast<TxStream *>(_stream.toQObject())) == 0 ||
        !count.isNumber()) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }
    SCRIPT_EX_GUARD_START_FUN
    StreamSeekAction *action = new StreamSeekAction(section);
    action->setStream(stream);
    action->setCount((qint64)count.toInteger());
    section->addAction(action);
    ret = streamSeekActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

/*^jsdoc
 * Write from a stream to a file
 * 
 * @method writeStream
 * @param {ng.tx.TxStream} stream
 * @param {string} dest
 * @param {Number} [count=0]
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.WriteStreamAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionWriteStream(QScriptContext *context,
                                         QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 2) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue _stream = context->argument(0);
    TxStream *stream;
    QScriptValue dest = context->argument(1);
    if (!_stream.isQObject() ||
        (stream = qobject_cast<TxStream *>(_stream.toQObject())) == 0 ||
        !dest.isString()) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }
    SCRIPT_EX_GUARD_START_FUN
    WriteStreamAction *action = new WriteStreamAction(section);
    action->setStream(stream);
    action->setDest(dest.toString());
    if (context->argumentCount() >= 3) {
        QScriptValue count = context->argument(2);
        if (count.isNumber()) {
            action->setCount((qint64)count.toInteger());
        }
    }
    section->addAction(action);
    ret = writeStreamActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

/*^jsdoc
 * Build an MPK archive
 * 
 * @method buildMpk
 * @param {string} path
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.BuildMpkAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionBuildMpk(QScriptContext *context,
                                      QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 1) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue path = context->argument(0);
    if (!path.isString()) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }
    SCRIPT_EX_GUARD_START_FUN
    BuildMpkAction *action = new BuildMpkAction(section);
    action->setPath(path.toString());
    section->addAction(action);
    ret = buildMpkActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

/*^jsdoc
 * Search and replace a pattern in a binary file
 * 
 * @method binarySearchReplace
 * @param {string} path
 * @param {string} needle
 * @param {string} replace
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.BinarySearchReplaceAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionBinarySearchReplace(QScriptContext *context,
                                                 QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 3) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue path = context->argument(0);
    QScriptValue needle = context->argument(1);
    QScriptValue replace = context->argument(2);
    if (!path.isString() || !needle.isString() || !replace.isString()) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }
    SCRIPT_EX_GUARD_START_FUN
    BinarySearchReplaceAction *action = new BinarySearchReplaceAction(section);
    action->setPath(path.toString());
    action->setNeedle(needle.toString());
    action->setReplace(replace.toString());
    section->addAction(action);
    ret = binarySearchReplaceActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

#ifdef Q_OS_WIN32
/*^jsdoc
 * Set a registry value (Win32 only)
 * 
 * @method setRegistryValue
 * @param {ng.win32.RootKey} root
 * @param {string} key
 * @param {boolean} use64bit - request 64-bit (`true`) or 32-bit (`false`) registry view on 64-bit Windows
 * @param {string} valName
 * @param value - string or number (converted to unsigned 32-bit integer REG_DWORD)
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.SetRegistryValueAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionSetRegistryValue(QScriptContext *context,
                                              QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 5) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue root = context->argument(0);
    QScriptValue key = context->argument(1);
    QScriptValue use64bit = context->argument(2);
    QScriptValue valName = context->argument(3);
    QScriptValue value = context->argument(4);
    if (!root.isNumber() || !key.isString() || !use64bit.isBool() ||
        !valName.isString() || !value.toVariant().isValid()) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }
    SCRIPT_EX_GUARD_START_FUN
    SetRegistryValueAction *action = new SetRegistryValueAction(section);
    action->setRoot((Registry::RootKey)root.toUInt32());
    action->setKey(key.toString());
    action->setUse64bit(use64bit.toBool());
    action->setValName(valName.toString());
    action->setValue(value.toVariant());
    section->addAction(action);
    ret = setRegistryValueActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}
#endif

/*^jsdoc
 * Create a (desktop/start menu) shortcut (Win32, Linux only)
 * 
 * @method createShortcut
 * @param {Object} params
 * @param {string} params.shortcutPath **(Required)** File path of the shortcut itself
 * @param {string} params.version Linux only. Display version
 * @param {string} params.displayName Linux only. Display name - on Windows, that's just the shortcut's filename itself minus .lnk.
 * @param {string} params.targetPath **(Required)** File path to the shortcut's target
 * @param {string} params.targetArgs Command-line arguments (if any). Note these are *not* parsed for FS macros.
 * @param {string} params.workingDir Working directory to execute the target in (or default)
 * @param {string} params.iconPath (Persistent) path to the icon to use (or default). On Windows, always uses the first icon present in that file.
 * @param {string} params.tooltip Display description
 * @param {string} params.xdgCategories Linux only. Semicolon-separated list of [XDG menu entry categories]{@link https://standards.freedesktop.org/menu-spec/latest/apa.html}
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.CreateShortcutAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionCreateShortcut(QScriptContext *context,
                                            QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 1) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }

    QScriptValue obj = context->argument(0);
    if (!obj.isObject()) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }
    if (!obj.property("shortcutPath").isString() ||
        !obj.property("targetPath").isString()) {
        SCRIPT_THROW_FUN("Missing required parameter / invalid type")
        return ret;
    }

    SCRIPT_EX_GUARD_START_FUN
    CreateShortcutAction *action = new CreateShortcutAction(section);
    action->setShortcutPath(obj.property("shortcutPath").toString());
    action->setTargetPath(obj.property("targetPath").toString());
    if (obj.property("version").isString()) {
        action->setVersion(obj.property("version").toString());
    }
    if (obj.property("displayName").isString()) {
        action->setDisplayName(obj.property("displayName").toString());
    }
    if (obj.property("targetArgs").isString()) {
        action->setTargetArgs(obj.property("targetArgs").toString());
    }
    if (obj.property("workingDir").isString()) {
        action->setWorkingDir(obj.property("workingDir").toString());
    }
    if (obj.property("iconPath").isString()) {
        action->setIconPath(obj.property("iconPath").toString());
    }
    if (obj.property("tooltip").isString()) {
        action->setTooltip(obj.property("tooltip").toString());
    }
    if (obj.property("xdgCategories").isString()) {
        action->setXdgCategories(obj.property("xdgCategories").toString());
    }
    section->addAction(action);
    ret = createShortcutActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

/*^jsdoc
 * Remove directory operation (recursive)
 * 
 * @method removeDirectory
 * @param {string} path
 * @param {bool} [onlyIfEmpty=false]
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.RemoveDirectoryAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionRemoveDirectory(QScriptContext *context,
                                             QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 1) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue path = context->argument(0);
    if (!path.isString()) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }

    SCRIPT_EX_GUARD_START_FUN
    RemoveDirectoryAction *action = new RemoveDirectoryAction(section);
    action->setPath(path.toString());
    if (context->argumentCount() >= 2) {
        QScriptValue onlyIfEmpty = context->argument(1);
        if (onlyIfEmpty.isBool()) {
            action->setOnlyIfEmpty(onlyIfEmpty.toBool());
        }
    }
    section->addAction(action);
    ret = removeDirectoryActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

/*^jsdoc
 * Main uninstall operation
 * 
 * May only be used in uninstall mode
 * 
 * @method rollbackReceipt
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.RollbackReceiptAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionRollbackReceipt(QScriptContext *context,
                                             QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    SCRIPT_EX_GUARD_START_FUN
    RollbackReceiptAction *action = new RollbackReceiptAction(section);
    section->addAction(action);
    ret = rollbackReceiptActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

/*^jsdoc
 * Remove single file operation
 * 
 * @method removeFile
 * @param {string} path
 * @memberof ng.tx.TxSection
 * @returns {ng.tx.RemoveFileAction}
 * @instance
 ^jsdoc*/
static QScriptValue txSectionRemoveFile(QScriptContext *context,
                                        QScriptEngine *engine) {
    QScriptValue _this = context->thisObject();
    TxSection *section;
    txSectionFromScriptValue(_this, section);
    QScriptValue ret;
    if (context->argumentCount() < 1) {
        SCRIPT_THROW_FUN("Missing required parameter")
        return ret;
    }
    QScriptValue path = context->argument(0);
    if (!path.isString()) {
        SCRIPT_THROW_FUN("Parameter has invalid type")
        return ret;
    }

    SCRIPT_EX_GUARD_START_FUN
    RemoveFileAction *action = new RemoveFileAction(section);
    action->setPath(path.toString());
    section->addAction(action);
    ret = removeFileActionToScriptValue(engine, action);
    SCRIPT_EX_GUARD_END_FUN(ret)
    return ret;
}

namespace api {
TxHost::TxHost(ApiHost *parent) : QObject(parent) {
    QScriptEngine *engine = parent->engine();
    qScriptRegisterMetaType(engine, transactionToScriptValue,
                            transactionFromScriptValue);
    qScriptRegisterMetaType(engine, txSectionToScriptValue,
                            txSectionFromScriptValue);
    qScriptRegisterMetaType(engine, txActionToScriptValue,
                            txActionFromScriptValue);
    qScriptRegisterMetaType(engine, copyFilesActionToScriptValue,
                            copyFilesActionFromScriptValue);
    qScriptRegisterMetaType(engine, logActionToScriptValue,
                            logActionFromScriptValue);
    qScriptRegisterMetaType(engine, createDirectoryActionToScriptValue,
                            createDirectoryActionFromScriptValue);
    qScriptRegisterMetaType(engine, streamOpenActionToScriptValue,
                            streamOpenActionFromScriptValue);
    qScriptRegisterMetaType(engine, streamCloseActionToScriptValue,
                            streamCloseActionFromScriptValue);
    qScriptRegisterMetaType(engine, streamSeekActionToScriptValue,
                            streamSeekActionFromScriptValue);
    qScriptRegisterMetaType(engine, writeStreamActionToScriptValue,
                            writeStreamActionFromScriptValue);
    qScriptRegisterMetaType(engine, buildMpkActionToScriptValue,
                            buildMpkActionFromScriptValue);
    qScriptRegisterMetaType(engine, binarySearchReplaceActionToScriptValue,
                            binarySearchReplaceActionFromScriptValue);
#ifdef Q_OS_WIN32
    qScriptRegisterMetaType(engine, setRegistryValueActionToScriptValue,
                            setRegistryValueActionFromScriptValue);
#endif
    qScriptRegisterMetaType(engine, createShortcutActionToScriptValue,
                            createShortcutActionFromScriptValue);
    qScriptRegisterMetaType(engine, removeDirectoryActionToScriptValue,
                            removeDirectoryActionFromScriptValue);
    qScriptRegisterMetaType(engine, rollbackReceiptActionToScriptValue,
                            rollbackReceiptActionFromScriptValue);
    qScriptRegisterMetaType(engine, removeFileActionToScriptValue,
                            removeFileActionFromScriptValue);
    qScriptRegisterMetaType(engine, txStreamToScriptValue,
                            txStreamFromScriptValue);
    qScriptRegisterMetaType(engine, txFileStreamToScriptValue,
                            txFileStreamFromScriptValue);
    qScriptRegisterMetaType(engine, txXdelta3StreamToScriptValue,
                            txXdelta3StreamFromScriptValue);
    qScriptRegisterMetaType(engine, txMpkInputStreamToScriptValue,
                            txMpkInputStreamFromScriptValue);
}
void TxHost::setupScriptObject(QScriptValue &o) {}
TxHost::~TxHost() {}

/*^jsdoc
 * Prepare a stream for reading from file
 * 
 * @method fileStream
 * @param {string} inPath
 * @memberof ng.tx
 * @returns {ng.tx.TxFileStream}
 * @static
 ^jsdoc*/
TxFileStream *TxHost::fileStream(const QString &inPath) {
    SCRIPT_EX_GUARD_START
    TxFileStream *ret = new TxFileStream(tx());
    ret->setInPath(inPath);
    return ret;
    SCRIPT_EX_GUARD_END(nullptr)
}

/*^jsdoc
 * Prepare a stream for decoding a VCDIFF file
 * 
 * @method xdelta3Stream
 * @param {ng.tx.TxStream} srcStream
 * @param {ng.tx.TxStream} diffStream
 * @memberof ng.tx
 * @returns {ng.tx.TxXdelta3Stream}
 * @static
 ^jsdoc*/
TxXdelta3Stream *TxHost::xdelta3Stream(TxStream *srcStream,
                                       TxStream *diffStream) {
    SCRIPT_EX_GUARD_START
    TxXdelta3Stream *ret = new TxXdelta3Stream(tx());
    ret->setSrcStream(srcStream);
    ret->setDiffStream(diffStream);
    return ret;
    SCRIPT_EX_GUARD_END(nullptr)
}

/*^jsdoc
 * Prepare a stream for reading an MPK entry
 * 
 * @method mpkInputStream
 * @param {string} inPath
 * @param {number} entry
 * @memberof ng.tx
 * @returns {ng.tx.TxMpkInputStream}
 * @static
 ^jsdoc*/
TxMpkInputStream *TxHost::mpkInputStream(const QString &inPath, quint32 entry) {
    SCRIPT_EX_GUARD_START
    TxMpkInputStream *ret = new TxMpkInputStream(tx());
    ret->setInPath(inPath);
    ret->setEntry(entry);
    return ret;
    SCRIPT_EX_GUARD_END(nullptr)
}

/*^jsdoc
 * Returns the global {@link ng.tx.Transaction} instance.
 * @method tx
 * @memberof ng.tx
 * @static
 * @returns {ng.tx.Transaction}
 ^jsdoc*/
Transaction *TxHost::tx() { return ngApp->tx(); }

/*^jsdoc
 * Moves to the progress page and starts the installation. **This must be
 * the last thing you call** in the whole script.
 * @method run
 * @memberof ng.tx
 * @static
 ^jsdoc*/
void TxHost::run() {
    ProgressPage *page = new ProgressPage(0);
    ngApp->window()->push(page);
}

}  // namespace api