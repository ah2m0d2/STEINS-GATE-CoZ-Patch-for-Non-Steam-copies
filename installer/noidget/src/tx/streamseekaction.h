#pragma once

#include "txaction.h"
#include "txstream.h"
#include <QObject>
#include <QScriptable>

/*^jsdoc
 * Action to queue the seeking of a previously opened stream
 * 
 * Not all streams are seekable.
 * 
 * @class StreamSeekAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {ng.tx.TxStream} stream - stream this action is to be executed on
 * @property {Number} count - bytes to seek relative to position at time of execution
 ^jsdoc*/
class StreamSeekAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(TxStream* stream READ stream WRITE setStream)
    Q_PROPERTY(qint64 count READ count WRITE setCount)

   public:
    explicit StreamSeekAction(QObject* parent = 0) : TxAction(parent) {}
    void run() override;

    TxStream* stream() { return _stream; }
    void setStream(TxStream* const& stream) { _stream = stream; }
    qint64 count() { return _count; }
    void setCount(qint64 count) { _count = count; }

   private:
    TxStream* _stream;
    qint64 _count;

    qint64 calcSubactionCount() override { return 0; }
};