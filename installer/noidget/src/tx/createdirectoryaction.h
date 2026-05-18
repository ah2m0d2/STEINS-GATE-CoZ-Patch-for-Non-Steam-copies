#pragma once

#include "txaction.h"
#include <QObject>
#include <QScriptable>

/*^jsdoc
 * Action to create a directory
 * 
 * Parent directories will be created when this is executed.
 * 
 * @class CreateDirectoryAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} path
 ^jsdoc*/
class CreateDirectoryAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath)

   public:
    explicit CreateDirectoryAction(QObject* parent = 0) : TxAction(parent) {}
    void run() override;

    QString path() { return _path; }
    void setPath(const QString& path) { _path = path; }

   private:
    QString _path;
};