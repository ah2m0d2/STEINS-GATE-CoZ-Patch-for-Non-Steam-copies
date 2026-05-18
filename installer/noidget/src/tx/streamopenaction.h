#pragma once

#include "txaction.h"
#include "txstream.h"
#include <QObject>
#include <QScriptable>

/*^jsdoc
 * Action to queue the opening of a stream
 * 
 * Parameters such as input file can be set in the stream builder factories /
 * on the stream builders themselves.
 * 
 * @class StreamOpenAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {ng.tx.TxStream} stream - stream this action is to be executed on
 ^jsdoc*/
class StreamOpenAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(TxStream* stream READ stream WRITE setStream)

   public:
    explicit StreamOpenAction(QObject* parent = 0) : TxAction(parent) {}
    void run() override;

    TxStream* stream() { return _stream; }
    void setStream(TxStream* const& stream) { _stream = stream; }

   private:
    TxStream* _stream;

    qint64 calcSubactionCount() override { return 0; }
};