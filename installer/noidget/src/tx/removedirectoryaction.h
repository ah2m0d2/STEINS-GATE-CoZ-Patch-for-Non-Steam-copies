#pragma once

#include "txaction.h"
#include <QObject>
#include <QScriptable>

/*^jsdoc
 * Action to recursively remove a directory
 * 
 * @class RemoveDirectoryAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} path
 * @property {bool} onlyIfEmpty - default false. If true, only deletes if
 * directory contains no files or subdirectories.
 ^jsdoc*/
class RemoveDirectoryAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath)
    Q_PROPERTY(bool onlyIfEmpty READ onlyIfEmpty WRITE setOnlyIfEmpty)

   public:
    explicit RemoveDirectoryAction(QObject* parent = 0) : TxAction(parent) {}
    void run() override;

    QString path() { return _path; }
    void setPath(const QString& path) { _path = path; }
    bool onlyIfEmpty() { return _onlyIfEmpty; }
    void setOnlyIfEmpty(bool onlyIfEmpty) { _onlyIfEmpty = onlyIfEmpty; }

   private:
    QString _path;
    bool _onlyIfEmpty = false;
};