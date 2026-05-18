#pragma once

#include "txaction.h"
#include "txstream.h"
#include <QObject>
#include <QScriptable>

/*^jsdoc
 * Action to queue the closing of a previously opened stream
 * 
 * @class StreamCloseAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {ng.tx.TxStream} stream - stream this action is to be executed on
 ^jsdoc*/
class StreamCloseAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(TxStream* stream READ stream WRITE setStream)

   public:
    explicit StreamCloseAction(QObject* parent = 0) : TxAction(parent) {}
    void run() override;

    TxStream* stream() { return _stream; }
    void setStream(TxStream* const& stream) { _stream = stream; }

   private:
    TxStream* _stream;

    qint64 calcSubactionCount() override { return 0; }
};