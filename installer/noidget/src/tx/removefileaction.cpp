#include "removefileaction.h"
#include "installerapplication.h"
#include "fs.h"
#include <QFile>

void RemoveFileAction::run() {
    QString expandedPath = ngApp->globalFs()->expandedPath(_path);
    if (!ngApp->globalFs()->pathIsFile(expandedPath)) {
        emit log(
            QString("Not deleting %1: not an existing file").arg(expandedPath));
    } else {
        QFile::remove(expandedPath);
        emit log(QString("Deleted %1").arg(expandedPath));
    }
}