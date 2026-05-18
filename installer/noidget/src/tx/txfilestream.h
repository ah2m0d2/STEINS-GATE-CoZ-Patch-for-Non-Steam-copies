#pragma once

#include "txstream.h"
#include <QObject>
#include <QFile>

/*^jsdoc
 * File (read) stream builder
 *
 * @class TxFileStream
 * @extends ng.tx.TxStream
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} inPath - path to file to read from
 ^jsdoc*/
class TxFileStream : public TxStream {
    Q_OBJECT
    Q_PROPERTY(QString inPath READ inPath WRITE setInPath)

   public:
    explicit TxFileStream(QObject* parent = 0) : TxStream(parent) {}
    ~TxFileStream();
    void open() override;
    void close() override;
    void seek(qint64 count) override;
    void seekAbs(qint64 pos) override;
    qint64 read(void* buffer, qint64 max) override;

    QString inPath() { return _inPath; }
    void setInPath(const QString& inPath) { _inPath = inPath; }

   private:
    QString _inPath;
    QFile* _inFile = 0;

    void doClose();
};