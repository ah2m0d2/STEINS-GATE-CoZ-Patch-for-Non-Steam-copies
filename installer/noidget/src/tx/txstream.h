#pragma once

#include <QObject>
#include <QScriptable>

/*^jsdoc
 * Abstract stream (builder) to be operated on with various actions
 *
 * @class TxStream
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 ^jsdoc*/
class TxStream : public QObject, protected QScriptable {
    Q_OBJECT
    Q_PROPERTY(bool isOpen READ isOpen)

   public:
    explicit TxStream(QObject* parent = 0) : QObject(parent) {}
    virtual ~TxStream() {}
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void seek(qint64 count) = 0;
    virtual void seekAbs(qint64 pos) = 0;
    virtual qint64 read(void* buffer, qint64 max) = 0;

    bool isOpen() const { return _isOpen; }

   protected:
    bool _isOpen = false;
};