#pragma once

#include "txaction.h"
#include <QObject>
#include <QScriptable>

/*^jsdoc
 * Action to remove a single file
 * 
 * @class RemoveFileAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} path
 ^jsdoc*/
class RemoveFileAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath)

   public:
    explicit RemoveFileAction(QObject* parent = 0) : TxAction(parent) {}
    void run() override;

    QString path() { return _path; }
    void setPath(const QString& path) { _path = path; }

   private:
    QString _path;
};