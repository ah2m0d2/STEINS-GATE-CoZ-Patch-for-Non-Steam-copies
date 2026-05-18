#pragma once

#include "txaction.h"
#include <QObject>
#include <QScriptable>
#include <QVector>

class Fs;

/*^jsdoc
 * Action to copy files
 * 
 * `src` can be a directory (in which case the directory is copied to `dest` whole),
 * `/path/to/some/directory/*` (in which case all of its files and subdirectories
 * are copied to the `dest` directory), or a single file (copied into `dest` if that 
 * refers to a directory, or named `dest` otherwise).
 * 
 * Destination directory (and parents) do not need to exist and existing files
 * will be overwritten.
 * 
 * @class CopyFilesAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} src
 * @property {string} dest
 * @property {bool} deferredCollect - default false. If true, source will only be searched right before the action runs, instead of before the transaction starts. Useful for copying files created during installation.
 ^jsdoc*/
class CopyFilesAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(QString src READ src WRITE setSrc)
    Q_PROPERTY(QString dest READ dest WRITE setDest)
    Q_PROPERTY(
        bool deferredCollect READ deferredCollect WRITE setDeferredCollect)

   public:
    explicit CopyFilesAction(QObject* parent);
    void prepare() override;
    void run() override;

    QString src() { return _src; }
    void setSrc(const QString& src) { _src = src; }
    QString dest() { return _dest; }
    void setDest(const QString& dest) { _dest = dest; }
    bool deferredCollect() { return _deferredCollect; }
    void setDeferredCollect(bool deferredCollect) {
        _deferredCollect = deferredCollect;
    }

   private:
    QString _src, _dest;
    bool _deferredCollect = false;
    QVector<QString> _srcPaths;
    qint64 _progress = 0;
    qint64 _subactionProgress = 0;
    Fs* _fs;

    qint64 calcSize() override;
    qint64 calcSubactionCount() override;
    void copySingleFile(const QString& src, const QString& dest);
    void collect();
};