#pragma once

#include "txstream.h"
#include <QObject>
#include <QFile>
#include <QDataStream>

/*^jsdoc
 * MPK (read) stream builder. Only uncompressed MPKv2 supported for now.
 *
 * @class TxMpkInputStream
 * @extends ng.tx.TxStream
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} inPath - path to file to read from
 * @property {Number} entry - entry ID
 ^jsdoc*/
class TxMpkInputStream : public TxStream {
    Q_OBJECT
    Q_PROPERTY(QString inPath READ inPath WRITE setInPath)
    Q_PROPERTY(quint32 entry READ entry WRITE setEntry)

   public:
    explicit TxMpkInputStream(QObject* parent = 0) : TxStream(parent) {}
    ~TxMpkInputStream();
    void open() override;
    void close() override;
    void seek(qint64 count) override;
    void seekAbs(qint64 pos) override;
    qint64 read(void* buffer, qint64 max) override;

    QString inPath() { return _inPath; }
    void setInPath(const QString& inPath) { _inPath = inPath; }
    quint32 entry() { return _entry; }
    void setEntry(quint32 entry) { _entry = entry; }

   private:
    QString _inPath;
    quint32 _entry;

    qint64 _size;
    qint64 _offset;
    QFile* _inFile = 0;
    QDataStream* _inStream = 0;

    void doClose();
};