#pragma once

#include <QObject>
#include <QScriptable>

/*^jsdoc
 * Abstract base class for individual operations in a transaction
 *
 * @class TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 ^jsdoc*/
class TxAction : public QObject, protected QScriptable {
    Q_OBJECT

   public:
    explicit TxAction(QObject* parent = 0) : QObject(parent) {}
    virtual ~TxAction() {}
    virtual void prepare() {}
    virtual void run() = 0;

    qint64 size() { return _sizeOverridden ? _sizeOverride : calcSize(); }
    qint64 subactionCount() {
        return _sizeOverridden ? _sizeOverride : calcSubactionCount();
    }
    /*^jsdoc
     * Explicitly specify operation size in bytes or subactions (for progress bar)
     *
     * Passing 0 is allowed (this operation will then not count towards the progress
     * bar). If this is never set (and the action supports it), operation size
     * is calculated given its inputs before the transaction is committed.
     * 
     * Setting this can speed up the installation as the size does not have to
     * be computed (and likely read from disk) in that case.
     *
     * @method overrideSize
     * @param {Number} newSize
     * @memberof ng.tx.TxAction
     * @instance
     ^jsdoc*/
    Q_INVOKABLE void overrideSize(qint64 newSize) {
        _sizeOverridden = true;
        _sizeOverride = newSize;
    }

   signals:
    void log(const QString& text, bool fileOnly = false);
    void progress(qint64 progress);
    void subactionProgress(qint64 progress);

   public slots:
    void cancel() { _isCancelled = true; }

   protected:
    virtual qint64 calcSize() { return 0; }
    virtual qint64 calcSubactionCount() { return 1; }
    bool _isCancelled = false;

   private:
    bool _sizeOverridden = false;
    qint64 _sizeOverride;
};