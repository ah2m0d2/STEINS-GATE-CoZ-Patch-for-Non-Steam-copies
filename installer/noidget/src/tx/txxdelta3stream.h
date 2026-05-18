#pragma once

#include "txstream.h"
#include <QObject>
#include <QFile>
extern "C" {
#include <xdelta3.h>
}

/*^jsdoc
 * Xdelta3 decode stream builder
 *
 * Xdelta3 streams are **not seekable**.
 * 
 * @class TxXdelta3Stream
 * @extends ng.tx.TxStream
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {ng.tx.TxStream} srcStream - (unopened) stream of input file to be patched
 * @property {ng.tx.TxStream} diffStream - (unopened) stream of VCDIFF patch
 ^jsdoc*/
class TxXdelta3Stream : public TxStream {
    Q_OBJECT
    Q_PROPERTY(TxStream* srcStream READ srcStream WRITE setSrcStream)
    Q_PROPERTY(TxStream* diffStream READ diffStream WRITE setDiffStream)

   public:
    explicit TxXdelta3Stream(QObject* parent = 0) : TxStream(parent) {}
    ~TxXdelta3Stream();
    void open() override;
    void close() override;
    void seek(qint64 count) override;
    void seekAbs(qint64 pos) override;
    qint64 read(void* buffer, qint64 max) override;

    TxStream* srcStream() { return _srcStream; }
    void setSrcStream(TxStream* const& srcStream) { _srcStream = srcStream; }
    TxStream* diffStream() { return _diffStream; }
    void setDiffStream(TxStream* const& diffStream) {
        _diffStream = diffStream;
    }

   private:
    TxStream *_srcStream = 0, *_diffStream = 0;
    void *_srcBuffer = 0, *_diffBuffer = 0;
    qint64 _srcPos;

    void* _outBuffer = 0;
    qint64 _outBytesAvailable = 0;

    bool _xd3NeedsInput;
    bool _inputAvailable;

    xd3_stream _xd3stream;
    xd3_config _xd3config;
    xd3_source _xd3source;

    void readUntilOutput();
    void doClose();
};