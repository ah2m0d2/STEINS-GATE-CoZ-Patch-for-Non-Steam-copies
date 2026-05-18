#pragma once

#include "txaction.h"
#include <QObject>
#include <QScriptable>

/*^jsdoc
 * Action to write plain text to the log when execution reaches it
 * 
 * Rich text is not supported. Text will be displayed in the UI unless `fileOnly`
 * is specified, and written to the plaintext log file.
 * 
 * @class LogAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} text
 * @property {boolean} fileOnly - default `false`
 ^jsdoc*/
class LogAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool fileOnly READ fileOnly WRITE setFileOnly)

   public:
    explicit LogAction(QObject* parent = 0) : TxAction(parent) {}
    void run() override;

    QString text() { return _text; }
    void setText(const QString& text) { _text = text; }
    bool fileOnly() { return _fileOnly; }
    void setFileOnly(bool fileOnly) { _fileOnly = fileOnly; }

   private:
    QString _text;
    bool _fileOnly = false;

    qint64 calcSubactionCount() override { return 0; }
};