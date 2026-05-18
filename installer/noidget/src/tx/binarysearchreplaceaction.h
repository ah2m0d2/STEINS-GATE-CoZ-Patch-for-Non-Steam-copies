#pragma once

#include "txaction.h"
#include <QObject>
#include <QScriptable>

/*^jsdoc
 * Action to search and replace a pattern in a binary file
 * 
 * All occurrences of `needle` will be replaced. The source file is loaded into
 * memory entirely first. `needle` and `pattern` may have different lengths.
 * 
 * Wildcards are not supported.
 * 
 * @class BinarySearchReplaceAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} path
 * @property {string} needle - hex string of pattern to search for
 * @property {string} replace - hex string of pattern to replace it with
 ^jsdoc*/
class BinarySearchReplaceAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath)
    Q_PROPERTY(QString needle READ needle WRITE setNeedle)
    Q_PROPERTY(QString replace READ replace WRITE setReplace)

   public:
    explicit BinarySearchReplaceAction(QObject* parent = 0)
        : TxAction(parent) {}
    void run() override;

    QString path() { return _path; }
    void setPath(const QString& path) { _path = path; }
    QString needle() { return _needle; }
    void setNeedle(const QString& needle) { _needle = needle; }
    QString replace() { return _replace; }
    void setReplace(const QString& replace) { _replace = replace; }

   private:
    QString _path, _needle, _replace;
};