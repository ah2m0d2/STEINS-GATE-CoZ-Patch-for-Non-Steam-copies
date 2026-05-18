#pragma once

#include <QObject>
#include <QScriptable>
#include <QScriptValue>

class Transaction;
class TxStream;
class TxFileStream;
class TxXdelta3Stream;
class TxMpkInputStream;

namespace api {

class ApiHost;

/*^jsdoc
 * Transaction system
 * 
 * After discovering all necessary data and receiving all necessary input,
 * the installer script must fill the (global) {@link ng.tx.Transaction} with all
 * actions to be performed during the installation.
 * 
 * These actions are not performed immediately, but queued to be run after the
 * script calls {@link ng.tx.run} (upon which control flow leaves the script
 * and the user is presented with a native progess page).
 * 
 * @example
 * var tx = ng.tx.tx();
 * 
 * var section1 = tx.addSection("Installing game");
 * section1.copyFiles("%GAME_SRC%/*", "%INSTALL_ROOT%");
 * 
 * var section2 = tx.addSection("Installing patch");
 * section2.copyFiles("%PATCH_SRC%/*", "%INSTALL_ROOT%");
 * section2.binarySearchReplace("%INSTALL_ROOT%/Game.exe", originalCode, replacementCode);
 * 
 * ng.tx.run();
 * 
 * @namespace tx
 * @memberof ng
 * @static
 * @toplevel
 ^jsdoc*/
class TxHost : public QObject, protected QScriptable {
    Q_OBJECT

   public:
    explicit TxHost(ApiHost* parent);
    void setupScriptObject(QScriptValue& o);
    ~TxHost();

    Q_INVOKABLE Transaction* tx();
    Q_INVOKABLE void run();
    Q_INVOKABLE TxFileStream* fileStream(const QString& inPath);
    Q_INVOKABLE TxMpkInputStream* mpkInputStream(const QString& inPath,
                                                 quint32 entry);
    Q_INVOKABLE TxXdelta3Stream* xdelta3Stream(TxStream* srcStream,
                                               TxStream* diffStream);
};
}  // namespace api