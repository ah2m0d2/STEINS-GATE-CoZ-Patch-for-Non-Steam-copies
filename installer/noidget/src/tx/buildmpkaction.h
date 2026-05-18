#pragma once

#include "txaction.h"
#include <QObject>
#include <QScriptable>
#include <QVector>

class TxStream;

/*^jsdoc
 * Action to build an MPK archive
 * 
 * Streams added to the archive do not need to be opened first. If they were not
 * previously opened, BuildMpkAction will open and close them. If they were opened,
 * BuildMpkAction will keep them open.
 * 
 * BuildMpkAction always reads streams to the end.
 * 
 * Display sizes must be specified for progress indication to happen, BuildMpkAction
 * cannot estimate source sizes.
 * 
 * @class BuildMpkAction
 * @extends ng.tx.TxAction
 * @memberof ng.tx
 * @static
 * @toplevel
 * @hideconstructor
 * @property {string} path - destination file path
 ^jsdoc*/
class BuildMpkAction : public TxAction {
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath)

   public:
    explicit BuildMpkAction(QObject* parent = 0) : TxAction(parent) {}
    void run() override;

    QString path() { return _path; }
    void setPath(const QString& path) { _path = path; }

    void addEntry(int id, const QString& name, const QString& srcPath,
                  qint64 displaySize = 0);
    void addEntry(int id, const QString& name, TxStream* source,
                  qint64 displaySize = 0);

   private:
    QString _path;
    struct MpkMetaEntry {
        int id;
        TxStream* source;
        char name[0xE0];  // null-terminated, ISO 8859-1 (presumably)
        qint64 displaySize;

        qint64 dataOffset;
        qint64 dataSize;
    };
    QVector<MpkMetaEntry> _entries;
    qint64 _progress = 0;

    qint64 calcSize() override;
};