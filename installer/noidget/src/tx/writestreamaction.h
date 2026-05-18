#pragma once

#include "txaction.h"
#include "txstream.h"
#include <QObject>
#include <QScriptable>

/*^jsdoc
 * Queued stream-to-file-writing action
 * 
 * If the file `dest` already exists it will be overwritten. If `count` is not
 * specified (or 0), all remaining data will be written.
 * 
 * Note that you *cannot overwrite the stream's source* with this.
 * 
 * The directory `dest` should reside in must already exist when this is executed.
 * 
 * @class WriteStreamAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {ng.tx.TxStream} stream - stream this action is to be executed on
 * @property {string} dest - destination file path
 ^jsdoc*/
class WriteStreamAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(TxStream* stream READ stream WRITE setStream)
    Q_PROPERTY(QString dest READ dest WRITE setDest)
    /*^jsdoc
     * Bytes to write. If unset or zero, all remaining data in the stream
     * will be written.
     * @member {Number} count
     * @default 0
     * @instance
     * @memberof ng.tx.WriteStreamAction
     ^jsdoc*/
    Q_PROPERTY(qint64 count READ count WRITE setCount)

   public:
    explicit WriteStreamAction(QObject* parent = 0) : TxAction(parent) {}
    void run() override;

    TxStream* stream() { return _stream; }
    void setStream(TxStream* const& stream) { _stream = stream; }
    QString dest() { return _dest; }
    void setDest(const QString& dest) { _dest = dest; }
    qint64 count() { return _count; }
    void setCount(qint64 count) { _count = count; }

   private:
    TxStream* _stream;
    QString _dest;
    qint64 _count = 0;
    qint64 _progress = 0;

    qint64 calcSize() override { return _count; }
};